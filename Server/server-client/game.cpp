#include <iostream>


#include "Game.h"

//using namespace std;
using namespace RPGEnv;

const int DeltaPos = 1;

const int SPACE = 32;

std::map<int,MsgHandler> Mailbox_out;
std::list<MsgHandler> Mailbox_in;
std::mutex lock;

Game::Game(const char* &img4map)
{
	srand((unsigned int)time(NULL));
	Interface = new Environment(img4map);
	Character::setMapSize(Interface->SIZE_X, Interface->SIZE_Y);
	
}

void Game::Invalidate(std::map<int,MsgHandler> &Mailbox_out)
{
	std::string message, name;
	std::string delimiter = ";";
	int current_id;
	for (auto it = Characters.begin(); it != Characters.end(); it++)
	{
		current_id = (*it)->id;

		if ((*it)->current_HP <= 0)
		{
			auto i = it;
			i--;
			(*it)->Die();
			Heroes.try_pop(*it);
			Characters.erase(it);
			message = std::to_string(current_id) + ";DEAD\n";
			Mailbox_out[current_id].changeMsg(message);
			it = i;
			continue;
		}

		message = (*it)->Parse();

		
	
		auto message_it = Mailbox_out.find(current_id);

		if (message_it == Mailbox_out.end())
		{
			Mailbox_out.try_emplace(current_id,MsgHandler(name, false, message));
		}
		
		else if (message_it != Mailbox_out.end() && message_it->second.readMsg() != message)
		{
			Mailbox_out[current_id].changeMsg(message);
		}

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

void Game::KeyEventHandler(int keypressed, Hero* PlayerOne)
{


	switch (keypressed)
	{
	case 32:				//space
	{
		Character *Enemy = FindNearest(*PlayerOne);
		if (Enemy != NULL)
		{
			if (CalcDist(*PlayerOne, *Enemy) <= PlayerOne->getRange())
			{
				PlayerOne->Attack(*Enemy);
				PlayerOne->gainXP(PlayerOne->getNormDmg());
				if (0 >= Enemy->current_HP)
				{
					PlayerOne->gainXP((20 * Enemy->Level) - (PlayerOne->Level - Enemy->Level) * 5);
					Characters.remove(Enemy);
					Mailbox_out[Enemy->id].changeMsg(std::to_string(Enemy->id) + ";DEAD\n");
					Enemy->Die();
				}
			}
		}
		break;
	}

	case 99:
	{
		Character* Enemy = FindNearest(*PlayerOne);
		if (Enemy != NULL)
		{
			if (CalcDist(*PlayerOne, *Enemy) <= PlayerOne->getRange())
			{
				
				PlayerOne->SpecialStrike(*Enemy);
				PlayerOne->gainXP(PlayerOne->getSpecDmg());
				if (0 >= Enemy->current_HP)
				{
					PlayerOne->gainXP((20 * Enemy->Level) - (PlayerOne->Level - Enemy->Level) * 5);
					Characters.remove(Enemy);
					Mailbox_out[Enemy->id].changeMsg(std::to_string(Enemy->id) + ";DEAD\n");
					Enemy->Die();
				}
			}
		}
		break;
	}
	case 119:
	{
		PlayerOne->Move(0, -DeltaPos, *Interface);
		break;
	}
	case 97:
	{
		PlayerOne->Move(-DeltaPos, 0, *Interface);
		break;
	}
	case 115:
	{
		PlayerOne->Move(0, DeltaPos, *Interface);
		break;
	}
	case 100:
	{
		PlayerOne->Move(DeltaPos, 0, *Interface);
		break;
	}
	}
	//Invalidate();
	PlayerOne->Exploring(keypressed);
	std::cout << "XP: " << PlayerOne->experience << std::endl;
}
void Game::AddNewMonster(int count, int Level, int HP)
{
	for (int i = 0; i < count; i++)
	{
		Monster *boo = new Monster(Heroes, Level, HP, HP);
		Villians.push_back(boo);
		Characters.push_back(boo);
		MonsterEngageThreads[i] = std::thread(&Monster::Engage, boo, std::ref(Heroes));
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
	std::queue<Hero> heroes;			// -------------------------------------------- <-
	//Mailbox.emplace_back("x");
	Game *game = new Game(img4map);
	//game->AddNewMonster(50);
	std::thread server_thread(server, ipAdrress,port);
	server_thread.detach();

	while (true)
	{
		
		lock.lock();
		if (Mailbox_in.size() != 0)
		{
			for (auto msg = Mailbox_in.begin(); msg != Mailbox_in.end(); msg++)
			{
				if (msg->in == true) 
				{
					// TODO: TEST
					if (msg->readMsg() == "JOINED")
					{

						// DELETE if the Joined one exists
						for (auto it = game->Characters.begin(); it != game->Characters.end(); it++)
						{
							if ((*it)->name == msg->readName()) 
							{
								auto i = it;
								i--;
								(*it)->Die();
								game->Heroes.try_pop(*it);
								game->Characters.erase(it);
								it = i;
								break ;
							}
						}
						// Create a new hero
						std::string hero_name = msg->readName();
						Hero* New_Player = NULL;

						if (hero_name.find("warrior") != std::string::npos)
						{
							Warrior *New_Player = new Warrior(hero_name);
						}
						else if (hero_name.find("mage") != std::string::npos)
						{
							Mage *New_Player = new Mage(hero_name);
						}
						else 
						{
							Warrior *New_Player = new Warrior(hero_name);
						}
						if (New_Player != NULL) 
						{
							std::cout << hero_name << std::endl;
							New_Player->setExplorationMatrix(game->Interface->SIZE_X, game->Interface->SIZE_Y);

							game->Characters.push_back(New_Player);
							game->Heroes.push(New_Player);
						}


					}
					else
					{
						//refresh Heroes state
						std::string message = msg->readMsg();
						if (message.find("JOIN") == std::string::npos) 
						{
							int KeyPressed = std::stoi(message);
							for (auto it = game->Characters.begin(); it != game->Characters.end(); it++)
							{
								if (msg->readName() == (*it)->name)
								{
									game->KeyEventHandler(KeyPressed, (Hero*)(*it));
								}
							}
						}
						
						
					}

					
					Mailbox_in.erase(msg);
					msg = Mailbox_in.begin();
					if (Mailbox_in.empty())
					{
						break;
					}
				}
			}
		}
		game->Invalidate(Mailbox_out);
		lock.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

}