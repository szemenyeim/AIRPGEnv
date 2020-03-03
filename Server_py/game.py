import Server_py.gui as gui
from Server_py.tcp_server import Server
from Server_py.characters import Monster, Hero
import threading, time
import Server_py.global_vars as gv


class Game():
    delta_pos = 1

    def __init__(self):
        # todo: write param parser
        self.__window_name = "AiRPG"
        self.__image = r"S:\Onlab\AI_RPG_Environment\map_min.jpg"
        self.interface = gui.GUI(self.__image, self.__window_name)

        gv.server_lock = threading.Lock()
        gv.out_lock = threading.Lock()
        gv.character_lock = threading.Lock()

        self.server = threading.Thread(target=self.server)
        self.server.daemon = True
        self.server.start()

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

    def find_nearest(self, player):
        nearest = None
        min_dist = 5
        for i in gv.characters:
            act_dist = 0
            if i.position[0] != player.position[0] or i.position[1] != player.position[1]:
                act_dist = player.calc_dist(i)
                if act_dist < min_dist:
                    nearest = i
        return nearest

    def key_handler(self, key_pressed, player):
        print("BOOOM")
        if key_pressed is ' ':
            enemy = self.find_nearest(player)
            if enemy is not None:
                player.attack(enemy)
            # print("oop")
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
            monster = Monster(self.interface, 1)
            gv.characters.append(monster)
            gv.villians.append(monster)

    @staticmethod
    def monster_handler():
        while True:
            gv.server_lock.acquire()
            for monster in gv.villians:
                monster.engage()
            time.sleep(1)
            gv.server_lock.release()

    @staticmethod
    def server():
        server = Server()
        server.run_server()
        return 0


def main():
    game = Game()
    game.add_monster(game.interface, 50)

    while True:
        # gv.server_lock.acquire()

        if len(gv.mailbox_in) != 0:
            message = gv.mailbox_in.pop()

            print(message, len(gv.mailbox_in))

            if "JOINED" in message:
                # character gets deleted i its exist
                for fella in gv.heroes:
                    if fella.name == message[0]:
                        gv.characters.remove(fella)
                        gv.heroes.remove(fella)
                        break

                # Create new hero
                hero = Hero(game.interface, message[0])
                gv.heroes.append(hero)
                gv.characters.append(hero)
            # refresh the state of the hero
            elif len(message) == 2:

                key = int(message[1])

                for fella in gv.heroes:
                    if fella.name == message[0]:
                        game.key_handler(chr(key), fella)
        # gv.server_lock.release()
        game.invalidate()
        game.draw()

        # time.sleep(.1)

if __name__ == "__main__":
    main()
