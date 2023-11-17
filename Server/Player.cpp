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
	int iSendResult = send(_clientSocket, mess.c_str(), (int)mess.size(), 0);
	if (iSendResult == SOCKET_ERROR) {
		closesocket(_clientSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}

std::string Player::getName()
{
	return _name;
}

int Player::getType()
{
	return (_type);
}

SOCKET Player::getSocket()
{
	return _clientSocket;
}