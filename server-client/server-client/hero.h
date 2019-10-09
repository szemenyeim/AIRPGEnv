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
		Hero(std::string name = "", int level = 1, int max_HP = 100, int curr_HP = 100) :
			Character(level, max_HP, curr_HP, name) {};
		void setExplorationMatrix(int rows, int cols) {
			int sum = 0;
			std::cout << "Rows, Cols: " << rows << "," << cols << std::endl;
			ExplorationMatrix = new int*[rows];
			for (int i = 0; i < rows; i++) {
				ExplorationMatrix[i] = new int[cols];
				for (int j = 0; j < cols; j++) {
					ExplorationMatrix[i][j] = 0;
					sum += 1;
				}
			}

			for (int row = position.y - 32; row < position.y + 32; row++) {
				for (int col = position.x - 32; col < position.x + 32; col++) {
					ExplorationMatrix[row][col] = 1;
				}
			}
		};
		void Die() {
			delete ExplorationMatrix;
			delete this;
		}
		void Exploring(int keypressed) {
			switch (keypressed)
			{
				case (int)'w':
				{
					for (int col = position.x - 32; col < position.x + 32; col++) //north
					{
						if (ExplorationMatrix[position.y + 32][col] == 0)
						{
							ExplorationMatrix[position.y + 32][col] = 1;
							experience += 0.1;
						}
					}
					std::cout << *ExplorationMatrix[position.y + 32] << std::endl;
					break;
				}
				case (int)'s': 
				{
					for (int col = position.x - 32; col < position.x + 32; col++) //south
					{
						if (ExplorationMatrix[position.y - 32][col] == 0)
						{
							ExplorationMatrix[position.y - 32][col] = 1;
							experience += 0.1;
						}
					}
					break;
				}

				case (int)'a': 
				{
					for (int row = position.y - 32; row < position.y + 32; row++) //west
					{
						if (ExplorationMatrix[row][position.x - 32] == 0)
						{
							ExplorationMatrix[row][position.x - 32] = 1;
							experience += 0.1;
						}
					}
					break;
				}
				case (int)'d': 
				{
					for (int row = position.y - 32; row < position.y + 32; row++) //east
					{
						if (ExplorationMatrix[row][position.x + 32] == 0)
						{
							ExplorationMatrix[row][position.x + 32] = 1;
							experience += 0.1;
						}
					}
					
					break;
				}
			}
			
		}
		std::string Parse()
		{
			std::string msg;
			if (current_HP <= 0)
			{
				msg = std::to_string(id) + ";DEAD\n";
				Die();
			}
			else
			{
				msg = std::to_string(id) + ";" + name + ";" + std::to_string(position.x) + ";"
					+ std::to_string(position.y) + ";"
					+ std::to_string(Level) + ";"
					+ std::to_string(current_HP) + ";"
					+ std::to_string(experience) + "\n";
				return msg;
			}
		}
	private:

		int maximum_MP;
		int current_MP;

	};
}