#pragma once

#include <WS2tcpip.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <list>

#pragma comment (lib, "ws2_32.lib")

namespace RPGEnv {
	class MsgHandler
	{
	private:
		std::string msg;
		std::string name;
		
	public:

		bool in;
		//std::mutex lock;


		std::string readName()
		{
			return this->name;
		}

		std::string readMsg()
		{
			return this->msg;
		}
		
		MsgHandler(std::string name, bool in ,std::string msg = "")
		{
			this->in = in;
			this->name = name;
			this->msg = msg;
		}
	};


	class TcpServer
	{

	public:

		TcpServer(const char* ipAddress, int port) :
			m_ipAddress(ipAddress), m_port(port) { }

		// Initialize the listener
		int init();

		// Run the listener
		int run();

	protected:

		// Handler for client connections
		virtual void onClientConnected(int clientSocket);

		// Handler for client disconnections
		virtual void onClientDisconnected(int clientSocket);

		// Handler for when a message is received from the client
		virtual void onMessageReceived(int clientSocket, const char* msg, int length);

		// Send a message to a client
		void sendToClient(int clientSocket, const char* msg, int length);

		// Broadcast a message from a client
		void broadcastToClients(int sendingClient);

	private:

		const char*		m_ipAddress;	// IP Address server will run on
		int				m_port;			// Port # for the web service
		int				m_socket;		// Internal FD for the listening socket
		fd_set			m_master;		// Master file descriptor set
		//std::list<MsgHandler> *Mailbox;
	};
}