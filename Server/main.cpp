#include <iostream>
#include <vector>

#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

std::vector<SOCKET> clients;

// Server Window Procedure
LRESULT CALLBACK ServerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
    {
        // Set up server (create socket, bind, listen, etc.)
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed." << std::endl;
            PostQuitMessage(1);
            return -1;
        }

        SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == INVALID_SOCKET) {
            std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
            PostQuitMessage(1);
            return -1;
        }

        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(8080);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

        if (bind(serverSocket, reinterpret_cast<SOCKADDR*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
            std::cerr << "Error binding socket: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            PostQuitMessage(1);
            return -1;
        }

        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Error listening on socket: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            PostQuitMessage(1);
            return -1;
        }

        clients.clear();

        // Start accepting clients in a separate thread or use asynchronous I/
    }
    break;

    case WM_USER + 1:
        // Handle client connection request
        // lParam contains the socket handle of the connected client
        std::cout << "Client connected!" << std::endl;
        break;

        // Add other message handling cases as needed

    case WM_DESTROY:
        // Clean up resources
        for (SOCKET clientSocket : clients) {
            closesocket(clientSocket);
        }
        WSACleanup();
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int  WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd
) {
    // Register server window class
    WNDCLASS serverClass = {};
    serverClass.lpfnWndProc = ServerWndProc;
    serverClass.hInstance = GetModuleHandle(nullptr);
    serverClass.lpszClassName = L"ServerClass";
    RegisterClass(&serverClass);

    // Create server window
    HWND serverWindow = CreateWindow(L"ServerClass", L"Server Window", 0, 0, 0, 0, 0, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
