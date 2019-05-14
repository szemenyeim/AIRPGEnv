#pragma once
#include <time.h>
#include <random>
#include <iostream>
#include "Environment.h"

static unsigned int MAP_SIZE_X = 64;
static unsigned int MAP_SIZE_Y = 64;
static int count_of_people = 0;

namespace RPGEnv {
	
	class Position
	{
	public:
		int x;
		int y;

		Position(int x = 0, int y = 0) :x(x), y(y) {};
	};

	class Character
	{
	public:
		int id;
		Position position;
		int Level;
		int maximum_HP;
		int current_HP;
		double experience = 0.0;
		int ThreadID;

		Character(int level = 1, int max_HP = 100, int curr_HP = 100, int experience = 0);
		static void setMapSize(unsigned int x, unsigned int y);
	
		void Die();
		void gainXP(int xp);
		void Move(int dx, int dy, Environment &gui);
		void Attack(Character& Enemy);
		void Defense();
		virtual std::string Parse() = 0;

		bool operator==(const Character & other);
	private:
		
		static void freshmen() {count_of_people++;}
	};

}