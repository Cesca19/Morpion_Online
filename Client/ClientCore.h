#pragma once
#include "Client.h"
#include "Game.h"
#include "SFML/Graphics.hpp"
#include <iostream>

class ClientCore
{
public :
	ClientCore();
	~ClientCore();
	int init(std::string windowName, int width, int height,Client &client);
	int run();
	Game* _game;
};

