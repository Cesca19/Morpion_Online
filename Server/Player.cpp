#include "Player.h"

Player::Player(SOCKET clientSocket, PlayerType type) : 
	_clientSocket(clientSocket), _type(type)
{
}

Player::~Player()
{
}

void Player::setName(std::string name)
{
	_name = name;
}

int Player::sendMessage(std::string mess)
{
	int iSendResult = send(_clientSocket, mess.c_str(), mess.size(), 0);
	if (iSendResult == SOCKET_ERROR) {
		closesocket(_clientSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}

SOCKET Player::getSocket()
{
	return _clientSocket;
}