#pragma once
#include "character.h"
#include <string>

namespace RPGEnv {

	class Hero : public Character {
	public:
		std::string name;
		bool Attacking;
		bool Defensing;
		Hero(std::string name = "", int level = 1, int max_HP = 100, int curr_HP = 100) :
			Character(level, max_HP, curr_HP) 
		{
			this->name = name;
		};

	
		std::string Parse()
		{
			std::string msg;
			msg = id + ";" + name + ";" + std::to_string(position.x) + ";"
				+ std::to_string(position.y) + ";"
				+ std::to_string(Level) + ";"
				+ std::to_string(current_HP) + ";"
				+ std::to_string(maximum_HP) + "\n";
			return msg;
		}

	private:

		int maximum_MP;
		int current_MP;

	};
}