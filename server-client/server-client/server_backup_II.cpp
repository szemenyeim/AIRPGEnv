#include <iostream>
#include <WS2tcpip.h>
#include <string>

using namespace std;

#pragma comment (lib, "ws2_32.lib")

int main()
{
	string ipAddress = "127.0.0.1";
	int port = 54000;

	//initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOK = WSAStartup(ver, &wsData);
	if (wsOK != 0)
	{
		cerr << "Can't initialise winsock! Quitting" << endl;
		return 1;
	}
	/////////////////////////////////////////////////////////////////////////////////
	// create socket
	
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		return 1;
	}

	//Bind the socket to an ip address and port 
	
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	//socket is for listening
	listen(listening, SOMAXCONN);
	
	fd_set master;
	FD_ZERO(&master);

	FD_SET(listening, &master);
	/////////////////////////////////////////////////////////////////////////
	while (true)
	{
		fd_set copy = master;

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);
		
		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];
			if (sock == listening)
			{
				//Accept new connection
				SOCKET client = accept(listening, nullptr, nullptr);

				//Add the new connection to the list of connected clients
				FD_SET(client, &master);
				
				//Send a welcome message to the connected client
				string welcomeMsg = "anyad";
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);

				// TODO: Broadcast we have a new connection
			}
			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);
				
				//Receive message
				int bytesReceive = recv(sock, buf, 4096, 0);
				if (bytesReceive <= 0)
				{
					//Drop client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					//Send message to other clients, and definiately NOT the listening socket
					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock)
						{
							send(outSock, buf, bytesReceive, 0);
						}
					}
				}
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////
	//cleanup winsock
	WSACleanup();
}