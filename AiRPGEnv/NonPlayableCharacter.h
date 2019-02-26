#pragma once
#include "Character.h"

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
		Monster(int level = 1, int max_HP = 100, int curr_HP = 100) :
			NPC(level, max_HP, curr_HP) {}
		void Attack();
		void Defense();
		void Draw(GUI& gui) {
			gui.DrawMonster(position.x, position.y);
		}
	};

}