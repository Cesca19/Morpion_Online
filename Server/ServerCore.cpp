#include "ServerCore.h"

ServerCore::ServerCore(HINSTANCE hInstance, std::string port) :
	Server(hInstance, port), _gameLogic(new Game()),
	_lastPlayerMessage("")
{
}

ServerCore::~ServerCore()
{
}

int ServerCore::init()
{
	Server::init();
	//if (_gameServer->init())
		//return 1;
//	_gameServer->setCore(this);
	_gameLogic->initGameMap();
	_gameLogic->setCore(this);
	return 0;
}

void ServerCore::update()
{
//	_gameServer->run();
	_gameLogic->run();
	//return 0;
}

void ServerCore::addPlayer(std::string name)
{
	if (_players.size() < 2)
		_players.push_back(name);
	if (_players.size() == 2)
		_gameLogic->initPlayer(_players[0], _players[1]); 
}

void ServerCore::sendMessageToPlayers(std::string message)
{
	for (int i = 0; i < _playersVect.size(); i++)
		_playersVect[i]->sendMessage(message);
	OutputDebugStringA("SENDING TO PLAYERS----------------\n");
}

std::string ServerCore::getPlayerLastMessage()
{
	return "";
}