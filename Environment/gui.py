import cv2
import copy


class GUI:
    def __init__(self,img4map, window_name):
        self.current_game = None
        self.img4map = img4map
        self.window_name = window_name

        self.clear_window()
        cv2.namedWindow(window_name,cv2.WINDOW_NORMAL)
        x_size = self.current_game.shape[0]
        y_size = self.current_game.shape[1]

    def clear_window(self):
        map = cv2.imread(self.img4map)
        self.current_game = copy.deepcopy(map)

    def show_window(self,x,y):
        x = x - 32 if x - 32 > 0 else 0
        y = y - 32 if y - 32 > 0 else 0
        x = x if (x + 64 < self.current_game.shape[1])  else self.current_game.shape[1]  - 64
        y = y if (y + 64 < self.current_game.shape[0]) else self.current_game.shape[0] - 64
        self.current_game = self.current_game[y : y + 64, x : x + 64,]
        cv2.imshow(self.window_name, self.current_game)

        cv2.waitKey(1)

    def get_key_pressed(self):
        return cv2.waitKey(10)

    def is_blue(self, x, y):
        blue = False
        color_b = self.current_game[x,y][2]
        if color_b > 250:
            blue = True
        return blue

    def draw_hero(self, x, y):
        cv2.circle(
            self.current_game,
            (int(x),int(y)),
            1,
            (0, 0, 255)
        )

    def draw_monster(self, x, y):
        cv2.circle(
            self.current_game,
            (int(x), int(y)),
            1,
            (255,0,0)
        )