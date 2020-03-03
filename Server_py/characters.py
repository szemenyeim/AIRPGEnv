import random, time
import numpy as np
import Server_py.global_vars as gv

"""todo: param loader"""


class Character:
    def __init__(self, gui, name, level, max_HP):
        x = y = 0
        while True:
            x = random.randint(0, gv.map_size[0] - 1)
            y = random.randint(0, gv.map_size[1] - 1)
            if not gui.is_blue(x, y):
                break
        gv.count_of_people += 1
        self.id = gv.count_of_people
        gv.out_lock.acquire()
        gv.mailbox_out[self.id] = gv.mail()
        gv.out_lock.release()
        self.name = name
        self.position = [x, y]
        self.level = level
        self.XP = 0
        self.curr_HP = max_HP
        self.max_HP = max_HP

    def calc_dist(self, other):
        x_dist = self.position[0] - other.position[0]
        y_dist = self.position[1] - other.position[1]
        return (x_dist ** 2 + y_dist ** 2) ** 0.5

    def move(self, dx, dy, gui):
        new_x = self.position[0] + dx
        new_y = self.position[1] + dy

        if 0 < new_x < gv.map_size[0] and 0 < new_y < gv.map_size[1]:
            if not (gui.is_blue(new_x, new_y)):
                self.position[0] = new_x
                self.position[1] = new_y

    def gainXP(self, xp):
        self.XP += xp
        self.level = self.XP // (50 + 50 * self.level)

    def attack(self, enemy):
        hp_change = 30
        enemy.curr_HP -= hp_change
        if enemy.curr_HP <= 0:
            gv.characters.remove(enemy)
            if enemy in gv.heroes:
                gv.heroes.remove(enemy)
            message = "{};DEAD\n".format(enemy.id)
            # print(message)
            gv.mailbox_out[enemy.id].text = message
            gv.mailbox_out[enemy.id].sent = False
        self.gainXP(hp_change)

    def parse(self):
        msg = "{0};{1};{2};{3};{4};{5};{6}\n" \
            .format(self.id, self.name, self.position[0], self.position[1], self.level, self.curr_HP, self.XP)
        return msg


class Hero(Character):
    def __init__(self, gui, name, level=1, max_HP=100):
        super(Hero, self).__init__(gui, name, level, max_HP)
        # set exploration matrix
        self.ExplorationMatrix = np.array([[0 for x in range(gv.map_size[0])] for y in range(gv.map_size[1])])
        n = 60
        x, y = self.crop_view(n)
        self.ExplorationMatrix[y + 2: y + n + 2, x + 2: x + n + 2] = 1

    def crop_view(self, n):
        x_new = self.position[0] - (n // 2) if self.position[0] - (n // 2) > 0 else 0
        y_new = self.position[1] - (n // 2) if self.position[1] - (n // 2) > 0 else 0
        x_new = x_new if (x_new + n < gv.map_size[0]) else gv.map_size[0] - n
        y_new = y_new if (y_new + n < gv.map_size[0]) else gv.map_size[0] - n
        return x_new, y_new

    def explore(self, key):
        gained_xp_multiplicator = 1
        n = 60
        x, y = self.crop_view(60)

        already_known = self.ExplorationMatrix[y + 2: y + n + 2, x + 2: x + n + 2].sum()
        self.ExplorationMatrix[y + 2: y + n + 2, x + 2: x + n + 2] = 1
        explored = self.ExplorationMatrix[y + 2: y + n + 2, x + 2: x + n + 2].sum()
        new_territory = explored - already_known
        gainedXP = new_territory * gained_xp_multiplicator
        self.gainXP(gainedXP)

    def draw(self, gui):
        gui.draw_hero(self.position[0], self.position[1])


class NPC(Character):
    sight = 20
    attack_range = 5

    def __init__(self, gui, name, level, max_HP):
        super(NPC, self).__init__(gui, name, level, max_HP)


class Monster(NPC):
    def __init__(self, gui, level, max_HP=100, name="Monster", ):
        super(Monster, self).__init__(gui, name, level, max_HP)

    def draw(self, gui):
        gui.draw_monster(self.position[0], self.position[1])

    def engage(self):
        for hero in gv.heroes:
            if self.calc_dist(hero) < self.attack_range:
                self.attack(hero)
            if self.calc_dist(hero) < self.sight:
                if self.position[0] < hero.position[0]:
                    self.position[0] += 1
                else:
                    self.position[0] -= 1
                if self.position[1] < hero.position[1]:
                    self.position[1] += 1
                else:
                    self.position[1] -= 1
