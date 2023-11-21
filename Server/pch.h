#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <memory>
#include <vector>
#include <Windows.h>
#include <unordered_map>
#pragma comment(lib, "Core.lib")
#include "../Core/GameProtocol.h"
#include <string> 

#pragma comment(lib, "Ws2_32.lib")

#ifndef MY_HEADER_FILE_
#define MY_HEADER_FILE_

#define ACCEPT_CLIENT (WM_USER + 1)
#define READ_MESSAGE (WM_USER + 2)

#define GAME_SERVER_ID (WM_USER + 3)
#define WEB_SERVER_ID (WM_USER + 4)

#define NEW_GAME_CLIENT (WM_USER + 5)
#define NEW_MESSAGE (WM_USER + 6)
#define SEND_MESSAGE_TO_PLAYERS (WM_USER + 7)
#define SEND_MESSAGE_TO_PLAYER (WM_USER + 8)

#define NEW_WEB_CLIENT (WM_USER + 9)
#define NEW_WEB_MESSAGE (WM_USER + 10)
#define SEND_GAME_MAP (WM_USER + 11)


struct Data_t {
	std::string content;
};

struct GameMap_t {
	int** map;
};

#endif
