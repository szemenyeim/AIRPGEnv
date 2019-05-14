#include "Character.h"


using namespace RPGEnv;



Character::Character(int level, int max_HP, int curr_HP, int experience) :
	Level(level), maximum_HP(max_HP), current_HP(curr_HP), experience(experience)
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

void Character::Die()
{
	
	delete this;
}

void Character::Move(int dx, int dy, Environment &gui)
{

	int new_x = this->position.x + dx;
	int new_y = this->position.y + dy;

	int blue = gui.isBlue(new_x, new_y);

	if (0 < new_x && 0 < new_y && new_x < MAP_SIZE_X && new_y < MAP_SIZE_Y && blue == 0)
	{
		this->position.x = new_x;
		this->position.y = new_y;
	}
}

void Character::Attack(Character &Enemy)
{

	{
			Enemy.current_HP -= 30;//Level * 5;
			std::cout << "Enemy HP: " << Enemy.current_HP << std::endl;
	}
}


void Character::gainXP(int xp) 
{

		this->experience += xp;
		this->Level = static_cast<int>(this->experience / (50 + 50 * this->Level));

		std::cout << "Hero LVL: " << this->Level << std::endl;
		std::cout << "Hero XP: " << this->experience << std::endl;

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