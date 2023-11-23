#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Client.h"
#include "ClientCore.h"

Client* Client::_client = nullptr;

DWORD WINAPI Client::MyThreadFunction(LPVOID lpParam)
{
	Client_Conf_t* clientConf = (Client_Conf_t *)lpParam;
	Client* client = new Client(GetModuleHandle(NULL), clientConf->ip, clientConf->port);

	client->setCore(clientConf->core);
	client->init();
	client->run();
	delete client;
	delete clientConf;
	OutputDebugStringA("Thread Client closing ...\n");
	return 0;
}

Client* Client::getClient()
{
	return _client;
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return (Client::getClient()->clientWndProc(hwnd, msg, wParam, lParam));
}

Client::Client(HINSTANCE hInstance, std::string address, std::string port) :
	_hInstance(hInstance), _ipAddress(address), _port(port)
{
	_client = this;
}

Client::~Client()
{
}

void Client::serverClosing()
{
	PostMessage(_coreHwnd, DISCONNECT_SERVER, 0, 0);
}

LRESULT Client::clientWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case SEND_MESSAGE_TO_SERVER: {
		sendData(wParam, lParam);
		break;
	} case NEW_SERVER_MESSAGE: {
		if (LOWORD(lParam) == FD_READ)
			readData(wParam, lParam);
		else if (LOWORD(lParam) == FD_CLOSE)
			serverClosing();
		break;
	} case DISCONNECT_CLIENT: {
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

int Client::initWindow()
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
	wcex.lpszClassName = L"Client";
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL, L"Call to RegisterClassEx failed!",
			L"Windows Desktop Guided Tour", NULL);
		return 1;
	}
	_hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"Client",
		L"Client app", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, 100, 100, NULL, NULL, _hInstance, NULL);
	if (!_hwnd) {
		MessageBox(NULL, L"Call to CreateWindow failed!",
			L"Windows Desktop Guided Tour", NULL);
		return 1;
	}
	UpdateWindow(_hwnd);
	return 0;
}

int  Client::initWinsock()
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

int Client::createSocket()
{
	addrinfo* result = NULL, hints = {};
	int iResult;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	iResult = getaddrinfo(_ipAddress.c_str(), _port.c_str(), &hints, &result);
	if (iResult != 0) {
		OutputDebugStringA(("getaddrinfo failed: %d\n" + std::to_string(iResult) + " \n").c_str());
		WSACleanup();
		return 1;
	}
	_connectSocket = INVALID_SOCKET;
	_connectSocket = socket(result->ai_family, result->ai_socktype,
		result->ai_protocol);
	if (_connectSocket == INVALID_SOCKET) {
		OutputDebugStringA(("Error at socket(): %ld\n" + std::to_string(WSAGetLastError()) + " \n").c_str());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	iResult = connect(_connectSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(_connectSocket);
		_connectSocket = INVALID_SOCKET;
	}
	freeaddrinfo(result);
	if (_connectSocket == INVALID_SOCKET) {
		OutputDebugStringA("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}
	return 0;
}

int Client::initClient()
{
	if (initWinsock())
		return 1;
	if (createSocket())
		return 1;
	WSAAsyncSelect(_connectSocket, _hwnd, NEW_SERVER_MESSAGE, FD_READ | FD_CLOSE);
	return 0;
}

int Client::init()
{
	if (initWindow())
		return 1;
	PostMessage(_coreHwnd, GAME_CLIENT_ID, (WPARAM)_hwnd, 0);
	if (initClient())
		return 1;
	return 0;
}

int Client::sendData(WPARAM wParam, LPARAM lParam)
{
	Data_t* mess = (Data_t*)wParam;
	std::string data(mess->content);
	int iResult;

	iResult = send(_connectSocket, data.c_str(), (int)data.size(), 0);
	if (iResult == SOCKET_ERROR) {
		OutputDebugStringA(std::string("send failed: " + std::to_string(WSAGetLastError())).c_str());
		closesocket(_connectSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}

void Client::setCore(HWND coreHwnd)
{
	_coreHwnd = coreHwnd;
}

std::string Client::readData(WPARAM wParam, LPARAM lParam)
{
	int iResult;
	std::string receivedMessage;
	char readMessage[DEFAULT_BUFLEN];


	ZeroMemory(readMessage, DEFAULT_BUFLEN);
	iResult = recv(_connectSocket, readMessage, DEFAULT_BUFLEN, 0);
	if (iResult > 0) {
		receivedMessage += std::string(readMessage);
		Data_t* data = new Data_t;
		data->content = receivedMessage;
		PostMessage(_coreHwnd, NEW_MESSAGE_FROM_SERVER, (WPARAM)data, 0);
	} else if (iResult < 0)
		OutputDebugStringA(std::string("client recv failed: " + std::to_string(WSAGetLastError()) + "\n").c_str());
	return receivedMessage;
}

int Client::run()
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

int Client::close()
{
	closesocket(_connectSocket);
	WSACleanup();
	PostMessage(_hwnd, WM_CLOSE, 0, 0);
	OutputDebugStringA("Client closing ...\n");
	return 0;
}