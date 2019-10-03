
class Character:
    def __init__(self, x, y, level, max_HP, curr_HP, experience = 0):
        self.position = (x,y)
        self.level = level
        self.max_HP = max_HP
        self.curr_HP = curr_HP
        self.experience = experience

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