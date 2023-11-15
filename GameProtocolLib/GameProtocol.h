#pragma once
#include "GameProtocol.h"
#include <windows.data.json.h>
#include <unordered_map>
#include "../nlohmann/json.hpp"

class GameProtocol
{
private:
public:
	GameProtocol();
	~GameProtocol();

	void handleMessages(const std::string& message, SOCKET client);
	nlohmann::json receiveJsonMessage(SOCKET socket);
	void sendJsonMessage(SOCKET socket, const nlohmann::json& message);
		 
};