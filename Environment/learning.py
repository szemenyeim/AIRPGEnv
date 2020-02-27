# -*- coding: utf-8 -*-
"""
Created on Tue Feb 25 23:05:08 2020

@author: Kertész Gergő
"""
from environment import Environment
import random

if __name__ == "__main__":

    env = Environment("gym", "HeroicHero")
    random.seed()

    while True:
        env.step(random.randint(0,3))
        env.render(env.gui)

        time.sleep(0.01)
        
    env.close()