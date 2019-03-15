#include <iostream>
#include "Game.h"

using namespace RPGEnv;

const char* img4map = "map2.jpg";
const char* window_name = "AiPG";
const int DeltaPos = 1;

Game::Game(const char* &img4map, const char* &window_name)
{
	srand(time(NULL));
	Interface = new GUI(img4map, window_name);
	Character::setMapSize(Interface->SIZE_X, Interface->SIZE_Y);
	PlayerOne = Hero();
	Characters.push_back(&PlayerOne);
	Invalidate();

}

void Game::Invalidate()
{
	Interface->ClearWindow();
	for (auto it = Characters.begin(); it != Characters.end(); it++)
	{
		(*it)->Draw(*Interface);
	}
	Interface->ShowWindow(PlayerOne.position.x, PlayerOne.position.y );
}

Character * RPGEnv::Game::FindNearest(Character & You)
{
	Character* nearest = nullptr;
	int min_dist=5;
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

void Game::KeyEventHandler() 
{
	
	int keypressed = GUI::GetKeyPressed();
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
	{	PlayerOne.Move(0, -DeltaPos, *Interface);
	break;
	}
	case (int)'a':
	{
		PlayerOne.Move(-DeltaPos, 0, *Interface);
		break;
	}
	case (int)'s':
	{
		PlayerOne.Move(0, DeltaPos, *Interface);
		break;
	}
	case (int)'d':
	{
		PlayerOne.Move(DeltaPos, 0 , *Interface);
		break;
	}
	}
	Invalidate();
}

void Game::AddNewMonster(int count = 1, int Level = 1, int HP = 100)
{
	for (int i = 0; i < count; i++)
	{
		Monster *boo = new Monster(Level, HP, HP);
		Villians.push_back(boo);
		Characters.push_back(boo);
		Invalidate();
	}
}

int main()
{
	// initializing the game
	Game *game = new Game(img4map, window_name);
	game->AddNewMonster(50);
	while (true)
	{
		game->KeyEventHandler();
	}
	
	return 0;

}
