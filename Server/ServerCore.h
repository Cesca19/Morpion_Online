#pragma once
#include "Server.h"
#include "Game.h"

class ServerCore : public Server 
{
public:
	ServerCore(HINSTANCE hInstance, std::string port);
	~ServerCore();
	virtual int init() override;
	virtual void update() override;
	void addPlayer(std::string name);

	void sendMessageToPlayers(std::string Message);
	std::string getPlayerLastMessage();
private:
	std::string _lastPlayerMessage;
	std::shared_ptr<Game> _gameLogic;
	//std::shared_ptr<Server> _gameServer;

	std::vector<std::string> _players;
};

