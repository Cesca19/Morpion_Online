#pragma once
#include "pch.h"

#define NOMINMAX
#define DEFAULT_PORT "6666"
#define DEFAULT_BUFLEN 512


class Client
{
public:
	Client(HINSTANCE hInstance, std::string address, std::string port);
	~Client();
	int init();
	int run();
	int close();

	int sendData(WPARAM wParam, LPARAM lParam);
	std::string readData(WPARAM wParam, LPARAM lParam);
	static DWORD WINAPI MyThreadFunction(LPVOID lpParam);

	void setCore(HWND coreHwnd);
	static Client* getClient();
	LRESULT clientWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	int initWindow();
	int initWinsock();
	int initClient();
	int createSocket();


	HINSTANCE _hInstance = nullptr;
	HWND _hwnd = nullptr;
	HWND _coreHwnd = nullptr;

	SOCKET _connectSocket;
	std::string _ipAddress;
	std::string _port;

	std::string _name;
	static Client* _client;
};

