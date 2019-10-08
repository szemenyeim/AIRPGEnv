import socket
import threading, time
from typing import Dict, Any

from gui import GUI
from characters import Monster, Hero, Character


class Environment:

    def __init__(self, playername, ip, port):
        """
        Initialization of the environment and
        :param playername: Name of the player character
        :param ip: IP address of the server to connect
        :param port: port address of the server to connect
        """

        # operational variables for the environment
        self.playerName = playername
        self.__window_name = "AiRPG"
        self.__image = "map2.jpg"
        self.__client = None
        self.__key = threading.Lock()

        # variables to handle the player and game_state
        self.__characters ={}
        self.__my_xpos = self.__my_ypos = self.__my_id = 0
        self.__hp_changed = self.__xp_got = 0

        client_thread = threading.Thread(target=self.communication, args=(ip, port, playername))
        client_thread.start()

        self.gui = GUI(img4map=self.__image, window_name=self.__window_name)

    def process_msg(self, message):
        """
        :param message: message received from the server
        """

        # aquire the mutex
        self.__key.acquire()

        # split the messages for the information
        characters_from_msg = message.split("\n")
        for msg_it in characters_from_msg:
            params = msg_it.split(";")

            # handle death
            if len(params) == 2 and params[1] == "DEAD":
                self.__characters.pop(int(params[0]))

            # TODO: HANDLE GAMEOVER

            # handle errors
            if len(params) != 7:
                continue

            # parse the parameters of the character
            try:
                id = int(params[0])
                x_pos = int(params[2])
                y_pos = int(params[3])
                level = int(params[4])
                currentHP = int(params[5])
                maximumHP = int(params[6])
            except:
                continue
            name = params[1]

            is_there = self.__characters.__contains__(id)

            # if there is a new character in the game we create it
            if not is_there:
                if name == 'Monster':
                    self.__characters[id] = Monster(x_pos, y_pos, level, maximumHP, currentHP)

                else:
                    self.__characters[id] = Hero(x_pos, y_pos, level, maximumHP, currentHP)
                    if name == self.playerName:
                        self.__my_xpos = int(x_pos)
                        self.__my_ypos = int(y_pos)
                        self.__my_id = id

            # if we knew that this character exist
            elif is_there:

                # if the character is the player we refresh the states
                if self.__my_id == id:
                    self.__hp_changed = currentHP - self.__characters[self.__my_id].curr_HP
                    # self.__xp_got = pass
                    self.__my_xpos = int(x_pos)
                    self.__my_ypos = int(y_pos)

                self.__characters[id].position = (x_pos, y_pos)
                self.__characters[id].max_HP = maximumHP
                self.__characters[id].curr_HP = currentHP

        self.__key.release()

    def render(self, interface: GUI):
        interface.clear_window()
        for id in self.__characters:
            self.__characters[id].draw(interface)
        interface.show_window(self.__my_xpos, self.__my_ypos)

    def communication(self, ipAddress, port, player):
        # create an ipv4 socket object
        self.__client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # connect the client
        print(port)
        self.__client.connect((ipAddress, int(port)))
        self.__client.setblocking(False)

        # send the name of the player to the server
        self.__client.send(player.encode())
        received = 0
        while True:

            # listening the incoming messages
            try:
                received = self.__client.recv(4096)
            except:
                pass
            if received:
                message = received.decode()
                print(message)
                self.process_msg(message)
            received = None

            time.sleep(0.01)

    def step(self, action):
        game = 1  # game state: 1: game ongoing    0: game over    -1: conncetion lost
        reward = 0

        hp_lost = 0
        xp_got = 0

        # forward the taken action to the server
        self.__key.acquire()
        telegram = self.playerName + ":" + str(action)

        if action > 0:
            try:
                self.__client.send(telegram.encode())
            except:
                print("Send failed!")

        self.__key.release()

        if xp_got:
            reward += xp_got
        if hp_lost:
            reward -= hp_lost
        try:
            new_state = self.__characters[self.__my_id]
        except:
            new_state = None

        return new_state, reward, game


if __name__ == "__main__":

    env = Environment('Apa', '127.0.0.1', 54000)

    while True:
        env.step(action=env.gui.get_key_pressed())
        env.render(env.gui)

        time.sleep(0.01)
