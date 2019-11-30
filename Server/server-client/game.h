#pragma once
#include "Character.h"
#include "hero.h"
#include "non-playable-character.h"
#include "Environment.h"
#include "TcpServer.h"

#include <concurrent_queue.h>
#include <vector>
#include <list>
#include <map>
#include <mutex>
#include <thread>
#include <chrono>

namespace RPGEnv {

	class Game
	{
	public:
		unsigned char* GameData;
		std::list<Character*> Characters;
		std::vector<Monster*> Villians;
		Concurrency::concurrent_queue<Character*> Heroes;
		Environment *Interface;
		std::thread MonsterEngageThreads[51];
		int character_cnt = 0;

		Game(const char* &img4map);
		double CalcDist(Character & You, Character & Other);
		void KeyEventHandler(int keypressed, Hero *PlayerOne);
		void AddNewMonster(int count, int Level = 1, int HP = 100);	
		void Invalidate(std::map<int,MsgHandler> &Mailbox);

	private:
		

		Character * FindNearest(Character & You);

	};

}