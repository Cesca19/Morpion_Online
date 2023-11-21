#pragma once

#include "pch.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
 
#include "Player.h"

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "6666"
#define DEFAULT_BUFLEN 512

class Server
{
public:
	Server(HINSTANCE hInstance, std::string port);
	~Server();
	int init();
	int run();
	static Server* getServer();
	LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	std::vector<std::shared_ptr<Player>> getPlayersList();
	std::unordered_map<std::string, std::shared_ptr<Player>> getPlayers();
	void sendMessageToPlayers(std::string message);
	void sendMessageToPlayer(std::string name, std::string message);
	void setCore(void* core);

private:
	int initWindow();
	int initWinsock();
	int createSocket();
	int acceptClient();
	int initServer();
	int sendData(std::string data, SOCKET clientSocket);
	int readData(WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE _hInstance = nullptr;
	HWND _hwnd = nullptr;

	SOCKET _listenSocket;
	std::string _port;
	static Server* _server;
	void* _core;

	std::string _lastPlayerMessage;
	int _id;
	std::vector<std::shared_ptr<Player>> _playersVect;
	std::unordered_map<SOCKET, std::shared_ptr<Player>> _playersMap;
	std::unordered_map<std::string, std::shared_ptr<Player>> _playersNameMap;
};
