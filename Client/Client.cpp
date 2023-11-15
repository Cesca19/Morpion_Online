#include "Client.h"

Client::Client()
{
    InitClientSocket();
}

Client::~Client()
{
    
}

void Client::InitClientSocket()
{
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
    }

    // Create a socket
    addrinfo* result = NULL, hints = {};
    
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(serverIpAddress, serverPort, &hints, &result);
    if (iResult != 0) {
        std::cerr << "getaddrinfo failed: " << iResult << std::endl;
        WSACleanup();
    }

    serverSocket = INVALID_SOCKET;
    serverSocket = socket(result->ai_family, result->ai_socktype,
            result->ai_protocol);

    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
    }

    iResult = connect(serverSocket, result->ai_addr, (int) result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
    }

    freeaddrinfo(result);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Unable to connect to server!" << std::endl;
        WSACleanup();
    } else
    {
        std::cout << "Connected to the server!" << std::endl;
    }
}

void Client::SendMove(int x, int y)
{
    Position pos(x, y);
    char buffer[sizeof(Position)];
    memcpy(buffer, &pos, sizeof(Position));
    int bytesSent = send(serverSocket, buffer, sizeof(Position), 0);
    if(bytesSent == SOCKET_ERROR) {
        std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
    }
}

void Client::ReceiveState()
{
    do {
        ZeroMemory(recvbuf, DEFAULT_BUFLEN);
        iResult = recv(serverSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            std::cout << "Message received in client: " << recvbuf << std::endl;
        }
        else if (iResult == 0)
            std::cout << "Connection closed" << std::endl;
        else
            std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
    } while (iResult > 0);
}

void Client::CloseAndCleanUp()
{
    closesocket(serverSocket);
    WSACleanup();
    PostQuitMessage(0);
}


