#pragma once
#include <time.h>
#include "Gui.h"


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

		Character(int x_pos, int y_pos, int level = 1, int max_HP = 100, int curr_HP = 100);
		static void setMapSize(unsigned int x, unsigned int y);

		bool operator==(const Character & other);

		virtual void Draw(GUI& gui) = 0;
	};
}