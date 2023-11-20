#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "WebServer.h"
//#include "ServerCore.h"

WebServer* WebServer::_webServer = nullptr;

DWORD WINAPI WebServer::MyThreadFunction(LPVOID lpParam)
{
	OutputDebugStringA("THIS start *****************************************************************************************\n");
	WebServer* server = (WebServer*)lpParam;

	server->init();
	server->run();
	OutputDebugStringA("THIS end *****************************************************************************************\n");
	return 0;
}

WebServer* WebServer::getServer()
{
	return _webServer;
}

LRESULT CALLBACK
WebWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return (WebServer::getServer()->wndProc(hwnd, msg, wParam, lParam));
}

WebServer::WebServer(HINSTANCE hInstance, std::string port) : _hInstance(hInstance),
_port(port)
{
	_webServer = this;
}

WebServer::~WebServer()
{
	closesocket(_listenSocket);
	WSACleanup();
}

LRESULT WebServer::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_USER + 1:
	{
		if (LOWORD(lParam) == FD_ACCEPT)
			acceptClient();
		else if (LOWORD(lParam) == FD_CLOSE)
		{
		}
		break;
	}
	case WM_USER + 2:
		if (LOWORD(lParam) == FD_READ)
			readData(wParam, lParam);
		else if (LOWORD(lParam) == FD_CLOSE)
		{
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}

int WebServer::initWindow()
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WebWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = _hInstance;
	wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"WebServer";
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL, L"Call to RegisterClassEx failed!",
			L"Windows Desktop Guided Tour", NULL);
		return 1;
	}
	_hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"WebServer",
		L"WebServer app", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, 100, 100, NULL, NULL, _hInstance, NULL);
	if (!_hwnd) {
		MessageBox(NULL, L"Call to CreateWindow failed!",
			L"Windows Desktop Guided Tour", NULL);
		return 1;
	}
	ShowWindow(_hwnd, SW_SHOW);
	UpdateWindow(_hwnd);
	return 0;
}

int  WebServer::initWinsock()
{
	WSADATA wsaData;
	int iResult;

	//	The WSAStartup function is called to initiate use of WS2_32.dll.
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		std::string mess("WSAStartup failed: " + std::to_string(iResult));
		OutputDebugStringA(mess.c_str());
		return 1;
	}
	return 0;
}

int WebServer::createSocket()
{
	int iResult;
	struct addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; // IPv4 address
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	iResult = getaddrinfo(NULL, _port.c_str(), &hints, &result);
	if (iResult != 0) {
		std::string mess("getaddrinfo failed: " + std::to_string(iResult));
		OutputDebugStringA(mess.c_str());
		WSACleanup();
		return 1;
	}
	_listenSocket = INVALID_SOCKET;
	_listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (_listenSocket == INVALID_SOCKET) {
		std::string mess("Error at socket(): " + std::to_string(WSAGetLastError()));
		OutputDebugStringA(mess.c_str());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	iResult = bind(_listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		std::string mess("bind failed with error: " + std::to_string(WSAGetLastError()));
		OutputDebugStringA(mess.c_str());
		freeaddrinfo(result);
		closesocket(_listenSocket);
		WSACleanup();
		return 1;
	}
	freeaddrinfo(result);
	return 0;
}

int WebServer::initServer()
{
	if (initWinsock())
		return 1;
	if (createSocket())
		return 1;
	WSAAsyncSelect(_listenSocket, _hwnd, WM_USER + 1, FD_ACCEPT | FD_CLOSE);
	if (listen(_listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::string mess("Listen failed with error: " + std::to_string(WSAGetLastError()));
		OutputDebugStringA(mess.c_str());
		closesocket(_listenSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}

int WebServer::init()
{
	if (initWindow())
		return 1;
	if (initServer())
		return 1;
	return 0;
}

int WebServer::sendData(std::string data, SOCKET clientSocket)
{
	int iSendResult = send(clientSocket, data.c_str(), data.size(), 0);
	if (iSendResult == SOCKET_ERROR) {
		OutputDebugStringA(std::string("send failed: " + std::to_string(WSAGetLastError())).c_str());
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}

std::string WebServer::buildResponse(std::string mess)
{
	std::string file = "<!DOCTYPE html><html lang=\"en\"><body><h1> Morpion Game </h1> <p>" + mess + "</p></body></html>";
	std::string header = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + std::to_string(file.size()) + "\n\n";

	return header + file;
}

int WebServer::readData(WPARAM wParam, LPARAM lParam)
{
	//ServerCore* core = (ServerCore*)_core;
	int iResult;
	char recvbuf[DEFAULT_BUFLEN];
	SOCKET clientSocket = (SOCKET)wParam;

	std::string receivedMess;

	//do {
		ZeroMemory(recvbuf, DEFAULT_BUFLEN);
		iResult = recv(clientSocket, recvbuf, DEFAULT_BUFLEN, 0);
		receivedMess += recvbuf;
		if (iResult < 0) {
			std::string mess("recv failed: " + std::to_string(WSAGetLastError()));
			OutputDebugStringA(mess.c_str());
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}
	//} while (iResult > 0);

	OutputDebugStringA(std::string("-------------------------------------------------------------------Mess received in Server: " + receivedMess + "\n").c_str());
	// mess handling
	sendData(buildResponse("Mirror, Mirror on the Wall, Who's the Fairest of Them All?"), clientSocket);
	return 0;
}

int WebServer::acceptClient()
{
	SOCKET ClientSocket;

	ClientSocket = accept(_listenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		OutputDebugStringA(("accept failed: " + std::to_string(WSAGetLastError()) + "\n").c_str());
		closesocket(_listenSocket);
		WSACleanup();
		return 1;
	}
	WSAAsyncSelect(ClientSocket, _hwnd, WM_USER + 2, FD_READ | FD_CLOSE);
	return 0;
}

int WebServer::run()
{
	MSG msg = { 0 };

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

/*
void WebServer::setCore(void* core)
{
	_core = core;
}*/