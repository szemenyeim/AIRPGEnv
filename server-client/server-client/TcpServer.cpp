#include "TcpServer.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace RPGEnv;

extern std::list<MsgHandler> Mailbox;
extern std::mutex lock;

int TcpServer::init()
{

	//this->Mailbox = mbox;
	// Initialze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		return wsOk;
	}

	// Create a socket
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
	{
		return WSAGetLastError();
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(m_port);
	inet_pton(AF_INET, m_ipAddress, &hint.sin_addr);

	if (bind(m_socket, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}

	// Tell Winsock the socket is for listening 
	if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		return WSAGetLastError();
	}

	// Create the master file descriptor set and zero it
	FD_ZERO(&m_master);

	// Add our first socket that we're interested in interacting with; the listening socket!
	// It's important that this socket is added for our server or else we won't 'hear' incoming
	// connections 
	FD_SET(m_socket, &m_master);

	return 0;
}

int TcpServer::run()
{
	// this will be changed by the \quit command
	bool running = true;

	while (running)
	{
		// Make a copy of the master file descriptor set, this is SUPER important because
		// the call to select() is _DESTRUCTIVE_. The copy only contains the sockets that
		// are accepting inbound connection requests OR messages. 

		// E.g. You have a server and it's master file descriptor set contains 5 items;
		// the listening socket and four clients. When you pass this set into select(), 
		// only the sockets that are interacting with the server are returned. Let's say
		// only one client is sending a message at that time. The contents of 'copy' will
		// be one socket. You will have LOST all the other sockets.

		// SO MAKE A COPY OF THE MASTER LIST TO PASS INTO select() !!!

		fd_set copy = m_master;

		// See who's talking to us
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		// Loop through all the current connections / potential connect
		for (int i = 0; i < socketCount; i++)
		{
			// Makes things easy for us doing this assignment
			SOCKET sock = copy.fd_array[i];

			// Is it an inbound communication?
			if (sock == m_socket)
			{
				// Accept a new connection
				SOCKET client = accept(m_socket, nullptr, nullptr);

				// Add the new connection to the list of connected clients
				FD_SET(client, &m_master);

				onClientConnected(client);
			}
			else // It's an inbound message
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				// Receive message
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					// Drop the client
					onClientDisconnected(sock);
					closesocket(sock);
					FD_CLR(sock, &m_master);
				}
				else
				{
					onMessageReceived(sock, buf, bytesIn);
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	// Remove the listening socket from the master file descriptor set and close it
	// to prevent anyone else trying to connect.
	FD_CLR(m_socket, &m_master);
	closesocket(m_socket);

	while (m_master.fd_count > 0)
	{
		// Get the socket number
		SOCKET sock = m_master.fd_array[0];

		// Remove it from the master file list and close the socket
		FD_CLR(sock, &m_master);
		closesocket(sock);
	}

	// Cleanup winsock
	WSACleanup();
	return 0;
}


void TcpServer::sendToClient(int clientSocket, const char* msg, int length)
{
	send(clientSocket, msg, length, 0);
}

void TcpServer::broadcastToClients(int sendingClient)
{
	//TODO: Ez valami huncutságot mûvel 
	std::string message;
	for (int i = 0; i < m_master.fd_count; i++)
	{
		SOCKET outSock = m_master.fd_array[i];
		if (outSock != m_socket)
		{
			lock.lock();
			// send the message to the client
			for (auto msg = Mailbox.begin(); msg != Mailbox.end(); msg++)
			{
				if (msg->in == false) 
				{
					
					message = msg->readMsg();
					//message += ";" + msg->readMsg();
					const char *c = message.c_str();
					sendToClient(outSock, c, message.length());
					
				}
				std::list<MsgHandler>::iterator it;
				if (msg != Mailbox.begin())
				{
					it = msg;
					it--;
				}
				
				Mailbox.erase(msg);
				it = Mailbox.begin();
				msg = it;
				if (Mailbox.empty())
					break;
				
			}
			lock.unlock();
		}
	}
}

void TcpServer::onClientConnected(int clientSocket)
{
	// Send a welcome message to the connected client
	std::string welcomeMsg = "Welcome to AiRPGEnv_Server\n\r";
	sendToClient(clientSocket, welcomeMsg.c_str(), welcomeMsg.size() + 1);

	char buf[4096];
	ZeroMemory(buf, 4096);

	// Receive message
	int bytesIn = recv(clientSocket, buf, 4096, 0);
	if (bytesIn > 0)
	{
		//Message is the name you should leave a message to create a new hero with
		Mailbox.push_back(MsgHandler(buf,true));
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

}

void TcpServer::onClientDisconnected(int clientSocket)
{

}

void TcpServer::onMessageReceived(int clientSocket, const char* msg, int length)
{
	std::vector<std::string> splitted;
	std::string token;
	std::istringstream tokenStream(msg);
	while (std::getline(tokenStream, token, ':'))
	{
		splitted.push_back(token);
	}
	if ("0" != splitted[1])
	{
		Mailbox.push_back(MsgHandler(splitted[0], true, splitted[1]));
	}
	
	
	broadcastToClients(clientSocket);
	std::cout << msg << std::endl;
}
