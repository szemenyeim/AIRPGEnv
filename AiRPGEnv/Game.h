#pragma once
#include "Gui.h"
#include "Hero.h"
#include "Character.h"
#include "NonPlayableCharacter.h"
#include <vector>
#include "concurrent_queue.h"
#include <thread>

namespace RPGEnv {

	class Game
	{
	public:
		unsigned char* GameData;
		std::list<Character*> Characters;
		std::vector<Monster*> Villians;
		Concurrency::concurrent_queue<Character*> Heroes;
		Hero PlayerOne;
		GUI *Interface;

		std::thread MonsterEngageThreads[51];

		Game(const char* &img4map, const char* &window_name);
		double CalcDist(Character & You, Character & Other);
		void KeyEventHandler();
		void AddNewMonster(int count, int Level, int HP);
		/*std::list<Character*> getHeroes() {
			return this->Heroes;
		}*/
		void Invalidate();

	private:


		Character * FindNearest(Character & You);

	};
}