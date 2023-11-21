#pragma once
#include "Server.h"
#include "Game.h"
#include <iostream>

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

	void setLastPlayerMessage(std::string mess);
	int** getGameMap();

	void SetHistoricMsg(std::string mess);

private:
	std::shared_ptr<Game> _gameLogic;
	int _numPlayers;
};

