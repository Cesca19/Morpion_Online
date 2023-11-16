#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

enum PlayerType {
	SPECTATOR,
	PLAYER1,
	PLAYER2
};

class Player
{
public:
	Player(SOCKET clientSocket, PlayerType type);
	~Player();
	void setName(std::string name);
	int sendMessage(std::string mess);
private:
	SOCKET _clientSocket;
	PlayerType _type;
	std::string _name;
};

