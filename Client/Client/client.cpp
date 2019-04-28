#pragma once
#include <iostream>
#include <string>
#include <WS2tcpip.h>

#include <thread>
#include <chrono>
#include <mutex>
#include "character.h"
#include "Non-playable-character.h"
#include "hero.h"
#include "gui.h"

#pragma comment(lib,"ws2_32.lib")

const char* img4map = "map2.jpg";
const char* window_name = "AiPG";

std::list<RPGEnv::Character*> Characters;

int my_xpos, my_ypos;
using namespace std;


/*Global variables*/
int KEY_PRESSED;
std::mutex key_lock;


void DrawAll(std::string message)
{
	string token, monster;
	string delimiter = ";";
	std::vector<string> characters_from_msg, params;
	size_t pos = 0;
	while ((pos = message.find("\n")) != string::npos)
	{
			token = message.substr(0, pos);
			characters_from_msg.push_back(token);
			message.erase(0, pos + 2);
	}
	for (auto it = characters_from_msg.begin(); it != characters_from_msg.end(); it++)
	{
		monster = (*it);
		while ((pos = monster.find(delimiter)) != string::npos)
		{
			
			token = monster.substr(0, pos);
			params.push_back(token);
			monster.erase(0, pos + delimiter.length());
		}
		params.push_back(monster);
		if (!params.empty())
		{
			if (params[1] == "Monster")
			{
				RPGEnv::Monster * New =
					new RPGEnv::Monster(
						stoi(params[2]), stoi(params[3]),
						stoi(params[4]), stoi(params[5]), stoi(params[6])
					);
				Characters.push_back(New);
			}
			else
			{
				RPGEnv::Hero *New = new RPGEnv::Hero(
					params[1], stoi(params[2]), stoi(params[3]),
					stoi(params[4]), stoi(params[5]), stoi(params[6]));
			}
			params.clear();
		}
	}
	characters_from_msg.clear();
	
}


void Invalidate(GUI *Interface)
{
	Interface->ClearWindow();
	for (auto it = Characters.begin(); it != Characters.end(); it++)
	{
		(*it)->Draw(*Interface);
	}
	Characters.clear();
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
		
		ZeroMemory(buf, 4096);
		int bytesReceived = recv(sock, buf, 4096, 0);
		
		if (bytesReceived > 0)
		{
			message = string(buf, 0, bytesReceived);
			//Echo response to console
			cout << "SERVER > " << message << endl;
			DrawAll(message);

		}

		//TODOD::MUTEX
		key_lock.lock();
		userInput = PlayerName + ":" + to_string(KEY_PRESSED);
		if (userInput.size() > 0)
		{
			//Send the text
			int sendResult = send(sock, userInput.c_str(), (int)userInput.size() + 1, 0);
			KEY_PRESSED = 0;

		}
		key_lock.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		
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

	std::thread client_thread(client, ipAddress, port, PlayerName);
	client_thread.detach();

	//img4map name shall be get from the server
	GUI *Interface = new GUI(img4map, window_name);
	while (true)
	{
		key_lock.lock();
		KEY_PRESSED = GUI::GetKeyPressed();
		key_lock.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		Invalidate(Interface);
	}
	
}