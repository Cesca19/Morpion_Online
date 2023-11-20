#pragma once
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <memory>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "6666"
#define DEFAULT_BUFLEN 512


class Client
{
public:
	Client(HINSTANCE hInstance);
	~Client();
	int init();
	int run();

	int sendData(std::string data);
	std::string readData(WPARAM wParam, LPARAM lParam);

	void setCore(void* core);
	static Client* getClient();
	LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	int initWindow();

	int initWinsock();
	int initClient();
	int createSocket();



	HINSTANCE _hInstance = nullptr;
	HWND _hwnd = nullptr;

	SOCKET _connectSocket;
	std::string _ipAddress = "127.0.0.1";
	std::string _port = "6666";

	std::string _name;

	static Client* _client;

	void* _clientCore;
};

