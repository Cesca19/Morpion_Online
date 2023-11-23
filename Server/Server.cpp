#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Server.h"
#include "ServerCore.h"

Server* Server::_server = nullptr;

DWORD WINAPI Server::MyThreadFunction(LPVOID lpParam)
{
	Server_Conf_t* gameServerConf = (Server_Conf_t*)lpParam;
	Server* server = new Server(GetModuleHandle(NULL), gameServerConf->port);

	server->setCore(gameServerConf->core);
	server->init();
	server->run();
	delete gameServerConf;
	delete server;
	OutputDebugStringA("Game server thread closing ...\n");
	return 0;
}

Server* Server::getServer()
{
	return _server;
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return (Server::getServer()->wndProc(hwnd, msg, wParam, lParam));
}

Server::Server(HINSTANCE hInstance, std::string port) : _hInstance(hInstance),
_port(port), _lastPlayerMessage(""), _id(1), _listenSocket(INVALID_SOCKET)
{
	_server = this;
}

Server::~Server()
{
}

void Server::close()
{
	closesocket(_listenSocket);
	WSACleanup();
	PostMessage(_hwnd, WM_CLOSE, 0, 0);
}

LRESULT Server::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case SEND_MESSAGE_TO_PLAYERS: 
	{
		sendMessageToPlayers(wParam, lParam);
		break;
	} case SEND_MESSAGE_TO_PLAYER: 
	{
		sendMessageToPlayer(wParam, lParam);
		break;
	} case ACCEPT_CLIENT: 
	{
		if (LOWORD(lParam) == FD_ACCEPT)
			acceptClient();
		else if (LOWORD(lParam) == FD_CLOSE)
		{}
		break;
	} case READ_MESSAGE: 
	{
		if (LOWORD(lParam) == FD_READ)
			readData(wParam, lParam);
		else if (LOWORD(lParam) == FD_CLOSE)
		{
			OutputDebugStringA("Server: client closing\n");
		}
		break;
	} case DISCONNECT_GAME_SERVER : 
	{
		close();
		break;
	} case WM_DESTROY:
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
	UpdateWindow(_hwnd);
	return 0;
}

int  Server::initWinsock()
{
	WSADATA wsaData;
	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		std::string mess("WSAStartup failed: " + std::to_string(iResult));
		OutputDebugStringA(mess.c_str());
		return 1;
	}
	return 0;
}

int Server::createSocket()
{
	int iResult;
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
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

int Server::initServer()
{
	if (initWinsock())
		return 1;
	if (createSocket())
		return 1;
	WSAAsyncSelect(_listenSocket, _hwnd, ACCEPT_CLIENT, FD_ACCEPT | FD_CLOSE | FD_READ);
	if (listen(_listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::string mess("Listen failed with error: " + std::to_string(WSAGetLastError()));
		OutputDebugStringA(mess.c_str());
		closesocket(_listenSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}

int Server::init()
{
	if (initWindow())
		return 1;
	PostMessage(_coreHwnd, GAME_SERVER_ID, (WPARAM)_hwnd, 0);
	if(initServer())
		return 1;
	return 0;
}

int Server::sendData(std::string data, SOCKET clientSocket)
{
	int iSendResult = send(clientSocket, data.c_str(), (int)data.size(), 0);
	if (iSendResult == SOCKET_ERROR) {
		OutputDebugStringA( std::string ("send failed: " + std::to_string( WSAGetLastError())).c_str() );
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
	Data_t* data = new Data_t;
	
	ZeroMemory(recvbuf, DEFAULT_BUFLEN);
	iResult = recv(clientSocket, recvbuf, DEFAULT_BUFLEN, 0);
	data->content = recvbuf;
	PostMessage(_coreHwnd, NEW_MESSAGE, (WPARAM)data, _clientsSocket[clientSocket]->getId());
	if (iResult < 0) {
		std::string mess("recv failed: " + std::to_string(WSAGetLastError()));
		OutputDebugStringA(mess.c_str());
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}
	OutputDebugStringA("read data succes \n");
	return 0;
}

int Server::closeClient(WPARAM wParam, LPARAM lParam)
{
	/*SOCKET clientSocket = (SOCKET)wParam;
	closesocket(clientSocket);*/
	return 0;
}

int Server::acceptClient()
{
	SOCKET ClientSocket;

	ClientSocket = accept(_listenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		OutputDebugStringA(("accept failed: " + std::to_string(WSAGetLastError()) + "\n").c_str());
		closesocket(_listenSocket);
		WSACleanup();
		return 1;
	}
	WSAAsyncSelect(ClientSocket, _hwnd, READ_MESSAGE, FD_READ | FD_CLOSE);
	PostMessage(_coreHwnd, NEW_GAME_CLIENT, ClientSocket, _id);
	std::shared_ptr<GameClient> client(new GameClient(ClientSocket, _id));
	_clientsVect.push_back(client);
	_clientsMap[_id] = client;
	_clientsSocket[ClientSocket] = client;
	sendData(Protocol::GameProtocol::createNewClientMessage(_id, "HUHU"), ClientSocket);
	_id++;
	return 0;
}

int Server::run()
{
	MSG msg = { 0 };

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}

void Server::sendMessageToPlayers(WPARAM wParam, LPARAM lParam)
{
	Data_t* data = (Data_t*)wParam;
	std::string message = data->content;
	for (int i = 0; i < _clientsVect.size(); i++)
		_clientsVect[i]->sendMessage(message);
	delete data;
}

void Server::sendMessageToPlayer(WPARAM wParam, LPARAM lParam)
{
	Data_t* data = (Data_t*)wParam;
	std::string message = data->content;
	int id = (int)lParam;

	_clientsMap[id]->sendMessage(message);
	delete data;
}

void Server::setCore(HWND coreHwnd)
{
	_coreHwnd = coreHwnd;
}

