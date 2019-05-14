#include "Character.h"

using namespace RPGEnv;

static unsigned int MAP_SIZE_X = 64;
static unsigned int MAP_SIZE_Y = 64;

Character::Character(int x_pos, int y_pos, int level, int max_HP, int curr_HP, int experience) :
	Level(level), maximum_HP(max_HP), current_HP(curr_HP)
{
	position = Position(x_pos, y_pos);
}


void Character::setMapSize(unsigned int x, unsigned int y)
{
	MAP_SIZE_X = 64; x;
	MAP_SIZE_Y = 64; y;
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
