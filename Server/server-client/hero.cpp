#include "hero.h"

RPGEnv::Hero::Hero(std::string name, int level, int max_HP, int curr_HP) :
	Character(level, max_HP, curr_HP, name) {};

void RPGEnv::Hero::Die()
{
	delete ExplorationMatrix;
	delete this;
};

void RPGEnv::Hero::setExplorationMatrix(int rows, int cols)
{
	int sum = 0;
	std::cout << "Rows, Cols: " << rows << "," << cols << std::endl;
	ExplorationMatrix = new int*[rows];

	for (int i = 0; i < rows; i++) 
	{
		ExplorationMatrix[i] = new int[cols];
		for (int j = 0; j < cols; j++)
		{
			ExplorationMatrix[i][j] = 0;
			sum += 1;
		}
	}

	min_y, max_y, min_x, max_x = 0;
	position.y - 32 <= 0 ? min_y = 0 : min_y = position.y - 32;
	position.y + 32 >= rows ? max_y = rows : max_y = position.y + 32;
	position.x - 32 <= 0 ? min_x = 0 : min_x = position.x - 32;
	position.x + 32 >= cols ? max_x = cols : max_x = position.x + 32;

	for (uint row = min_y; row < max_y; row++) 
	{
		for (int col = min_x; col < max_x; col++) 
		{
			ExplorationMatrix[row][col] = 1;
		}
	}
}

void RPGEnv::Hero::Exploring(int keypressed)
{
	switch (keypressed)
	{
	case (int)'w':
	{
		for (uint col = min_x; col < max_x; col++) //north
		{
			if (ExplorationMatrix[max_y-1][col] == 0)
			{
				ExplorationMatrix[max_y-1][col] = 1;
				experience += 0.1;
			}
		}
		break;
	}
	case (int)'s':
	{
		for (uint col = min_x; col < max_x; col++) //south
		{
			if (ExplorationMatrix[min_y][col] == 0)
			{
				ExplorationMatrix[min_y][col] = 1;
				experience += 0.1;
			}
		}
		break;
	}

	case (int)'a':
	{
		for (uint row = min_y; row < max_y; row++) //west
		{
			if (ExplorationMatrix[row][min_x] == 0)
			{
				ExplorationMatrix[row][min_x] = 1;
				experience += 0.1;
			}
		}
		break;
	}
	case (int)'d':
	{
		for (uint row = min_y; row < max_y; row++) //east
		{
			if (ExplorationMatrix[row][max_x-1] == 0)
			{
				ExplorationMatrix[row][max_x-1] = 1;
				experience += 0.1;
			}
		}

		break;
	}
	}
};

std::string RPGEnv::Hero::Parse()
{

	/// <summary>Parse the paramesers of the Hero to a string</summary>
	/// <returns>The parsed message</returns>  
	std::string msg;

	msg = std::to_string(id) + ";" + name + ";" + std::to_string(position.x) + ";"
		+ std::to_string(position.y) + ";"
		+ std::to_string(Level) + ";"
		+ std::to_string(current_HP) + ";"
		+ std::to_string(experience) + "\n";
	return msg;

};