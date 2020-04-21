import socket
import sys
import threading
import time

import numpy as np

from Environment.characters import Monster, Hero, NPC, Quest
from Environment.gui import GUI

"""GLOBAL VARIABLES"""
IP = '127.0.0.1'
PORT = 54000
PLAYERNAME = 'King_Arthur'

ACTION_LOOKUP = {
    0: ord('w'),
    1: ord('a'),
    2: ord('s'),
    3: ord('d'),
    4: ord(' '),
    5: ord('f')
}



class Environment():
    def __init__(self, mode = 'human', playername = PLAYERNAME, ip = IP, port = PORT):
        """
        Initialization of the environment and
        :param playername: Name of the player character
        :param ip: IP address of the server to connect
        :param port: port address of the server to connect
        """

        # operational variables for the environment
        self.ip = ip
        self.port =port
        self.playerName = playername
        self.DEATH = False
        self.__window_name = "AiRPG"
        self.__image = r"..\map_min.jpg"
        self.__client = None
        self.__key = threading.Lock()
        self.__mode = mode

        # variables to handle the player and game_state
        self.__characters ={}
        self.__my_xpos = self.__my_ypos = self.__my_id = -1
        self.__hp_changed = self.__xp_got = 0
        self.__quest = Quest()

        # setting up client thread
        self.__client_thread = threading.Thread(target=self.__communication, args=(self.ip, self.port, self.playerName))
        self.__client_thread.daemon = True
        self.__client_thread.start()

        self.gui = GUI(img4map=self.__image, window_name=self.__window_name)

        # making the whole environment gym compatible
        if self.__mode == 'gym':
            from gym import spaces
            super(Environment, self).__init__()
            self.action_space = spaces.Discrete(len(ACTION_LOOKUP))
            # using image input
            # self.observation_space = spaces.Tuple((
            #     spaces.Box(low=0, high=255, shape=(64, 64, 3), dtype=np.uint8),
            #     spaces.Box(low=0, high=255, shape=(16, 16, 3), dtype=np.uint8)
            # ))

            self.observation_space = spaces.Box(low=0, high=255, shape=(2,), dtype=np.uint8)

    def __process_msg(self, message):
        id = None
        """
        :param message: message received from the server
        """
        # aquire the mutex

        # print(message)
        # split the messages for the information
        characters_from_msg = message.split("\n")
        for msg_it in characters_from_msg:
            params = msg_it.split(";")
            # handle quest
            try:
                id = int(params[0])
            except:

                pass
            if len(params) >= 2:
                if id == self.__my_id and params[1] == "Targets":
                    self.__quest.targets.clear()
                    self.__quest.xp = 0
                    self.gui.targets = np.zeros((16, 16))
                    for i in params[2:]:
                        if i == params[-1]:
                            # store the bounty
                            self.__quest.xp = int(i)
                        else:
                            # store targets
                            self.__quest.targets.append(int(i))
                    continue

                # handle death of characters
                if len(params) == 2 and params[1] == "DEAD":
                    if id == self.__my_id:
                        self.gui.clear_map()
                        self.DEATH = True
                    elif id in self.__quest.targets:
                        self.__quest.targets.remove(id)
                        self.gui.targets = np.zeros((16, 16))
                    try:
                        self.__characters.pop(int(params[0]))

                    except:
                        # fella already removed
                        pass
                        # print("already removed")

            # handle errors
            if len(params) != 8:
                continue

            # parse the parameters of the character
            try:
                id = int(params[0])
                x_pos = int(params[2])
                y_pos = int(params[3])
                level = int(params[4])
                currentHP = int(params[5])
                currentXP = float(params[6])
                marked = (params[7] == "True")
            except:
                continue
            name = params[1]

            is_there = self.__characters.__contains__(id)

            # if there is a new character in the game we create it
            if not is_there:
                self.__key.acquire()
                if name == 'Monster':
                    self.__characters[id] = Monster(x_pos, y_pos, level, currentXP, currentHP,marked)
                elif name == 'NPC':
                    self.__characters[id] = NPC(x_pos, y_pos, level, currentXP, currentHP, marked)
                else:
                    self.__characters[id] = Hero(x_pos, y_pos, level, currentXP, currentHP, marked)
                    if name == self.playerName:
                        self.__my_xpos = int(x_pos)
                        self.__my_ypos = int(y_pos)
                        self.__my_id = id
                self.__key.release()
            # if we knew that this character exist
            elif is_there:

                # if the character is the player we refresh the states
                if self.__my_id == id:
                    self.__hp_changed = currentHP - self.__characters[self.__my_id].curr_HP
                    self.__xp_got = currentXP - self.__characters[self.__my_id].XP
                    self.__my_xpos = int(x_pos)
                    self.__my_ypos = int(y_pos)
                    # print("player_moved")

                # todo: unmark fellas
                self.__characters[id].position = (x_pos, y_pos)
                self.__characters[id].XP = currentXP
                self.__characters[id].curr_HP = currentHP
                self.__characters[id].marked = marked



    def render(self):
        self.gui.clear_window()
        # todo: lock required
        # self.__key.acquire()
        for id in self.__characters:
            self.__characters[id].draw(self.gui)
            if self.__characters[id].marked:
                self.gui.draw_mark(self.__characters[id].position[0], self.__characters[id].position[1])
            if id in self.__quest.targets:
                self.gui.draw_targets(self.__characters[id].position[0], self.__characters[id].position[1])
        # self.__key.release()

        self.gui.process_window(self.__my_xpos, self.__my_ypos)
        if self.__mode == 'human':
            self.gui.show_window()
        return self.gui.current_game, self.gui.minimap

    def connect2server(self, ipAddress, port, player):
        # create an ipv4 socket object
        self.__client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # connect the client
        try:
            self.__client.connect((ipAddress, int(port)))
        except:
            return -1

        self.__client.setblocking(False)

        # delete my gamestate representation
        self.__characters.clear()
        self.DEATH = False
        # send the name of the player to the server
        join_msg = player + ":JOINED\n\r"
        self.__client.send(join_msg.encode())
        # print(join_msg)

        return 1

    def __communication(self, ipAddress, port, player):
        self.connect2server(self.ip, self.port, self.playerName)
        received = 0
        while True:

            # listening the incoming messages
            try:
                received = self.__client.recv(4096)
            except:
                pass
            if received:
                message = received.decode()
                # print(message)
                self.__process_msg(message)
            received = None

            time.sleep(0.01)

    def step(self, action):

        game = 1  # game state: 1: game ongoing    0: game over    -1: conncetion lost
        reward = 0
        info = {}
        # forward the taken action to the server
        self.__key.acquire()
        if self.__mode == 'gym':
            action = ACTION_LOOKUP[action]
        telegram = self.playerName + ":" + str(action) + "\n\r"

        if action > 0:
            try:
                self.__client.send(telegram.encode())
            except:
                print("505: SERVER ERROR!\nSend failed!")
                game = self.connect2server(self.ip, self.port, self.playerName)

        self.__key.release()

        if self.__xp_got:
            reward += self.__xp_got
            self.__xp_got = 0
        if self.__hp_changed:
            reward += self.__hp_changed

            self.__hp_changed = 0

        #

        try:
            y = self.__characters[self.__my_id]
            new_state, map = self.render()

        except:
            new_state = self.gui.current_game[0:64, 0:64]
            map = np.zeros((16, 16, 3))
            game = 0
            # print(game,"GAME OVER")
            try:
                self.__characters.pop(self.__my_id)
            except:
                pass
            if self.DEATH:
                game = self.connect2server(self.ip, self.port, self.playerName)
                self.DEATH = False

        return new_state, map, reward, game, info

    def reset(self):
        try:
            new_state, map = self.render()
        except:
            new_state, map = self.render()
        return new_state, map

    def close(self):
        self.close = True
        sys.exit()


if __name__ == "__main__":

    if(len(sys.argv) > 1):

        env = Environment(playername=sys.argv[1])
    else:
        env = Environment()

    while True:
        env.step(action=env.gui.get_key_pressed())
        try:
            env.render()
        except:
            "upps"
        time.sleep(0.01)
