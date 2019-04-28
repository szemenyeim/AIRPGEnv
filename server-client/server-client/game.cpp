#include <iostream>


#include "Game.h"

//using namespace std;
using namespace RPGEnv;

const int DeltaPos = 1;

constexpr auto SPACE = 32;

std::list<MsgHandler> Mailbox;
std::mutex lock;

Game::Game(const char* &img4map)
{
	srand((unsigned int)time(NULL));
	Interface = new Environment(img4map);
	Character::setMapSize(Interface->SIZE_X, Interface->SIZE_Y);
	
}

void Game::Invalidate(std::list<MsgHandler> &Mailbox)
{
	std::string message, name;
	std::string delimiter = ";";

	for (auto it = Characters.begin(); it != Characters.end(); it++)
	{

		message = (*it)->Parse();
		Mailbox.push_back(MsgHandler(name, false, message));

	}
	

}

Character * Game::FindNearest(Character & You)
{
	Character* nearest = nullptr;
	int min_dist = 5;
	for (auto it = Characters.begin(); it != Characters.end(); it++)
	{
		double act_dist = 0;
		if ((*it)->position.x != You.position.x || (*it)->position.y != You.position.y)
		{
			act_dist = CalcDist(You, **it);
			if (act_dist < min_dist)
			{
				nearest = *it;
				break;
			}
		}
	}
	return nearest;
}

double Game::CalcDist(Character & You, Character & Other)
{
	int x_dist = You.position.x - Other.position.x;
	int y_dist = You.position.y - Other.position.y;
	return sqrt((x_dist * x_dist) + (y_dist*y_dist));
}

void Game::KeyEventHandler(int keypressed, Hero &PlayerOne)
{

	
	switch (keypressed)
	{
	case SPACE:				//space
	{
		Character *Enemy = FindNearest(PlayerOne);
		if (Enemy != NULL)
		{
			PlayerOne.Attack(*Enemy);
			if (0 >= Enemy->current_HP)
			{
				Characters.remove(Enemy);
				//Enemy->Die();

			}
		}
		break;
	}
	case (int)'w':
	{	PlayerOne.Move(0, -DeltaPos);
	break;
	}
	case (int)'a':
	{
		PlayerOne.Move(-DeltaPos, 0);
		break;
	}
	case (int)'s':
	{
		PlayerOne.Move(0, DeltaPos);
		break;
	}
	case (int)'d':
	{
		PlayerOne.Move(DeltaPos, 0);
		break;
	}
	}
	//Invalidate();
}

void Game::AddNewMonster(int count = 1, int Level = 1, int HP = 100)
{
	for (int i = 0; i < count; i++)
	{
		Monster *boo = new Monster(Level, HP, HP);
		Villians.push_back(boo);
		Characters.push_back(boo);
	}
}


int server(const char* ipAdrress, int port)
{
	TcpServer server(ipAdrress,port);
	if (server.init() != 0)
		return -1;

	server.run();

	return 0;
}

int main()
{
	const char* ipAdrress = "127.0.0.1";
	const char* img4map = "map2.jpg";
	int port = 54000;

	//Mailbox.emplace_back("x");
	Game *game = new Game(img4map);
	game->AddNewMonster(50);

	std::thread server_thread(server, ipAdrress,port);
	server_thread.detach();

	while (true)
	{
		lock.lock();
		if (Mailbox.size() != 0)
		{
			for (auto msg = Mailbox.begin(); msg != Mailbox.end(); msg++)
			{
				if (msg->in == true) 
				{
					if (msg->readMsg().empty())
					{
						// Create a new hero
						std::string hero_name = msg->readName();
						Hero *New_Player = new Hero(hero_name);
						game->Players.try_emplace(hero_name, *New_Player);
						game->Characters.push_back(New_Player);

					}
					else
					{
						//refresh Players state
						int KeyPressed = std::stoi(msg->readMsg());
						game->KeyEventHandler(KeyPressed, game->Players[msg->readName()]);

					}

					auto it = msg;
					it--;
					Mailbox.erase(msg);
					msg = it;
					if (Mailbox.empty())
					{
						break;
					}
				}
			}
		}
		game->Invalidate(Mailbox);
		lock.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

}