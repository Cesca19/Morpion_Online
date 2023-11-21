#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

class GameClient
{
public:
	GameClient(SOCKET clientSocket, int id);
	~GameClient();
	int sendMessage(std::string mess);
	SOCKET getSocket();
	int getId();
private:
	SOCKET _clientSocket;
	int _id;
};

