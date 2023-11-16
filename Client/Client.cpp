
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Client.h"
#include <time.h>

Client* Client::_client = nullptr;

void print(std::string mess)
{
	std::wstring res(mess.begin(), mess.end());
	OutputDebugStringW(res.c_str());
}

Client* Client::getClient()
{
	return _client;
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return (Client::getClient()->wndProc(hwnd, msg, wParam, lParam));
}

Client::Client(HINSTANCE hInstance, std::string address, std::string port) :
	_hInstance(hInstance), _ipAddress(address), _port(port)
{
	_client = this;
}

Client::~Client()
{
	closesocket(_connectSocket);
	WSACleanup();
}

LRESULT Client::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_USER + 2:
		if (LOWORD(lParam) == FD_READ)
			readData(wParam, lParam);
		else if (LOWORD(lParam) == FD_CLOSE)
		{ }
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
	ShowWindow(_hwnd, SW_SHOW);
	UpdateWindow(_hwnd);
	return 0;
}

int  Client::initWinsock()
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
	WSAAsyncSelect(_connectSocket, _hwnd, WM_USER + 2, FD_READ | FD_CLOSE);
	return 0;
}

int Client::init()
{
	srand(time(0));
	if (initWindow())
		return 1;
	if (initClient())
		return 1;
	_name = std::to_string(rand() % 100);
	sendData("name:" + _name);
	return 0;
}

int Client::sendData(std::string data)
{
	int iResult;

	iResult = send(_connectSocket, data.c_str(), data.size(), 0);
	if (iResult == SOCKET_ERROR) {
		OutputDebugStringA(std::string("send failed: " + std::to_string(WSAGetLastError())).c_str());
		closesocket(_connectSocket);
		WSACleanup();
		return 1;
	}
	return 0;
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
		OutputDebugStringA(std::string("Message received in client " + _name + ":" + receivedMessage + "\n").c_str());
	} else if (iResult < 0)
		OutputDebugStringA(std::string("client recv failed: " + std::to_string(WSAGetLastError()) + "\n").c_str());
	
	if (receivedMessage.substr(0, 5) == std::string("Turn:") && receivedMessage.substr(5, receivedMessage.size()) == _name) {
		sendData("move:" + std::to_string(rand() % 3) + ":" + std::to_string(rand() % 3));
	}
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
	return (int)msg.wParam;
}