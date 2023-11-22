#pragma once

#include "pch.h"
#include "Game.h"
#include "Player.h"
#include "Server.h"
#include "WebServer.h"
#include "ServerUI.h"

enum SERVER_STATE {
	NOT_INIT,
	NOT_RUNNING,
	RUN,
	IS_RUNNING,
	STOP
};

class ServerCore
{
public:
	ServerCore(HINSTANCE hInstance, std::string port);
	~ServerCore();
	int init();
	void run();
	void addPlayer(std::string name);
	void setState(SERVER_STATE state);
	void lauchServer();
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
private:
	SERVER_STATE _serverState;
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

	std::shared_ptr<ServerUI> _serverUI;
};