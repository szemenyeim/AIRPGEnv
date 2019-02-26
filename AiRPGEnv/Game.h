#pragma once
#include "Gui.h"
#include "Hero.h"
#include "Character.h"
#include "NonPlayableCharacter.h"
#include <vector>

namespace RPGEnv {

	class Game
	{
	public:
		unsigned char* GameData;
		std::list<Character*> Characters;
		std::vector<Monster*> Villians;
		Hero PlayerOne;
		GUI *Interface;
		
		Game(const char* &img4map, const char* &window_name);
		int CalcDist(Character & You, Character & Other);
		void KeyEventHandler();
		void AddNewMonster(int count, int Level, int HP);

	private:
		void Invalidate();
		
		Character * FindNearest(Character & You);

	};
}