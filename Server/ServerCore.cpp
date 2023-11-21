#include "ServerCore.h"
#include <fstream>

ServerCore::ServerCore(HINSTANCE hInstance, std::string port) :
	Server(hInstance, port), _gameLogic(new Game()), _numPlayers(0)
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

	_numPlayers++;
	if (_numPlayers == 2)
		_gameLogic->initPlayer(_playersVect[0]->getName(), _playersVect[1]->getName());

}

void ServerCore::sendMessageToPlayers(std::string message)
{
	for (int i = 0; i < _playersVect.size(); i++)
		_playersVect[i]->sendMessage(message);
}

std::string ServerCore::getPlayerLastMessage()
{
	std::string tmp = _lastPlayerMessage;
	_lastPlayerMessage = "";
	return tmp;

}

void ServerCore::setLastPlayerMessage(std::string mess)
{
	_lastPlayerMessage = mess;
}

int** ServerCore::getGameMap()
{
	return _gameLogic->getGameMap();
}

void ServerCore::SetHistoricMsg(std::string mess)
{
	std::string HistoricMsg = "";
	HistoricMsg += mess + "\n";
	std::ofstream HistoricFile;
	HistoricFile.close();
	HistoricFile.open("historic.txt", std::ofstream::app);
	if (HistoricFile.is_open()) {
		HistoricFile << HistoricMsg << std::endl;
		HistoricFile.close();
		OutputDebugString(L"aled");

	}
	else
		OutputDebugString(L"error");
	

}