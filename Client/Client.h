#pragma once

#include "pch.h"

#define DEFAULT_BUFLEN 512

class Client
{
public:
    Client();
    ~Client();

    void InitClientSocket();
    void SendMove(int x, int y);
    void ReceiveState();
    void CloseAndCleanUp();

    SOCKET GetServerSocket() { return serverSocket; }

protected:
    struct Position
    {
        int x, y;
        Position(int x, int y) : x(x), y(y) {}
    };
    
    WSADATA wsaData;
    int iResult;

    struct sockaddr_in server;
    PCSTR serverIpAddress = "127.0.0.1"; 
    PCSTR serverPort = "6666";
    SOCKET serverSocket;
    int recvbuflen = DEFAULT_BUFLEN;
    char recvbuf[DEFAULT_BUFLEN];

};
