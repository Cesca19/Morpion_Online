#pragma once
#include "GameProtocol.h"
#include <windows.data.json.h>
#include <unordered_map>
#include "../nlohmann/json.hpp"


namespace Protocol
{
	static class GameProtocol
	{
	public:

		enum class MessageType {
			GAME_STATE,
			MOVE,
			NEW_CLIENT,
		};
		
		struct GameStateMessage
		{
			int** board = NULL;
			int currentPlayer = NULL;
			int turnNumber = NULL;
			int winner = NULL;
		};

		struct MoveMessage
		{
			int id = NULL;
			int posX = NULL;
			int posY = NULL;
		};

		struct NewClientMessage
		{
			int id = NULL;
			std::string name = NULL;
		};

		struct ClientIDMessage
		{
			int id = NULL;
			int playerState = NULL;
		};

	public:
		

		GameProtocol();
		~GameProtocol();

		GameStateMessage handleGameStateMessage(const nlohmann::json& message); 
		MoveMessage handleMoveMessage(const nlohmann::json& message);
		NewClientMessage handleNewClientMessage(const nlohmann::json& message);
		ClientIDMessage handleClientIdMessage(const nlohmann::json& message);


	public:

		static GameStateMessage processGameStateMessage(const nlohmann::json& message); //client receives the gamestete from the server
		static MoveMessage processMoveMessage(const nlohmann::json& message); //server receives the move from client
		static NewClientMessage processNewClientMessage(const nlohmann::json& message); // client receives the new clien that logged in and add it to the vector 
		static ClientIDMessage processClientIdMessage(const nlohmann::json& message);  //client receives his own id from the server

		std::string createGameStateMessage(int** gameBoard, int turnNumber, int winner, int currentPlayer); //server creates a GameState message using his own value 
		std::string createMoveMessage(int id, int posX, int poxY); //player sends the move to the server
		std::string createNewClientMessage(int id, std::string name);// server create a message for clients to introduce new client
		std::string createClientIdMessage(int playerId, int playerState); //server creates a message to send a clien'ts ID to this client

	};
}