class Quest():
    targets = []
    xp = 0

class Character:
    def __init__(self, x, y, level, XP, curr_HP, marked = False):
        self.position = (x,y)
        self.level = level
        self.XP = XP
        self.curr_HP = curr_HP
        self.marked = marked

    def draw(self):
        raise (NotImplementedError)

class Hero(Character):
    def draw(self,gui):
        gui.draw_hero(self.position[0], self.position[1])

class NPC(Character):
    def draw(self, gui):
        gui.draw_NPC(self.position[0], self.position[1])

class Monster(NPC):
    def draw(self, gui):
        gui.draw_monster(self.position[0], self.position[1])