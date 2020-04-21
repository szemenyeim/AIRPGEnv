import cv2
import copy
import numpy as np


class GUI:
    def __init__(self, img4map, window_name):
        self.current_game = None

        self.img4map = img4map
        self.orig = cv2.imread(self.img4map)
        self.clear_map()
        # self.map =cv2.cvtColor(self.map,cv2.COLOR_BGR2HSV)
        self.window_name = window_name

        self.clear_window()
        self.x_size = self.current_game.shape[1]
        self.y_size = self.current_game.shape[0]

    def clear_window(self):
        self.current_game = copy.deepcopy(self.orig)

    def clear_map(self):
        self.map = np.zeros((self.orig.shape[0] + 2, self.orig.shape[1] + 2, 3), dtype="uint8")
        self.targets = np.zeros((16, 16))

    def process_window(self, x, y):
        def crop_view(n):
            x_new = x - (n // 2) if x - (n // 2) > 0 else 0
            y_new = y - (n // 2) if y - (n // 2) > 0 else 0
            x_new = x_new if (x_new + n < self.x_size) else self.x_size - n
            y_new = y_new if (y_new + n < self.y_size) else self.y_size - n
            return x_new, y_new

        n = 60
        x_new, y_new = crop_view(n)
        self.map[y_new + 2: y_new + n + 2, x_new + 2: x_new + n + 2] = self.current_game[y_new: y_new + n,
                                                                       x_new: x_new + n]
        self.minimap = cv2.resize(self.map, (16, 16), interpolation=cv2.INTER_AREA)

        self.minimap[np.where(self.targets == 1)] = (0, 0, 255)
        n = 64
        x_new, y_new = crop_view(n)
        self.current_game = self.map[y_new: y_new + n, x_new: x_new + n]

    def show_window(self):
        cv2.namedWindow("minimap", cv2.WINDOW_NORMAL)
        cv2.imshow("minimap", self.minimap)

        cv2.namedWindow(self.window_name, cv2.WINDOW_NORMAL)
        cv2.imshow(self.window_name, self.current_game)
        cv2.waitKey(1)



    @staticmethod
    def get_key_pressed():
        return cv2.waitKey(10)

    def draw_hero(self, x, y):
        cv2.circle(
            self.current_game,
            (int(x), int(y)),
            1,
            (95, 55, 255)
        )

    def draw_monster(self, x, y):
        cv2.circle(
            self.current_game,
            (int(x), int(y)),
            1,
            (255, 132, 10)
        )

    def draw_NPC(self, x, y):
        cv2.circle(
            self.current_game,
            (int(x), int(y)),
            1,
            (0, 0, 0)
        )

    def draw_mark(self, x, y):
        cv2.circle(
            self.current_game,
            (int(x), int(y)),
            2,
            (10, 159, 255)
        )

    def draw_targets(self, x_pos, y_pos):
            # draw red rectangles on the minimap grid around the targets position
            x = self.targets.shape[1] * x_pos // self.x_size
            y = self.targets.shape[0] * y_pos // self.y_size
            self.targets[y, x] = 1

            # mark targets purple
            cv2.circle(
                self.current_game,
                (int(x_pos), int(y_pos)),
                1,
                (242, 90, 191)
            )
