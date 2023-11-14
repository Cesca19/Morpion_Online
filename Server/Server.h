#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <memory>
#include <vector>

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

private:
	int initWindow();

	int initWinsock();
	int createSocket();
	int acceptClient();
	int initServer();
	int sendData(std::string data, SOCKET clientSocket);
	int readData(WPARAM wParam, LPARAM lParam);

	HINSTANCE _hInstance = nullptr;
	HWND _hwnd = nullptr;

	SOCKET _listenSocket;
	std::string _port;
	std::vector<SOCKET> _clientSockets;

	static Server* _server;

};

