#pragma once

#include "Client.h"
#include "Morpion.h"
#include "SFML/Graphics.hpp"
#include <iostream>

class ClientCore
{
public :
	ClientCore(HINSTANCE hInstance);
	~ClientCore();
	int init(std::string windowName, int width, int height);
	int run();
	void analyseMessage(std::string message);
	void sendMessage(std::string message);
	int** getGameMap();
	void setGameMap(int **);
	void setCurrentPlayer(std::string name);
	
private:
	Client* _client;
	std::string _name;
	Morpion* _game;
	int** _map;

	HANDLE _clientThread;
};

