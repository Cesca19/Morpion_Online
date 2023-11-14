#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "6666"
#define DEFAULT_BUFLEN 512

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
		print("WSAStartup failed: " + std::to_string(iResult) + " \n");
		return 1;
	}
	return 0;
}

int createConnectSocket(SOCKET* mysocket, const char* address, const char* port)
{
	addrinfo* result = NULL, hints = {};
	int iResult;

	// Socket creation
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	// Resolve the server address and port
	iResult = getaddrinfo(address, port, &hints, &result);
	if (iResult != 0) {
		print("getaddrinfo failed: %d\n" + std::to_string(iResult) + " \n");
		WSACleanup();
		return 1;
	}

	// init the socket
	*mysocket = INVALID_SOCKET;


	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	// Create a SOCKET for connecting to server
	*mysocket = socket(result->ai_family, result->ai_socktype,
		result->ai_protocol);

	if (*mysocket == INVALID_SOCKET) {
		print("Error at socket(): %ld\n" + std::to_string(WSAGetLastError()) + " \n");
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = connect(*mysocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(*mysocket);
		*mysocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);
	if (*mysocket == INVALID_SOCKET) {
		print("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}
	return 0;
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	SOCKET mySocket = INVALID_SOCKET;
	int iResult;

	if (initWinsock())
		return 1;

	if (createConnectSocket(&mySocket, "127.0.0.1", "6666"))
		return 1;

	// send and receive data

	//	Sending and Receiving Data on the Client
	int recvbuflen = DEFAULT_BUFLEN;
	const char* sendbuf = "GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: close\r\n\r\n";
	char recvbuf[DEFAULT_BUFLEN];
	// Send a message
	iResult = send(mySocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(mySocket);
		WSACleanup();
		return 1;
	}

	// shutdown the connection for sending
	// the client can still use the ConnectSocket for receiving data
	iResult = shutdown(mySocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		print("shutdown failed: " + std::to_string(WSAGetLastError()) + "\n");
		closesocket(mySocket);
		WSACleanup();
		return 1;
	}

	// Receive message date until the server closes the connection
	do {
		iResult = recv(mySocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			print("Message received: ");
			print(std::string(recvbuf) + " \n");
		}
		else if (iResult == 0)
			print("Connection closed\n");
		else
			print("recv failed: " + std::to_string(WSAGetLastError()) + "\n");
	} while (iResult > 0);

}