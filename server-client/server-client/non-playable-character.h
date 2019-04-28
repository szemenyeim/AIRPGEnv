#pragma once
#include <string>

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

		
		std::string Parse() {
			std::string msg;
			msg = std::to_string(id) + ";" + "Monster;" + std::to_string(position.x) + ";"
				+ std::to_string(position.y) + ";"
				+ std::to_string(Level) + ";"
				+ std::to_string(current_HP) + ";"
				+ std::to_string(maximum_HP) + "\n ";
			return msg;
		}
	};
}