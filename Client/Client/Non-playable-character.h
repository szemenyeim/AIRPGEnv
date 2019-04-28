#pragma once
#include "Character.h"

namespace RPGEnv {
	class NPC : public Character
	{
	public:
		NPC(int xpos, int ypos, int level = 1, int max_HP = 100, int curr_HP = 100) :
			Character(xpos, ypos, level, max_HP, curr_HP) {}
	private:

	};

	class Monster :public NPC
	{
	public:
		Monster(int xpos, int ypos, int level = 1, int max_HP = 100, int curr_HP = 100) :
			NPC(xpos, ypos, level, max_HP, curr_HP) {}


		void Draw(GUI& gui) {
			gui.DrawMonster(position.x, position.y);
		}
	};

}