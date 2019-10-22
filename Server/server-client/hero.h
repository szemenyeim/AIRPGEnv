#pragma once
#include "character.h"
#include <queue>
#include <string>



namespace RPGEnv {

	class Hero : public Character {
	public:

		bool Attacking;
		bool Defensing;
		int** ExplorationMatrix;
		Hero(std::string name = "", int level = 1, int max_HP = 100, int curr_HP = 100);
		void setExplorationMatrix(int rows, int cols);
		void Die();
		void Exploring(int keypressed);
		std::string Parse();
		
	private:
		int maximum_MP;
		int current_MP;

		uint min_x, max_x, min_y, max_y;
	};
}