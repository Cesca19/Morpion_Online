#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#include <string>
#include <memory>
#include <vector>
#include <iostream>

#include "Utils.h"
#include "../Core/GameProtocol.h"
#include "../nlohmann/json.hpp"

#pragma comment(lib, "Core.lib")
#pragma comment(lib, "Ws2_32.lib")

#ifndef MY_PCH_FILE_
#define MY_PCH_FILE_

#define NEW_SERVER_MESSAGE (WM_USER + 1)

#define GAME_CLIENT_ID (WM_USER + 2)
#define SEND_MESSAGE_TO_SERVER (WM_USER + 3)
#define NEW_MESSAGE_FROM_SERVER (WM_USER + 4)
#define DISCONNECT_CLIENT (WM_USER + 5)

#define DISCONNECT_SERVER (WM_USER + 6)

struct Client_Conf_t {
	HWND core;
	std::string port;
	std::string ip;
};

struct Data_t {
	std::string content;
};

#endif
