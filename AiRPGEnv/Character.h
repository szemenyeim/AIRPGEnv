#pragma once
#include <time.h>

static unsigned int MAP_SIZE_X = 64;
static unsigned int MAP_SIZE_Y = 64;

namespace RPGEnv {

	class Position {
	public:
		int x;
		int y;
		
		Position(int x = 0, int y = 0) :x(x), y(y) {};
	};

	class Character
	{
	public:
		Position position;

		int Level;
		int maximum_HP;
		int current_HP;

		Character(int level = 1, int max_HP = 100, int curr_HP = 100);
		static void setMapSize(unsigned int x, unsigned int y)
		{
			MAP_SIZE_X = x;
			MAP_SIZE_Y = y;
		}
		void Die();
		void Move(int dx, int dy);
		void Attack(Character& Enemy);
		void Defense();
		virtual void Draw(GUI& gui)=0;
	};

	Character::Character(int level, int max_HP, int curr_HP):
	Level(level),maximum_HP(max_HP),current_HP(curr_HP)
	{
		int x_pos = rand() % MAP_SIZE_X;
		int y_pos = rand() % MAP_SIZE_Y;
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
		if (0 < new_x && 0 < new_y && new_x < MAP_SIZE_X && new_y < MAP_SIZE_Y)
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
}