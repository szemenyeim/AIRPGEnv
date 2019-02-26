#pragma once
#include <time.h>

const unsigned int MAP_SIZE_X = 64;
const unsigned int MAP_SIZE_Y = 64;

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
		this->position.x += dx;
		this->position.y += dy;
		
	}

	void Character::Attack(Character &Enemy)
	{
		
		{
			Enemy.current_HP -= 100;//Level * 5;
		}
	}
}