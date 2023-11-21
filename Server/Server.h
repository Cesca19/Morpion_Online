#pragma once

#include "pch.h"
#include "GameClient.h"

#define DEFAULT_PORT "6666"
#define DEFAULT_BUFLEN 512

class Server
{
public:
	Server(HINSTANCE hInstance, std::string port);
	~Server();
	int init();
	int run();
	void setCore(HWND coreHwnd);
	LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static Server* getServer();
	static DWORD WINAPI MyThreadFunction(LPVOID lpParam);

private:
	int initWindow();
	int initWinsock();
	int createSocket();
	int acceptClient();
	int initServer();
	int sendData(std::string data, SOCKET clientSocket);
	int readData(WPARAM wParam, LPARAM lParam);
	void sendMessageToPlayers(WPARAM wParam, LPARAM lParam);
	void sendMessageToPlayer(WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE _hInstance = nullptr;
	HWND _hwnd = nullptr;
	HWND _coreHwnd = nullptr;

	SOCKET _listenSocket;
	std::string _port;
	static Server* _server;

	std::string _lastPlayerMessage;
	int _id;
	std::vector<std::shared_ptr<GameClient>> _clientsVect;
	std::unordered_map<SOCKET, std::shared_ptr<GameClient>> _clientsMap;
};