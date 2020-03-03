import copy
import cv2

import Server_py.global_vars as gv


class GUI:
    def __init__(self, img4map, window_name):
        self.current_game = None
        self.orig = cv2.imread(img4map)
        self.window_name = window_name

        self.clear_window()
        gv.map_size = (self.current_game.shape[1], self.current_game.shape[0])

    def clear_window(self):
        self.current_game = copy.deepcopy(self.orig)

    def show_window(self):
        cv2.namedWindow(self.window_name, cv2.WINDOW_NORMAL)
        cv2.imshow(self.window_name, self.current_game)
        cv2.waitKey(1)

    def is_blue(self, x, y):
        blue = False
        self.clear_window()
        hsv_im = cv2.cvtColor(self.current_game, cv2.COLOR_BGR2HSV)

        color = hsv_im[y, x, 0]
        if 138 > color > 78:
            blue = True
        return blue

    def draw_hero(self, x, y):
        cv2.circle(
            self.current_game,
            (int(x), int(y)),
            1,
            (0, 0, 255)
        )

    def draw_monster(self, x, y):
        cv2.circle(
            self.current_game,
            (int(x), int(y)),
            1,
            (255, 0, 0)
        )
