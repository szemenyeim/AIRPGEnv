import Server_py.gui as gui
from Server_py.tcp_server import Server
from Server_py.characters import Monster, Hero, NPC
import threading, time
import Server_py.global_vars as gv


class Game():
    delta_pos = 1

    def __init__(self):
        # todo: write param parser
        self.__window_name = "AiRPG"
        self.__image = r"..\map_min.jpg"
        self.interface = gui.GUI(self.__image, self.__window_name)

        gv.server_lock = threading.Lock()
        gv.out_lock = threading.Lock()

        self.server_thread = threading.Thread(target=self.start_server)
        self.server_thread.daemon = True
        self.server_thread.start()

        self.monsters_move = threading.Thread(target=self.monster_handler)
        self.monsters_move.daemon = True
        self.monsters_move.start()

    @staticmethod
    def invalidate():
        """post the state of the game into the mailbox"""
        for fella in gv.characters:
            message = fella.parse()
            if gv.mailbox_out[fella.id].text != message:
                gv.mailbox_out[fella.id].text = message
                gv.mailbox_out[fella.id].sent = False

    def draw(self):
        for fella in gv.characters:
            fella.draw(self.interface)
        self.interface.show_window()

    def choose(self, player):
        # chose the nearest character to target if it is not chosen already
        if player.target:
            if player.calc_dist(player.target) > player.attack_range:
                player.target = None
                player.aim()
        for i in gv.characters:
            if (i.position[0] != player.position[0] or i.position[1] != player.position[1]) and i != player.target:
                act_dist = player.calc_dist(i)
                if act_dist < player.attack_range:
                    player.aim(i)
        # mark the target


    def key_handler(self, key_pressed, player):
        if key_pressed is 'f' or key_pressed is' ':
            self.choose(player)
            if player.target:
                if player.target.name == "NPC":
                    player.target.give_quest(player)
                    self.server.broadcast()
                elif key_pressed is ' ':
                    player.attack()            # print("oop")
        elif key_pressed is 'w':
            player.move(0, -self.delta_pos, self.interface)
        elif key_pressed is 's':
            player.move(0, self.delta_pos, self.interface)
        elif key_pressed is 'd':
            player.move(self.delta_pos, 0, self.interface)
        elif key_pressed is 'a':
            player.move(-self.delta_pos, 0, self.interface)

        player.explore(key_pressed)

    def add_monster(self, gui, count=50, level=1, hp=100):
        for i in range(count):
            monster = Monster(gui=self.interface, level=1)


    @staticmethod
    def monster_handler():
        while True:
            # gv.server_lock.acquire()
            for monster in gv.villians:
                monster.engage()
            time.sleep(1)
            # gv.server_lock.release()


    def start_server(self):
        self.server = Server()
        self.server.run_server()
        return 0

    def put_NPC(self, num=1):
        for _ in range(num):
            NPC(self.interface,level=1)


def main():
    game = Game()
    game.add_monster(game.interface, 50)
    game.put_NPC(10)

    while True:
        if len(gv.mailbox_in) != 0:
            message = gv.mailbox_in.pop()
            # print(message)
            # Create a new Hero
            if "JOINED" in message:
                # character gets deleted i its exist
                for fella in gv.heroes:
                    if fella.name == message[0]:
                        gv.characters.remove(fella)
                        gv.heroes.remove(fella)
                        break

                # Create new hero
                hero = Hero(game.interface, message[0])
                # print(hero.name)
                # Reset the messages
                for slot in gv.mailbox_out:
                    gv.mailbox_out[slot].sent = False
                    # print(gv.mailbox_out[slot].text)

            # refresh the state of the hero
            elif len(message) == 2:
                key = int(message[1])
                for fella in gv.heroes:
                    if fella.name == message[0]:
                        # print(key)
                        game.key_handler(chr(key), fella)

        # Refresh GUI and messages
        game.invalidate()
        game.draw()

        # time.sleep(.1)

if __name__ == "__main__":
    main()
