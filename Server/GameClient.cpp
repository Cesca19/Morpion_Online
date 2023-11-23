#include "GameClient.h"


GameClient::GameClient(SOCKET clientSocket, int id) :
	_clientSocket(clientSocket), _id(id)
{
}

GameClient::~GameClient()
{
}

int GameClient::sendMessage(std::string mess)
{
	int iSendResult = send(_clientSocket, mess.c_str(), (int)mess.size(), 0);
	if (iSendResult == SOCKET_ERROR) {
		closesocket(_clientSocket);
		return 1;
	}
	return 0;
}

int GameClient::getId()
{
	return (_id);
}

SOCKET GameClient::getSocket()
{
	return _clientSocket;
}