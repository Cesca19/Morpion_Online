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
	static ClientCore* getClientCore();
	LRESULT coreWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	int init(std::string windowName, int width, int height);
	int run();
	int close();
	int closeClient();
	int initWindow();
	void analyseMessage(std::string message);
	void sendMessage(std::string message);
	int** getGameMap();
	void setGameMap(int **);
	void setCurrentPlayer(std::string name);
	void setGameClient(WPARAM wParam, LPARAM lParam);
	void analyseMessage(WPARAM wParam, LPARAM lParam);
private:
	HINSTANCE _hInstance = nullptr;
	HWND _hwnd = nullptr;
	HWND _clientHwnd = nullptr;
	HANDLE  _clientThread;

	static ClientCore* _clientCore;

	bool _isRunning;
	std::string _gamePort;
	std::string _ip;

	std::string _name;
	std::shared_ptr<Morpion> _game;
	int** _map;
};

