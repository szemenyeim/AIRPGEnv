#pragma once
#include <iostream>
#include <string>
#include <WS2tcpip.h>

#include <thread>
#include <chrono>
#include <ctime>
#include <mutex>
#include <map>
#include "character.h"
#include "Non-playable-character.h"
#include "hero.h"
#include "gui.h"

#pragma comment(lib,"ws2_32.lib")

const char* img4map = "map2.jpg";
const char* window_name = "AiPG";

std::map<int,RPGEnv::Character*> Characters;

int my_xpos, my_ypos,my_id;
using namespace std;


/*Global variables*/
int KEY_PRESSED;
std::mutex key_lock;


void DrawAll(std::string message, std::string PlayerName)
{
	key_lock.lock();
	string token, monster;
	string delimiter = ";";
	std::vector<string> characters_from_msg, params;
	size_t pos = 0;

	//parsing the whole message
	while ((pos = message.find("\n")) != string::npos)
	{
			token = message.substr(0, pos);
			characters_from_msg.push_back(token);
			message.erase(0, pos + 2);
	}
	
	//getting the realevant information from the monsters
	for (auto msg_it = characters_from_msg.begin(); msg_it != characters_from_msg.end(); msg_it++)
	{
		params.clear();
		
		monster = (*msg_it);
		while ((pos = monster.find(delimiter)) != string::npos)
		{
			
			token = monster.substr(0, pos);
			params.push_back(token);
			monster.erase(0, pos + delimiter.length());
		}
		params.push_back(monster);

		int id, x_pos, y_pos, level, currentHP, maximumHP;
		
		//Deleting a Char

		
		if (params.size() == 2 && params[1] == "DEAD")
		{
			Characters.erase(stoi(params[0]));
		}

		if (params.size() != 7)
			continue;

		try
		{
			id = stoi(params[0]);
			x_pos = stoi(params[2]);
			y_pos = stoi(params[3]);
			level = stoi(params[4]);
			currentHP = stoi(params[5]);
			maximumHP = stoi(params[6]);
		}
		catch (const std::exception&)
		{
			continue;
		}
		
		std::string name = params[1];
		

		auto char_it = Characters.find(id);

		//Creating a new Character
		if (!params.empty() && char_it == Characters.end())
		{
			//Creating a new monster
			if (params[1] == "Monster")
			{
				RPGEnv::Monster * New =
					new RPGEnv::Monster(x_pos, y_pos, level, maximumHP, currentHP);
				Characters.try_emplace(id, New);
			}

			//Creating a new Hero
			else
			{
				RPGEnv::Hero *New =
					new RPGEnv::Hero(name, x_pos, y_pos, level, maximumHP, currentHP);

				Characters.emplace(id, New);

				if (New->name == PlayerName)
				{
					my_xpos = x_pos;
					my_ypos = y_pos;
					my_id = id;
				}
			}
			
		}
	
		//refreshing gamestate
		else if (char_it != Characters.end())
		{
			Characters[id]->position.x = x_pos;
			Characters[id]->position.y = y_pos;
			Characters[id]->current_HP = currentHP;
			Characters[id]->maximum_HP = maximumHP;
			if (my_id = id)
			{
				my_xpos = x_pos;
				my_ypos = y_pos;
			}
		}
	}
	characters_from_msg.clear();
	key_lock.unlock();
}


void Invalidate(GUI *Interface)
{
	Interface->ClearWindow();
	for (auto it = Characters.begin(); it != Characters.end(); it++)
	{
		(*it).second->Draw(*Interface);
	}
	Interface->ShowWindow(my_xpos, my_ypos);
}

void client(string ipAddress, int port, string PlayerName)
{

	//Initialize WinSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cerr << "Can't start winsock, Err #" << wsResult << endl;
		return;
	}

	//Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	DWORD timeout = 0.001 * 1000;			//1ms timeout
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

	if (sock == INVALID_SOCKET)
	{
		cerr << "Can't create socket Err #" << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	//Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	//Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return;
	}

	//Send PlayerName to the server when connected

	int sendResult = send(sock, PlayerName.c_str(), (int)PlayerName.size() + 1, 0);


	//Do-while loop to send and received data
	char buf[4096];
	string userInput;
	string message;

	while(true)
	{
	
		//cout << "client" << endl;
		ZeroMemory(buf, 4096);
		int bytesReceived = recv(sock, buf, 4096, 0);
		
		//send(sock, "c", 2, 0);
		
		if (bytesReceived > 0)
		{
			message = string(buf, 0, bytesReceived);
			//Echo response to console
			cout << "SERVER > " << message << endl;
			DrawAll(message, PlayerName);
			
		}
		
		//TODOD::MUTEX
		key_lock.lock();
		userInput = PlayerName + ":" + to_string(KEY_PRESSED);
		if (userInput.size() > 0 && KEY_PRESSED > 0)
		{
			//Send the text
			int sendResult = send(sock, userInput.c_str(), (int)userInput.size() + 1, 0);
			KEY_PRESSED = 0;

		}
		key_lock.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		
	}
	//Gracefully close down everything
	closesocket(sock);
	WSACleanup();
}


int main()
{
	
	string ipAddress = "127.0.0.1";
	int port = 54000;
	string PlayerName = "Apa";

	// create thread for client and start the communication
	std::thread client_thread(client, ipAddress, port, PlayerName);
	client_thread.detach();

	// img4map name shall be get from the server
	// init the GUI
	GUI *Interface = new GUI(img4map, window_name);
	while (true)
	{
		//cout << "gui" << std::endl;

		key_lock.lock();

		// getting keypresses
		KEY_PRESSED = GUI::GetKeyPressed();
		Invalidate(Interface);
		key_lock.unlock();
		
		// give chance to the client thread to communicate
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		
	}
	
}