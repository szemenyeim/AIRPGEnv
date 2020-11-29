import random, time
import numpy as np
import global_vars as gv

"""todo: param loader"""


class Character:
    def __init__ (self, gui, name, level, max_HP, xp=0):
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
        gv.characters.append(self)
        self.name = name
        self.position = [x, y]
        self.level = level
        self.XP = xp
        self.curr_HP = max_HP
        self.max_HP = max_HP
        self.target = None
        self.marked = False
        self.hunt_list = []

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
        self.level = self.XP // (50 + 50 * int(self.level))
        if self.name in gv.connected:
            gv.connected[self.name].evolve(self.XP, self.level)

    def attack(self):
        xp_change = self.target.max_HP //2
        bonus = 500
        self.target.curr_HP -= self.target.max_HP // 2
        if self.target.curr_HP <= 0:
            try:
                 gv.characters.remove(self.target)
            except:
                 pass
        #         print("removed already")
            if self.name in gv.connected:
                gv.connected[self.name].kill()
            if self.target in gv.heroes:
                gv.connected [self.target.name].die()
                gv.heroes.remove(self.target)
            elif self.target in gv.villians:
                gv.villians.remove(self.target)
            if self.target in self.hunt_list:
                xp_change += bonus
                gv.connected[self.name].hunt()
                # print("{} scored the bonus for quest".format(self.id))
            message = "{};DEAD\n".format(self.target.id)
            try:
                gv.mailbox_out[self.target.id].post_message(message)
            except:
                pass
        self.gainXP(xp_change)
        self.target = None

    def aim(self, target=None):
        if self.target:
            self.target.marked = False
        if target:
            self.target = target
            self.target.marked = True

    def parse(self):
        if self.target:
            target = self.target.id
        else:
            target = None
        msg = "{0};{1};{2};{3};{4};{5};{6};{7};{8}\n" \
            .format(self.id, self.name, self.position[0], self.position[1], self.level, self.curr_HP, self.XP, self.marked, target)
        return msg


class Hero(Character):
    attack_range = 5

    def __init__(self, gui, name, level=1, max_HP=100):
        xp = 0

        if name in gv.connected:
            xp = gv.connected[name].xp
            level = gv.connected[name].level

        else:
            gv.connected [name] = gv.Stat()

        super (Hero, self).__init__ (gui, name, level, max_HP,xp)
        # set exploration matrix
        self.ExplorationMatrix = np.array([[0 for x in range(gv.map_size[0])] for y in range(gv.map_size[1])])
        n = 60
        x, y = self.crop_view(n)
        self.ExplorationMatrix[y + 2: y + n + 2, x + 2: x + n + 2] = 1
        gv.heroes.append(self)

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
    targets_num = 5

    def __init__(self, gui,  level, name = "NPC", max_HP = 100):
        super(NPC, self).__init__(gui=gui, name=name, level=level, max_HP=max_HP)


    def give_quest(self,player):
        dog_tags = "{};Targets;".format(player.id)
        bounty = 100
        # todo: lets find the xp given
        # give back some folks to hunt for a bounty todo: sophisticated selection.... maybe area based

        player.hunt_list = random.sample(gv.villians, k=min(self.targets_num, len(gv.villians)))

        # create messgae from the information
        for monster in player.hunt_list:
            dog_tags += "{};".format(monster.id)
        dog_tags += str(bounty) + "\n\r"
        # print(dog_tags)
        # emplace it in the mailbox

        gv.mailbox_out[player.id].post_message(dog_tags)



    def draw(self,gui):
        gui.draw_NPC(self.position[0], self.position[1])

class Monster(NPC):
    def __init__(self, gui, level, max_HP=100, name="Monster", ):
        super(Monster, self).__init__(gui=gui, name=name, level=level, max_HP=max_HP)
        gv.villians.append(self)

    def draw(self, gui):
        gui.draw_monster(self.position[0], self.position[1])

    def engage(self):
        # todo: improve with choose
        for hero in gv.heroes:
            if self.calc_dist(hero) < self.attack_range:
                self.aim(hero)
                self.attack()
            if self.calc_dist(hero) < self.sight:
                if self.position[0] < hero.position[0]:
                    self.position[0] += 1
                else:
                    self.position[0] -= 1
                if self.position[1] < hero.position[1]:
                    self.position[1] += 1
                else:
                    self.position[1] -= 1
