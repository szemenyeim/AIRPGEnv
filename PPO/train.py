import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torch.distributions import Categorical
import time, csv, random, numpy, os, sys
sys.path.insert(0,'.')
import Environment.environment as rpg
from multiprocessing_env import SubprocVecEnv, VecPyTorch
from tensorboardX import SummaryWriter

device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")


class Memory:
    def __init__(self):
        self.actions = []
        self.states = []
        self.maps = []
        self.logprobs = []
        self.rewards = []
        self.is_terminals = []

    def clear_memory(self):
        del self.actions[:]
        del self.states[:]
        del self.maps[:]
        del self.logprobs[:]
        del self.rewards[:]
        del self.is_terminals[:]



class ActorCritic(nn.Module):
    def __init__(self, input_num, output_num, hidden_layers):
        super(ActorCritic, self).__init__()

        self.hidden = hidden_layers
        # Use the nn package to define our model as a sequence of layers. nn.Sequential
        # is a Module which contains other Modules, and applies them in sequence to
        # produce its output. Each Linear Module computes output from input using a
        # linear function, and holds internal Tensors for its weight and bias.


        self.features = nn.Sequential(
            nn.Conv2d(in_channels=3, out_channels=32, kernel_size=3, stride=1),
            nn.ReLU(),
            nn.Conv2d(in_channels=32, out_channels=64, kernel_size=3, stride=1),
            nn.ReLU(),
            nn.Conv2d(in_channels=64, out_channels=64, kernel_size=3, stride=1),
            nn.ReLU(),
        )
        self.features2 = nn.Sequential(
            nn.Conv2d(in_channels=3, out_channels=16, kernel_size=3, stride=1),
            nn.ReLU(),
            nn.Conv2d(in_channels=16, out_channels=16, kernel_size=3, stride=1),
            nn.ReLU(),
            nn.Conv2d(in_channels=16, out_channels=16,kernel_size=3, stride=1),
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
        state = state.reshape(state.shape[0],3,64,64)
        minimap = minimap.reshape(state.shape[0],3,16,16)
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



class PPO:
    def __init__(self,  input_num, output_num, hidden_layers, lr, betas, gamma, K_epochs, eps_clip):
        self.lr = lr
        self.betas = betas
        self.gamma = gamma
        self.eps_clip = eps_clip
        self.K_epochs = K_epochs

        self.policy = ActorCritic( input_num, output_num, hidden_layers).to(device)
        self.optimizer = torch.optim.Adam(self.policy.parameters())
        self.policy_old = ActorCritic( input_num, output_num, hidden_layers).to(device)
        self.policy_old.load_state_dict(self.policy.state_dict())
        #
        self.MseLoss = nn.MSELoss()

    def update(self, memory):
        # Monte Carlo estimate of state rewards:
        rewards = []
        discounted_reward = 0
        for reward in reversed(memory.rewards):
            discounted_reward = reward + (self.gamma * discounted_reward)
            rewards.insert(0, discounted_reward)

        # Normalizing the rewards:
        rewards = torch.cat(rewards).to(device)
        rewards = (rewards - rewards.mean()) / (rewards.std() + 1e-5)

        # convert list to tensor
        old_states = torch.stack(memory.states).to(device).detach()
        old_actions = torch.stack(memory.actions).to(device).detach()
        old_logprobs = torch.stack(memory.logprobs).to(device).detach()
        old_maps = torch.stack(memory.maps).to(device).detach()

        # Optimize policy for K epochs:
        for _ in range(self.K_epochs):
            # Evaluating old actions and values :
            # with torch.no_grad():
            dist, state_value = self.policy(old_states.flatten(0,1),old_maps.flatten(0,1))
            logprobs = dist.log_prob(old_actions.flatten())
            dist_entropy = dist.entropy()

            state_values = torch.squeeze(state_value)

            # Finding the ratio (pi_theta / pi_theta__old):
            ratios = torch.exp(logprobs - old_logprobs.detach().flatten())

            # Finding Surrogate Loss:
            advantages = rewards - state_values.detach()
            surr1 = ratios * advantages
            surr2 = torch.clamp(ratios, 1 - self.eps_clip, 1 + self.eps_clip) * advantages
            actor_loss = -torch.min(surr1, surr2)
            critic_loss = self.MseLoss (state_values, rewards)
            loss = actor_loss + 0.5 * critic_loss - 0.001 * dist_entropy

            # take gradient step
            self.optimizer.zero_grad()
            loss.mean().backward()
            self.optimizer.step()

        # Copy new weights into old policy:
        self.policy_old.load_state_dict(self.policy.state_dict())
        return loss.mean(), actor_loss.mean(), critic_loss.mean(), dist_entropy.mean()

def main():
    current_time = time.ctime ().replace (":", "_")
    log_dir = "logs/PPO/{}".format (current_time)
    # tensorboard
    writer = SummaryWriter (log_dir=log_dir)

    # csv
    logfile_name = "{}/train_log.csv".format (log_dir)
    with open (logfile_name, 'w+', newline='') as f:
        csv_writer = csv.writer (f, delimiter=";")
        csv_writer.writerow (['update', 'running_loss', 'Reward', 'loss', 'actor_loss', 'critic_loss', 'entropy_loss', 'time'])

    ############## Hyperparameters ##############
    # env_name = "CartPole-v0"
    # creating environment
    envs = SubprocVecEnv([
        lambda: rpg.Environment('gym', "Neo"),
        lambda: rpg.Environment('gym', "Morpheus"),
        lambda: rpg.Environment('gym', "Trinity"),
        lambda: rpg.Environment('gym', "Oracle"),
        lambda: rpg.Environment('gym', "Cypher"),
        lambda: rpg.Environment('gym', "Tank"),
        lambda: rpg.Environment('gym', "Agent_Smith"),
        lambda: rpg.Environment('gym', "Dozer")
    ])

    env = VecPyTorch(envs, device)

    state_dim = (3, 64, 64)
    action_dim = env.action_space.n
    save_freq = 10000
    print_freq = 10
    max_episodes = 500001  # max training episodes
    max_timesteps = 5  # max timesteps in one episode
    n_latent_var = 256  # number of variables in hidden layer
    update_timestep = 15  # update policy every n timesteps
    lr = 0.002
    betas = (0.9, 0.999)
    gamma = 0.99  # discount factror
    K_epochs = 4  # update policy for K epochs
    eps_clip = 0.2  # clip parameter for PPO
    random_seed = 11
    actor_loss = 0
    critic_loss = 0
    entropy_loss = 0
    loss = 0
    #############################################

    if random_seed:
        os.environ ['PYTHONHASHSEED'] = str (random_seed)
        random.seed (random_seed)
        numpy.random.seed (random_seed)
        torch.manual_seed (random_seed)
        torch.backends.cudnn.deterministic = True
        torch.backends.cudnn.benchmark = False

    memory = Memory()
    ppo = PPO(state_dim, action_dim, n_latent_var, lr, betas, gamma, K_epochs, eps_clip)


    # logging variables
    running_reward = 0
    avg_length = 0
    timestep = 0
    state, minimap = env.reset ()

    # training loop
    for i_episode in range(1, max_episodes + 1):
        # state, minimap = env.reset()
        for t in range(max_timesteps):
            timestep += 1

            # Running policy_old:
            dist,_ = ppo.policy_old(state, minimap)
            action = dist.sample()
            state, minimap, reward, done, _ = env.step(action.unsqueeze(1))
            memory.states.append(state)
            memory.maps.append(minimap)
            memory.actions.append(action)
            memory.logprobs.append(dist.log_prob(action))

            # Saving reward and is_terminal:
            memory.rewards.append(reward.to(device).squeeze())
            memory.is_terminals.append(done)

            # update if its time
            if timestep % update_timestep == 0:
                loss, actor_loss,critic_loss, entropy_loss = ppo.update(memory)
                memory.clear_memory()
                timestep = 0

            running_reward += reward.mean().item()

        # avg_length += t

        # logging
        if i_episode % print_freq == 0:
            print ("********************************************************")
            print ("episode: {0}".format (i_episode))
            print ("mean/median reward: {:.1f}/{:.1f}".format (reward.mean (), reward.median ()))
            print ("min/max reward: {:.1f}/{:.1f}".format (reward.min (), reward.max ()))
            print ("actor loss: {:.5f}, critic loss: {:.5f}, entropy: {:.5f}"
                   .format (actor_loss, critic_loss, entropy_loss))
            print ("Loss: {0}".format (loss))
            print ("********************************************************")

        # show data in tensorflow
        writer.add_scalar ('Loss/Loss', loss, i_episode)
        writer.add_scalar ('Loss/Actor Loss', actor_loss, i_episode)
        writer.add_scalar ('Loss/Critic Loss', critic_loss, i_episode)
        writer.add_scalar ('Loss/Entropy', entropy_loss, i_episode)
        writer.add_scalar ('Reward/Running Reward', running_reward, i_episode)

        writer.add_scalar ('Reward/Min', reward.min(), i_episode)
        writer.add_scalar ('Reward/Max', reward.max(), i_episode)
        writer.add_scalar ('Reward/Mean', reward.mean(), i_episode)
        writer.add_scalar ('Reward/Median', reward.median(), i_episode)
        writer.add_scalar ('Reward/Sum', reward.sum(), i_episode)
        
        with open (logfile_name, 'a+', newline='') as f:
            csv_writer = csv.writer (f, delimiter=";")
            csv_writer.writerow ([i_episode, running_reward,reward.mean(), loss,
                                    actor_loss, critic_loss, entropy_loss, time.ctime()])

        if save_freq > 0 and i_episode % save_freq == 0:
            torch.save (ppo.policy.state_dict (), '{}/model.pth'.format (log_dir))
            torch.save (ppo.policy_old.state_dict (), '{}/model_old.pth'.format (log_dir))
            print("saved")

if __name__ == '__main__':
    main()
