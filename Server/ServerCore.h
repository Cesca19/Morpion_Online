#pragma once
#include "Server.h"
#include "Game.h"

class ServerCore
{
public:
	ServerCore(HINSTANCE hInstance, std::string port);
	~ServerCore();
	int init();
	void update();
	void addPlayer(std::string name);

	void sendMessageToPlayers(std::string Message);
	std::string getPlayerLastMessage();
	void setLastPlayerMessage(std::string mess);
private:
	std::shared_ptr<Game> _gameLogic;
	std::shared_ptr<Server> _server;
	std::string _lastPlayerMessage;
	int _numPlayers;
	bool _hasStart;
};

