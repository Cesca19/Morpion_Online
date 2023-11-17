#include "ServerCore.h"

ServerCore::ServerCore(HINSTANCE hInstance, std::string port) : _server(new Server(hInstance, port)),
_gameLogic(new Game()), _numPlayers(0),
	_hasStart(false)
{
}

ServerCore::~ServerCore()
{
}

int ServerCore::init()
{
	_server->init();
	_server->setCore(this);
	_gameLogic->initGameMap();
	_gameLogic->setCore(this);
	return 0;
}

void ServerCore::update()
{
	MSG msg = { 0 };
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		_gameLogic->run();
	}
}

void ServerCore::addPlayer(std::string name)
{
	std::vector<std::shared_ptr<Player>> playersVect =  _server->getPlayersList();
	std::unordered_map<std::string, std::shared_ptr<Player>> players = _server->getPlayers();
	
	if (players[name]->getType())
		_gameLogic->addPlayer(name, players[name]->getType());
	else
		_gameLogic->addWatcher(name);
	/*	_numPlayers++;
	if (_numPlayers == 2) {
		_gameLogic->initPlayer(playersVect[0]->getName(), playersVect[1]->getName());
		_hasStart = true;
		_numPlayers++;
	}
	if (_hasStart)
		_server->sendMessageToPlayer(name, "S#");*/
}

void ServerCore::sendMessageToPlayers(std::string message)
{
	_server->sendMessageToPlayers(message);
}

void ServerCore::sendMessageToPlayer(std::string name, std::string Message)
{
	_server->sendMessageToPlayer(name, Message);
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