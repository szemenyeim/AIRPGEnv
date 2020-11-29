import gui as gui
from tcp_server import Server
from characters import Monster, Hero, NPC, Character
import threading, time
import global_vars as gv
import os,csv, numpy, random, sys
sys.path.insert(0,'.')

class Game():
    delta_pos = 1

    def __init__(self,level):
        # todo: write param parser
        self.__window_name = "AiRPG Server View"
        self.__maps = [r"arctic.png", r"lake.png", r"valley.png"]
        self.current_map = -1
        self.level = level

        gv.server_lock = threading.Lock()
        gv.out_lock = threading.Lock()
        gv.con_lock = threading.Lock()

        self.server_thread = threading.Thread(target=self.start_server)
        self.server_thread.daemon = True
        self.server_thread.start()

        self.step = False
        self.monsters_move = threading.Thread(target=self.monster_handler)
        self.monsters_move.daemon = True
        self.monsters_move.start()


        current_time = time.ctime ().replace (":", "_")
        log_dir = "logs/game/{}".format (current_time)
        os.makedirs(log_dir)
        self.log = "{}/train_log.csv".format (log_dir)
        with open (self.log, 'w+', newline='') as f:
            csv_writer = csv.writer (f, delimiter=";")
            csv_writer.writerow (['time', 'map', 'game_level',
                                   'hero', 'hero_ level',
                                  'hero_xp', 'death_count',
                                  'killing_count', 'hunting_count'
                                  ])

        self.change_map ()
        self.server.broadcast()


    @staticmethod
    def invalidate():
        """post the state of the game into the mailbox"""
        for fella in gv.characters:
            message = fella.parse()
            gv.mailbox_out[fella.id].post_message(message)

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
        if key_pressed == 'f' or key_pressed ==' ':
            self.choose(player)
            if player.target in gv.characters:
                if player.target.name == "NPC":
                    player.target.give_quest(player)
                    self.server.broadcast()
                elif key_pressed == ' ':
                    player.attack()


        elif key_pressed == 'w':
            player.move(0, -self.delta_pos, self.interface)
        elif key_pressed == 's':
            player.move(0, self.delta_pos, self.interface)
        elif key_pressed == 'd':
            player.move(self.delta_pos, 0, self.interface)
        elif key_pressed == 'a':
            player.move(-self.delta_pos, 0, self.interface)

        player.explore(key_pressed)

    def add_monster(self, count=50):
        for _ in range(count):
            Monster(gui=self.interface, level=self.level, max_HP=self.level * 100)

    def monster_handler(self):
        while True:
            if self.step:
                for monster in gv.villians:
                    monster.engage()
                self.step = False
                time.sleep(1)



    def start_server (self):
        self.server = Server ()
        self.server.run_server ()
        return 0

    def put_NPC(self, num=1):
        for _ in range(num):
            NPC(self.interface,level=1)

    def process_message (self, message,sock):
            if "JOINED" in message:
            # character gets deleted i its exist
                for fella in gv.heroes:
                    if fella.name == message[0]:
                        gv.characters.remove(fella)
                        gv.heroes.remove(fella)
                        gv.out_lock.acquire()
                        gv.mailbox_out.pop(fella.id)
                        gv.out_lock.release()
                        break

                # Create new hero
                hero = Hero (self.interface, message [0])
                # Reset the messages
                for slot in gv.mailbox_out:
                    gv.mailbox_out[slot].sent = False
                gv.cons[sock] = hero

            # refresh the state of the hero
            elif len(message) == 2:
                key = int(message[1])
                for fella in gv.heroes:
                    if fella.name == message[0]:
                        # print(key)
                        self.key_handler(chr(key), fella)

    def change_map(self):
            self.level += 1
            self.current_map = (self.current_map + 1) % len (self.__maps)
            self.interface = gui.GUI (os.path.join (".", self.__maps [self.current_map]), self.__window_name)

            with open(self.log, 'a+', newline='') as f:
                csv_writer = csv.writer(f, delimiter=";")
                csv_writer.writerow([time.ctime(), self.level, self.__maps[self.current_map]])
                for hero_name in gv.connected:
                    csv_writer.writerow(['', '', '',
                                         hero_name, gv.connected[hero_name].level,
                                         gv.connected[hero_name].xp, gv.connected[hero_name].death,
                                         gv.connected[hero_name].killing, gv.connected[hero_name].hunting])

            gv.characters.clear()
            gv.out_lock.acquire()
            gv.mailbox_out.clear()
            gv.out_lock.release()
            gv.heroes.clear()
            gv.villians.clear()

            self.add_monster (50)
            self.put_NPC (10)
            gv.out_lock.acquire()
            gv.mailbox_out ['map'] = gv.mail ()
            gv.out_lock.release()
            gv.mailbox_out['map'].post_message(self.__maps[self.current_map]+";\n")


def main():
    level = 1
    game = Game(level)

    while True:
        # print(len(gv.characters), len(gv.villians), len(gv.heroes), gv.count_of_people)
        if len (gv.mailbox_in) != 0:
            gv.server_lock.acquire ()
            for con in gv.mailbox_in:
                if len (gv.mailbox_in [con]) != 0:
                    message = gv.mailbox_in [con].pop ()
                    game.process_message (message,con)
            game.step = True
            gv.server_lock.release ()

        gv.con_lock.acquire()
        for c in gv.cons:
            if gv.cons[c] not in gv.heroes:
                gv.cons[c] = None
        gv.con_lock.release()

        if len (gv.villians) == 0:
            # change map if no enemy left
            game.change_map()

        # Refresh GUI and messages
        game.invalidate()
        game.draw()

        # time.sleep(.01)

if __name__ == "__main__":
    seed_value = 42
    os.environ ['PYTHONHASHSEED'] = str (seed_value)
    random.seed (seed_value)
    numpy.random.seed (seed_value)
    main()
