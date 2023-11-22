#pragma once
#include "Server.h"
#include "Game.h"
#include <iostream>

#include "pch.h"
#include "Game.h"
#include "Player.h"
#include "Server.h"
#include "WebServer.h"

class ServerCore
{
public:
	ServerCore(HINSTANCE hInstance, std::string port);
	~ServerCore();
	int init();
	void update();
	void addPlayer(std::string name);

	int initWindow();

	static ServerCore* getServerCore();
	LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void dispatchGameMessage(WPARAM wParam, LPARAM lParam);
	void addNewGameClient(WPARAM wParam, LPARAM lParam);
	void setGameServer(WPARAM wParam, LPARAM lParam);
	void setWebServer(WPARAM wParam, LPARAM lParam);

	void dispatchWebMessage(WPARAM wParam, LPARAM lParam);

	void sendMessageToPlayers(std::string Message);
	void sendMessageToPlayer(std::string name, std::string Message);
	std::string getPlayerLastMessage();

	void setLastPlayerMessage(std::string mess);
	int** getGameMap();
	std::string getGameInfos();

	void SetHistoricMsg(std::string mess);
private:
	HINSTANCE _hInstance = nullptr;
	HWND _hwnd = nullptr;
	static ServerCore* _serverCore;

	HWND _gameServerHwnd = nullptr;
	HWND _webServerHwnd = nullptr;
	HANDLE  _serversThreadArray[2];

	std::shared_ptr<Game> _gameLogic;
	std::string _lastPlayerMessage;
	int _numPlayers;
	bool _hasStart;

	std::vector<std::shared_ptr<Player>> _playersVect;
	std::unordered_map<int, std::shared_ptr<Player>> _playersMap;
	std::unordered_map<std::string, std::shared_ptr<Player>> _playersNameMap;
};