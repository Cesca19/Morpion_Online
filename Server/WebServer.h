#pragma once

#include "pch.h"

#define DEFAULT_BUFLEN 512

class WebServer
{
public:
	WebServer(HINSTANCE hInstance, std::string port);
	~WebServer();

	int init();
	int run();
	void close();
	static DWORD WINAPI MyThreadFunction(LPVOID lpParam);
	static WebServer* getServer();
	LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void setCore(HWND coreHwnd);


private:
	int initWindow();
	int initWinsock();
	int createSocket();
	int acceptClient();
	int initServer();
	
	std::string buildResponse(std::string mess);
	std::string convertGameMap(int** map);
	int sendGameMap(WPARAM wParam, LPARAM lParam);
	int sendData(std::string data, SOCKET clientSocket);
	int readData(WPARAM wParam);
	
	HINSTANCE _hInstance = nullptr;
	HWND _hwnd = nullptr;
	HWND _coreHwnd = nullptr;

	SOCKET _listenSocket;
	std::string _port;
	static WebServer* _webServer;
};