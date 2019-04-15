#pragma once
#include "Character.h"
#include <queue>
#include "concurrent_queue.h"
#include <thread>
#include <iostream>
#include "Game.h"

namespace RPGEnv {
	class NPC : public Character
	{
	public:
		NPC(int level = 1, int max_HP = 100, int curr_HP = 100) :
			Character(level, max_HP, curr_HP) {}
	private:

	};

	class Monster :public NPC
	{
	public:

		Monster(Concurrency::concurrent_queue<Character*> heroes, int level = 1, int max_HP = 100, int curr_HP = 100) :
			NPC(level, max_HP, curr_HP) {
			//this->Heroes.operator= *heroes;
		}

		void Defense();
		//void Attack();
		void Draw(GUI& gui) {
			gui.DrawMonster(position.x, position.y);
		}
		Character * FindHero(Hero & PlayerOne) {

		}
		void Engage(Concurrency::concurrent_queue<Character*> heroes) {
			while (true) {

				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				auto it = heroes.unsafe_begin();
				if (5 > abs(this->position.x - (*it)->position.x) && 5 > abs(this->position.y - (*it)->position.y))
				{
					(*it)->current_HP -= 20;
					std::cout << "Hero HP: " << (*it)->current_HP << std::endl;
				}
				if (20 > abs(this->position.x - (*it)->position.x) && 20 > abs(this->position.y - (*it)->position.y)) {
					if ((*it)->position.x > this->position.x)
						this->position.x++;
					else
						this->position.x--;
					if ((*it)->position.y > this->position.y)
						this->position.y++;
					else
						this->position.y--;
				}
				if (0 >= this->current_HP) {
					break;
				}
			}


		}
	private:
		float TerritoryRadius = 15;

	};

}