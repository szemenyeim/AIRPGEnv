#pragma once
#include "Character.h"
#include "Gui.h"
#include <queue>

namespace RPGEnv {

	class Hero : public Character {
	public:
		bool Attacking;
		bool Defensing;
		Hero(int level = 1, int max_HP = 100 , int curr_HP = 100) :
			Character(level, max_HP, curr_HP) {};
		void Draw(GUI& gui)
		{
			gui.DrawHero(position.x, position.y);
		}

	private:

		int maximum_MP;
		int current_MP;
		
	};
}