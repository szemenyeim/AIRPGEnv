
class Character:
    def __init__(self, x, y, level, XP, curr_HP):
        self.position = (x,y)
        self.level = level
        self.XP = XP
        self.curr_HP = curr_HP

    def draw(self):
        pass

class Hero(Character):
    def draw(self,gui):
        gui.draw_hero(self.position[0], self.position[1])

class NPC(Character):
    pass

class Monster(NPC):
    def draw(self, gui):
        gui.draw_monster(self.position[0], self.position[1])