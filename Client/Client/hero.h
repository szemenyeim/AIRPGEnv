#pragma once
#include "Character.h"


namespace RPGEnv {

	class Hero : public Character {
	public:

		std::string name;
		bool Attacking;
		bool Defensing;
		Hero(std::string name, int xpos, int ypos, int level = 1, int max_HP = 100, int curr_HP = 100) :
			Character(xpos, ypos, level, max_HP, curr_HP) 
		{
			this->name = name;
		};
		void Draw(GUI& gui)
		{
			gui.DrawHero(position.x, position.y);
		}

	private:

		int maximum_MP;
		int current_MP;

	};
}