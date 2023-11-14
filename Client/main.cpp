#include "pch.h"

// Client Window Procedure
LRESULT CALLBACK ClientWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    int x, y = 0;
    POINT mousePos;
    if (GetCursorPos(&mousePos))
    {
        x = mousePos.x;
        y = mousePos.y;
    }
    //Check the mouse left button is pressed or not
    if (GetAsyncKeyState(VK_LBUTTON))
    {
        MessageBox(hWnd, L"left click pressed", L"UwU", 0);
    }


    switch (message) {
    case WM_CREATE:
    {
        // Set up client (create socket, connect to the server, etc.)
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            MessageBox(hWnd, L"WSAStartup failed", L"ERROR", 0);
            std::cerr << "WSAStartup failed." << std::endl;
            PostQuitMessage(1);
            return -1;
        }


        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET) {
            MessageBox(hWnd, L"Error creating socket", L"ERROR", 0);
            std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
            PostQuitMessage(1);
            return -1;
        }

        PCWSTR ipAddressText = L"127.0.0.1";  // Replace with your desired IP address

        sockaddr_in serverAddress;
        if (InetPton(AF_INET, ipAddressText, &(serverAddress.sin_addr)) == 1) {
            // Conversion successful

            std::cout << "Binary representation: " << serverAddress.sin_addr.S_un.S_addr << std::endl;
        }
        else {
            // Conversion failed
            MessageBox(hWnd, L"InetPton failed. Error code", L"ERROR", 0);

            std::cerr << "InetPton failed. Error code: " << WSAGetLastError() << std::endl;
        }

        if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
            MessageBox(hWnd, L"Error connecting to server" , L"ERROR", 0);
            std::cerr << "Error connecting to server" << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            PostQuitMessage(1);
            return -1;
        }

        // Start receiving messages in a separate thread or use asynchronous I/O
            // Clean up resources
        closesocket(clientSocket);
        WSACleanup();
        PostQuitMessage(0);

        return 0;

    }
    break;

    // Add other message handling cases as needed

    case WM_DESTROY:
        // Clean up resources
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
    // Register client window class
    WNDCLASS clientClass = {};
    clientClass.lpfnWndProc = ClientWndProc;
    clientClass.hInstance = GetModuleHandle(nullptr);
    clientClass.lpszClassName = L"ClientClass";
    RegisterClass(&clientClass);

    // Create client window
    HWND clientWindow = CreateWindow(L"ClientClass", L"Client Window", 0, 0, 0, 0, 0, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        return 0;
    }
}