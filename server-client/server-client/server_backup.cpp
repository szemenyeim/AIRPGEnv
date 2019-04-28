#include <iostream>
#include <WS2tcpip.h>
#include <string>

using namespace std;

#pragma comment (lib, "ws2_32.lib")

int main()
{
	//initialize winsock
	
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOK = WSAStartup(ver, &wsData);
	if (wsOK != 0)
	{
		cerr << "Can't initialise winsock! Quitting" << endl;
		return 1;
	}

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
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	//socket is for listening
	listen(listening, SOMAXCONN);
	
	//wait for connection
	sockaddr_in client;
	int clientsize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientsize);

	char host[NI_MAXHOST];		//client remote name
	char service[NI_MAXHOST];	//port the client is connected on

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port" << service << endl;
	}
	else 
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port" << ntohs(client.sin_port) << endl;
	}
	//close listening socket
	closesocket(listening);

	//accept message back to the client 
	
	char buf[4096];
	
	while (true)
	{
		ZeroMemory(buf, 4096);
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			cerr << "Error in recv(). Quitting " << endl;
			break;
		}
		if (bytesReceived == 0)
		{
			cout << "Client disconnected" << endl;
			break;
		}
		cout << "CLIENT > " << string(buf, 0, bytesReceived) << endl;
		send(clientSocket, buf, bytesReceived + 1, 0);
	}
	
	//close socket
	closesocket(clientSocket);
	
	//cleanup winsock
	WSACleanup();
}