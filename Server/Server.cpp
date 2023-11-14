#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Server.h"

Server* Server::_server = nullptr;

void print(std::string mess)
{
	std::wstring res(mess.begin(), mess.end());
	OutputDebugStringW(res.c_str());
}

Server* Server::getServer()
{
	return _server;
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return (Server::getServer()->wndProc(hwnd, msg, wParam, lParam));
}

Server::Server(HINSTANCE hInstance, std::string port) : _hInstance(hInstance),
_port(port)
{
	_server = this;
}

Server::~Server()
{
	//	###		DISCONNECT THE SERVER		###
		// shutdown each client socket since no more data will be sent
	/*iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	
	// cleanup
	closesocket(clientSocket);*/
	closesocket(_listenSocket);
	WSACleanup();

}

LRESULT Server::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_USER + 1:
		if (LOWORD(lParam) == FD_ACCEPT)
			acceptClient();
		else if (LOWORD(lParam) == FD_CLOSE)
		{ }
		break;
	case WM_USER + 2:
		if (LOWORD(lParam) == FD_READ)
			readData(wParam, lParam);
		else if (LOWORD(lParam) == FD_CLOSE)
		{}
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

int Server::initWindow()
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = MainWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = _hInstance;
	wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"Server";
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL, L"Call to RegisterClassEx failed!",
			L"Windows Desktop Guided Tour", NULL);
		return 1;
	}
	_hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"Server",
		L"Server app", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 
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

int  Server::initWinsock()
{
	WSADATA wsaData;
	int iResult;

	//	The WSAStartup function is called to initiate use of WS2_32.dll.
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		std::string mess("WSAStartup failed: " + std::to_string(iResult));
		MessageBoxA(nullptr, mess.c_str(), "Error", 0);
		return 1;
	}
	return 0;
}

int Server::createSocket()
{
	int iResult;

	//	###		CREATING A SOCKET FOR THE SERVER	###
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; // IPv4 address
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		std::string mess("getaddrinfo failed: " + std::to_string(iResult));
		MessageBoxA(nullptr, mess.c_str(), "Error", 0);
		WSACleanup();
		return 1;
	}
	_listenSocket = INVALID_SOCKET;
	// Create a SOCKET for the server to listen for client connections
	_listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (_listenSocket == INVALID_SOCKET) {
		std::string mess("Error at socket(): " + std::to_string(WSAGetLastError()));
		MessageBoxA(nullptr, mess.c_str(), "Error", 0);
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	//	###		BINDING THE SOCKET		####
	// For a server to accept client connections, it must be bound to a 
	// network address within the system.
	// Setup the TCP listening socket
	iResult = bind(_listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		std::string mess("bind failed with error: " + std::to_string(WSAGetLastError()));
		MessageBoxA(nullptr, mess.c_str(), "Error", 0);
		freeaddrinfo(result);
		closesocket(_listenSocket);
		WSACleanup();
		return 1;
	}
	freeaddrinfo(result);
	return 0;
}

int Server::initServer()
{
	if (initWinsock())
		return 1;
	if (createSocket())
		return 1;
	//MessageBox(0, L"done binding", L"Hi", 0);
	WSAAsyncSelect(_listenSocket, _hwnd, WM_USER + 1, FD_ACCEPT | FD_CLOSE);
	//	###		LISTEN ON A SOCKET		###
	if (listen(_listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::string mess("Listen failed with error: " + std::to_string(WSAGetLastError()));
		MessageBoxA(nullptr, mess.c_str(), "Error", 0);
		closesocket(_listenSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}

int Server::init()
{
	// create the window
	if (initWindow())
		return 1;
	// create the server
	if(initServer())
		return 1;
	return 0;
}

int Server::sendData(std::string data, SOCKET clientSocket)
{
	int iSendResult = send(clientSocket, data.c_str(), data.size(), 0);
	if (iSendResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}

int Server::readData(WPARAM wParam, LPARAM lParam)
{
	int iResult;
	char recvbuf[DEFAULT_BUFLEN];
	SOCKET clientSocket = (SOCKET)wParam;
	
	do {
		ZeroMemory(recvbuf, DEFAULT_BUFLEN);
		iResult = recv(clientSocket, recvbuf, DEFAULT_BUFLEN, 0);
		std::string mess("Mess received in Server: " + std::string(recvbuf));
		print(mess + "\n");
		MessageBoxA(nullptr, mess.c_str(), "read Data", 0);
		if (iResult < 0) {
			std::string mess("recv failed: " + std::to_string(WSAGetLastError()));
			MessageBoxA(nullptr, mess.c_str(), "Error", 0);
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}
	} while (iResult > 0);
	return 0;
}

int Server::acceptClient()
{
	SOCKET ClientSocket;

	ClientSocket = accept(_listenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		print("accept failed: " + std::to_string(WSAGetLastError()) + "\n");
		closesocket(_listenSocket);
		WSACleanup();
		return 1;
	}
	WSAAsyncSelect(ClientSocket, _hwnd, WM_USER + 2, FD_READ | FD_CLOSE);
	_clientSockets.push_back(ClientSocket);
	sendData("Hello world", ClientSocket);
	//MessageBox(0, L"Hi", L"You are now connected.", 0);
	return 0;
}

int Server::run()
{
	MSG msg = { 0 };

	while (msg.message != WM_QUIT) {
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_USER + 1)
		{
			// acceptConnection
		}
	}
	return (int)msg.wParam;
}