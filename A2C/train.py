import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torch.distributions import Categorical
from tensorboardX import SummaryWriter
import csv, time, os, random, numpy,sys
sys.path.insert(0,'.')
import Environment.environment as rpg
from multiprocessing_env import SubprocVecEnv, VecPyTorch

"""
Credits:
    github.com/sweetice/Deep-reinforcement-learning-with-pytorch
    https://github.com/raillab/a2c
"""

use_cuda = torch.cuda.is_available()  # Check nVidia GPU
device = torch.device("cuda" if use_cuda else "cpu")  # use GPU for training if it is available

# envs = rpg.Environment('gym', "Neo")

class A2C(torch.nn.Module):
    def __init__(self, input_num, output_num, hidden_layers):
        """
        Advanced Actro-Critic (A2C) Network
        """
        super(A2C, self).__init__()
        self.hidden = hidden_layers
        # TODO: improve with FeatureEncoder

        # Use the nn package to define our model as a sequence of layers. nn.Sequential
        # is a Module which contains other Modules, and applies them in sequence to
        # produce its output. Each Linear Module computes output from input using a
        # linear function, and holds internal Tensors for its weight and bias.


        self.features = nn.Sequential(
            nn.Conv2d(in_channels=3, out_channels = 64, kernel_size= 3, stride = 1),
            nn.ReLU(),
            nn.Conv2d(in_channels=64, out_channels=64, kernel_size=3, stride= 1),
            nn.ReLU(),
            nn.Conv2d(in_channels=64, out_channels=64, kernel_size=3, stride=1),
            nn.ReLU(),
        )
        self.features2 = nn.Sequential(
            nn.Conv2d(in_channels=3, out_channels=16, kernel_size=3, stride=1),
            nn.ReLU(),
            nn.Conv2d(in_channels=16, out_channels=16, kernel_size=3, stride=1),
            nn.ReLU(),
            nn.Conv2d(in_channels=16, out_channels=16, kernel_size=3, stride=1),
            nn.ReLU(),
        )

        # todo feature 2 torch.cat()
        # TODO: GRU Cell

        self.actor = nn.Linear(hidden_layers, output_num)
        self.critic = nn.Linear(hidden_layers, 1)
        self.linear = nn.Linear(216896, self.hidden)

        for m in self.features:
            if isinstance(m, nn.Conv2d):
                nn.init.orthogonal_(m.weight, nn.init.calculate_gain('relu'))
                nn.init.constant_(m.bias, 0.0)

        nn.init.orthogonal_(self.critic.weight)
        nn.init.constant_(self.critic.bias, 0.0)

        nn.init.orthogonal_(self.actor.weight, 0.01)
        nn.init.constant_(self.actor.bias, 0.0)

        nn.init.orthogonal_(self.linear.weight)
        nn.init.constant_(self.linear.bias, 0.0)

    # TODO: Recurrent LSTM buffers but why

    def forward(self, state, minimap):
        # Convert state to float and change the shape
        state = state.reshape ([2, 3, 64, 64])
        minimap = minimap.reshape ([2, 3, 16, 16])
        state = state.to(device) / 255.
        state = self.features(state)
        shape = state.shape
        state = state.view(shape[0], -1)

        minimap = minimap.to(device) / 255.
        minimap = self.features2(minimap)
        shape = minimap.shape
        minimap = minimap.view(shape[0], -1)

        state = torch.cat((state, minimap), dim=1)
        state = self.linear(state)
        state = F.relu(state)

        policy = self.actor(state)  # a list with the probability of each action over the action space
        action_prob = F.softmax(policy, dim=1)
        value = self.critic(state.squeeze())

        """Calculate action"""
        # convert policy outputs into probabilities
        policy_distribution = Categorical(action_prob)

        return policy_distribution, value


def compute_returns(next_value, rewards, masks, gamma=0.99):
    """
        Training code. Calcultes actor and critic loss and performs backprop.
    """
    R = next_value
    returns = []
    for step in reversed(range(len(rewards))):
        # Discounted Sum of Future Rewards + reward for the given state
        R = rewards[step] + gamma * R * masks[step]
        returns.insert(0, R)
    return returns


