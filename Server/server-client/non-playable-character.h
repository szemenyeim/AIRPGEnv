#pragma once
#include "Character.h"
#include <queue>
#include "concurrent_queue.h"
#include <thread>
#include <string>
#include <iostream>
#include "Game.h"

namespace RPGEnv {
	class NPC : public Character
	{
	public:
		NPC(int level = 1, int max_HP = 100, int curr_HP = 100, std::string name = "") :
			Character(level, max_HP, curr_HP, name) {}
	private:

	};

	class Monster :public NPC
	{
	public:
		Monster(Concurrency::concurrent_queue<Character*> heroes, int level = 1, int max_HP = 100, int curr_HP = 100, std::string name= "Monster" ) :
			NPC(level, max_HP, curr_HP, name) {
		}

		void Defense();

		
		std::string Parse() {
			std::string msg;
			msg = std::to_string(id) + ";" + name +";" + std::to_string(position.x) + ";"
				+ std::to_string(position.y) + ";"
				+ std::to_string(Level) + ";"
				+ std::to_string(current_HP) + ";"
				+ std::to_string(experience) + "\n ";
			return msg;
		}
		Character * FindHero(Hero & PlayerOne) {

		}
		void Engage(Concurrency::concurrent_queue<Character*> &heroes)
		{
			while (true) 
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				auto it = heroes.unsafe_begin();
				if (heroes.unsafe_size() > 0)
				{
					if (5 > abs(this->position.x - (*it)->position.x) && 5 > abs(this->position.y - (*it)->position.y))
					{
						//this->Attack(**it);
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
		}
	private:
		float TerritoryRadius = 15;

	};
}