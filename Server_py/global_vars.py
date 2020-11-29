class mail():
    def __init__(self):
        self.text = []
        self.sent = False
        self.kill = False

    def post_message (self,message: str):
        if self.text != message:
            self.text = message
            self.sent = False

class Stat():
    def __init__(self):
        self.xp = 0
        self.level = 1
        self.killing = 0
        self.death = 0
        self.hunting = 0

    def die(self):
        self.death += 1

    def evolve(self,xp, level):
        self.level = level
        self.xp = xp

    def kill(self):
        self.killing +=1

    def hunt(self):
        self.hunting += 1


map_size = ()
count_of_people = 0
mailbox_in = {}
mailbox_out = {}
characters = []
heroes = []
connected = {}  # {hero_name :  Stat}
cons = {}
villians = []
server_lock = None
out_lock = None
con_lock = None
character_lock = None