if __name__ == "__main__":
    """start the server"""
    # os.system(r"s:\Onlab\AI_RPG_Environment\Server\x64\Release\Server.exe")
    """Create environment for the agent"""
    # n_cpu = 4  # Threads for training
    current_time = time.ctime ().replace (":", "_")
    log_dir = "logs/A2C/{}".format(current_time)
    # tensorboard
    writer = SummaryWriter(log_dir = log_dir)

    # csv
    logfile_name = "{}/train_log.csv".format (log_dir)
    with open (logfile_name, 'w+', newline='') as f:
        csv_writer = csv.writer (f, delimiter=";")
        csv_writer.writerow (['update', 'running_loss','reward', 'loss', 'actor_loss', 'critic_loss', 'entropy_loss','time'])
    # creating environments for trainnig
    envs = SubprocVecEnv([
        lambda: rpg.Environment('gym', "Neo", visu=True),
        lambda: rpg.Environment('gym', "Morpheus"),
        # lambda: rpg.Environment('gym', "Trinity"),
        # lambda: rpg.Environment('gym', "Oracle", visu="True"),
        # lambda: rpg.Environment('gym', "Cypher"),
        # lambda: rpg.Environment('gym', "Tank"),
        # lambda: rpg.Environment('gym', "Agent_Smith"),
        # lambda: rpg.Environment('gym', "Dozer")
    ])


    envs = VecPyTorch(envs, device)

    """ training """
    # getting the size of possible inputs and outputs
    num_inputs = (64, 64, 3)  # envs.observation_space.shape
    num_outputs = envs.action_space.n
    # print(num_inputs, num_outputs)
    """hyper params:"""
    hidden_size = 256
    lr = 1e-3  # learning rate
    num_steps = 5

    """ Init the model """
    model = A2C(num_inputs, num_outputs, hidden_size).to(device)  # initialisation the model
    optimizer = optim.Adam(model.parameters())  # optimisation the model w/ Adaptive Momemt Estimation

    model.load_state_dict(torch.load(r'D:\Onlab\AI_RPG_Environment\model.pth'))
    model.eval()

    """ training parameters """
    # updates = args.num_frames // args.num_steps // args.num_envs
    seed_value = 11
    num_updates = 500001
    save_freq = 10000
    print_freq = 10
    test_rewards = []
    running_loss = 0
    running_reward = 0
    visu = True

    state, minimap = envs.reset()
    # random seed0
    os.environ['PYTHONHASHSEED'] = str(seed_value)
    random.seed(seed_value)
    numpy.random.seed(seed_value)
    torch.manual_seed(seed_value)
    torch.backends.cudnn.deterministic = True
    torch.backends.cudnn.benchmark = False

    for update in range(num_updates):
        log_probs = []
        values = []
        rewards = []
        masks = []
        entropies = []


        for _ in range(num_steps):
            # put the state through the Neural network
            # state = state.to(device)
            dist, value = model(state, minimap)
            values.append(value.squeeze())  # store values

            # sample the categorical distribution represented by these probabilities
            action = dist.sample()

            # make an action on the environment
            next_state, minimap, reward, done, _ = envs.step(action.unsqueeze(1))

            # store reward
            rewards.append(reward.to(device).squeeze())

            # calculate and store logarithm of probabilities
            log_prob = dist.log_prob(action)
            log_probs.append(log_prob)

            # calculate and store logarithm of probabilities
            entropy = dist.entropy()
            entropies.append(entropy)

            # calculate and store mask
            mask = torch.from_numpy(1.0 - done).to(device).float()
            masks.append(mask)

            # refresh state
            state = next_state

        with torch.no_grad():
            _, next_value = model(next_state, minimap)
            returns = compute_returns(next_value.squeeze(), rewards, masks)
            returns = torch.cat(returns)


        log_probs = torch.cat(log_probs)
        values = torch.cat(values)
        entropies = torch.cat(entropies)
        rewards = torch.cat(rewards).detach()

        advantage = returns - values

        actor_loss = (log_probs * advantage.detach()).mean()
        critic_loss = advantage.pow(2).mean()
        entropy_loss = entropies.mean()

        loss = 1.0 * actor_loss + 0.5 * critic_loss - 0.001 * entropy_loss
        
        optimizer.zero_grad()
        loss.backward()
        torch.nn.utils.clip_grad_norm_(model.parameters(), max_norm=0.5)
        optimizer.step()

        running_loss += loss.item()
        running_reward += reward.mean().item()
        
        if update % print_freq ==0:
            print("********************************************************")
            print("update: {0}".format(update))
            print("mean/median reward: {:.1f}/{:.1f}".format(rewards.mean(), rewards.median()))
            print("min/max reward: {:.1f}/{:.1f}".format(rewards.min(), rewards.max()))
            print("actor loss: {:.5f}, critic loss: {:.5f}, entropy: {:.5f}"
                  .format(actor_loss.item(), critic_loss.item(), entropy_loss.item()))
            print("Loss: {0}".format(loss))
            print("********************************************************")

        # show data in tensorflow
        writer.add_scalar('Loss/Loss',loss ,update)
        writer.add_scalar('Loss/Actor Loss',actor_loss ,update)
        writer.add_scalar('Loss/Critic Loss',critic_loss ,update)
        writer.add_scalar('Loss/Entropy',entropy_loss,update)
        writer.add_scalar('Reward/Running Reward', running_reward, update)
        writer.add_scalar('Reward/Min',reward.min(),update)
        writer.add_scalar('Reward/Max',reward.max(),update)
        writer.add_scalar('Reward/Mean',reward.mean(),update)
        writer.add_scalar('Reward/Median',reward.median(),update)
        writer.add_scalar('Reward/Sum',reward.sum(),update)

        with open (logfile_name, 'a+', newline='') as f:
            csv_writer = csv.writer (f, delimiter=";")
            csv_writer.writerow ([update, running_loss,reward.mean(), loss.item (),
                                  actor_loss.item (), critic_loss.item (), entropy_loss.item (), time.ctime(),
                                  rewards.mean(), reward.sum()])

        if save_freq > 0 and update % save_freq == 0:
            torch.save (model.state_dict (), '{}/kppmmodel.pth'.format(log_dir))

    print('hi')
    # writer.close()
    envs.close()
