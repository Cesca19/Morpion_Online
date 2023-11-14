#include "Server.h"

/*
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>

#include <cstdlib>
#include <signal.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "6666"
#define DEFAULT_BUFLEN 512*/


/*
void signal_callback_handler(int signum) {
	printf("sig quit\n");
	exit(signum);
}**/
/*
void print(std::string mess)
{
	std::wstring res(mess.begin(), mess.end());
	OutputDebugStringW(res.c_str());
}

int initWinsock()
{
	//	###		INITIALIZE WINSOCK		###
	// initialize the use of the Windows Sockets DLL
	//  before making other Winsock functions calls
	WSADATA wsaData;
	int iResult;

	//	The WSAStartup function is called to initiate use of WS2_32.dll.
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}
	return 0;
}

int createSocket(SOCKET* listenSocket)
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
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	*listenSocket = INVALID_SOCKET;
	// Create a SOCKET for the server to listen for client connections
	*listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (*listenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	//	###		BINDING THE SOCKET		####
	// For a server to accept client connections, it must be bound to a 
	// network address within the system.
	// Setup the TCP listening socket
	iResult = bind(*listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(*listenSocket);
		WSACleanup();
		return 1;
	}
	freeaddrinfo(result);
	return 0;
}

HINSTANCE hInst;
static TCHAR szWindowClass[] = L"DesktopApp";
static TCHAR szTitle[] = L"Windows Desktop Guided Tour Application";

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SOCKET ListenSocket;
	switch (message) {
	case WM_LBUTTONDOWN:
		int iResult, iSendResult;
		//signal(SIGINT, signal_callback_handler);

		if (initWinsock())
			return 1;
		
		if (createSocket(&ListenSocket))
			return 1;
		MessageBox(0, L"done binding", L"Hi", 0);
		WSAAsyncSelect(ListenSocket, hWnd, WM_USER + 1, FD_ACCEPT | FD_CLOSE);

		//	###		LISTEN ON A SOCKET		###
		if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
			print("Listen failed with error: " + std::to_string(WSAGetLastError()) + "\n");
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
		break;
	case WM_USER + 1:
		SOCKET ClientSocket;
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			print("accept failed: " + std::to_string(WSAGetLastError()) + "\n");
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
		WSAAsyncSelect(ClientSocket, hWnd, WM_USER + 1, FD_ACCEPT | FD_CLOSE);
		MessageBox(0, L"Hi", L"You are now connected.", 0);
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}*/

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	Server server(hInstance, "6666");

	if (server.init())
		return 1;
	server.run();
}
	/*WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			L"Call to RegisterClassEx failed!",
			L"Windows Desktop Guided Tour",
			NULL);

		return 1;
	}

	// Store instance handle in our global variable
	hInst = hInstance;

	HWND hWnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		100, 100,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd) {
		MessageBox(NULL,
			L"Call to CreateWindow failed!",
			L"Windows Desktop Guided Tour",
			NULL);

		return 1;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Main message loop:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
	/*int iResult, iSendResult;
	//signal(SIGINT, signal_callback_handler);

	if (initWinsock())
		return 1;
	SOCKET ListenSocket = INVALID_SOCKET;
	if (createSocket(&ListenSocket))
		return 1;
	//	###		LISTEN ON A SOCKET		###
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	//	###		 ACCEPT A CLIENT SOCKET		###
	SOCKET ClientSocket = INVALID_SOCKET;
	 
	while (1) {
		// get the new connected client address
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
		//closesocket(ListenSocket);

		//	###		RECEIVING AND SENDING DATA ON A SERVER		###
		char recvbuf[DEFAULT_BUFLEN];
		int recvbuflen = DEFAULT_BUFLEN;
		const char* mess = "Hello World\n";
		do {
			iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0) {
				printf("Message recived: ");
				printMessage(recvbuf, iResult);
				// Echo the buffer back to the sender
				iSendResult = send(ClientSocket, mess, strlen(mess), 0);
				if (iSendResult == SOCKET_ERROR) {
					printf("send failed: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				}
				printf("Bytes sent: %d\nMessage Sent: ", iSendResult);
				printMessage((char *)mess, iSendResult);
			}
			else if (iResult == 0)
				printf("Connection closing...\n");
			else {
				printf("recv failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
		} while (iResult > 0);

		//	###		DISCONNECT THE SERVER		###
		// shutdown the send half of the connection since no more data will be sent
		iResult = shutdown(ClientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
	}
	// cleanup
	closesocket(ListenSocket);
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}*/