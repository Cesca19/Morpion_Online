#pragma once

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
	virtual int init();
	virtual int run();
	//void setCore(void* core);
	static Server* getServer();
	LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	virtual void update() = 0;
	virtual void addPlayer(std::string name) = 0;

private:
	int initWindow();

	int initWinsock();
	int createSocket();
	int acceptClient();
	int initServer();
protected:
	int sendData(std::string data, SOCKET clientSocket);
	int readData(WPARAM wParam, LPARAM lParam);

	HINSTANCE _hInstance = nullptr;
	HWND _hwnd = nullptr;

	SOCKET _listenSocket;
	std::string _port;


	static Server* _server;
	///void* _core;
protected:
	std::string _lastPlayerMessage;
	int _id;
	std::vector<std::shared_ptr<Player>> _playersVect;
	std::unordered_map<SOCKET, std::shared_ptr<Player>> _playersMap;
};
