#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <memory>
#include <vector>
#include <iostream>

#define DEFAULT_BUFLEN 512

class WebServer
{
public:
	WebServer(HINSTANCE hInstance, std::string port);
	~WebServer();

	int init();
	int run();
	static DWORD WINAPI MyThreadFunction(LPVOID lpParam);
	static WebServer* getServer();
	LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void setCore(void* core);


private:
	int initWindow();
	int initWinsock();
	int createSocket();
	int acceptClient();
	int initServer();
	
	std::string buildResponse(std::string mess);
	std::string convertGameMap(int** map);
	int sendGameMap(SOCKET clientSocket);
	int sendData(std::string data, SOCKET clientSocket);
	int readData(WPARAM wParam, LPARAM lParam);
private:
	HINSTANCE _hInstance = nullptr;
	HWND _hwnd = nullptr;

	SOCKET _listenSocket;
	std::string _port;
	static WebServer* _webServer;
	void* _core;
};