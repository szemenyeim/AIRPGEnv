#include "Character.h"

using namespace RPGEnv;



Character::Character(int level, int max_HP, int curr_HP) :
	Level(level), maximum_HP(max_HP), current_HP(curr_HP)
{
	if (MAP_SIZE_X == 0 || MAP_SIZE_Y == 0)
	{
		
		return;
	}
	int x_pos = rand() % MAP_SIZE_X;
	int y_pos = rand() % MAP_SIZE_Y;
	id = count_of_people;
	freshmen();
	position = Position(x_pos, y_pos);
}

inline void Character::Die()
{
	delete this;
}

void Character::Move(int dx, int dy)
{

	int new_x = this->position.x + dx;
	int new_y = this->position.y + dy;

	int blue = 0;//gui.isBlue(new_x, new_y);

	if (0 < new_x && 0 < new_y && new_x < MAP_SIZE_X && new_y < MAP_SIZE_Y && blue == 0)
	{
		this->position.x = new_x;
		this->position.y = new_y;
	}
}

void Character::Attack(Character &Enemy)
{

	{
		Enemy.current_HP -= 100;//Level * 5;
	}
}

void Character::setMapSize(unsigned int x, unsigned int y)
{
	MAP_SIZE_X = x;
	MAP_SIZE_Y = y;
}

bool Character::operator==(const Character &theOther)
{
	bool equality = false;
	if (this->position.x == theOther.position.x &&
		this->position.y == theOther.position.y &&
		this->current_HP == theOther.current_HP &&
		this->maximum_HP == theOther.maximum_HP)
	{
		equality = true;
	}
	return equality;
}