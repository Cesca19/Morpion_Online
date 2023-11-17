#pragma once
#ifndef GAMEPROTOCOL_H
#define GAMEPROTOCOL_H
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
			GameStateMessage() : board(NULL), currentPlayer(NULL), turnNumber(NULL), winner(NULL) {}

		};

		struct MoveMessage
		{
			int playerId;
			int posX;
			int posY;

			 MoveMessage() : playerId(NULL), posX(NULL), posY(NULL) {}
		};

		struct NewClientMessage
		{
			int id = NULL;
			std::string name = NULL;
			NewClientMessage() : id(NULL), name(NULL) {}

		};

		struct ClientIDMessage
		{
			int id = NULL;
			int playerState = NULL;
			ClientIDMessage() : id(NULL), playerState(NULL) {}

		};

	public:
		

		GameProtocol();
		~GameProtocol();

		static GameProtocol::GameStateMessage handleGameStateMessage(std::string);
		static GameProtocol::MoveMessage handleMoveMessage(std::string);
		static GameProtocol::NewClientMessage handleNewClientMessage(std::string);
		static GameProtocol::ClientIDMessage handleClientIdMessage(std::string);


	public:

		static GameProtocol::GameStateMessage processGameStateMessage(const nlohmann::json& message); //client receives the gamestete from the server
		static GameProtocol::MoveMessage processMoveMessage(const nlohmann::json& message); //server receives the move from client
		static GameProtocol::NewClientMessage processNewClientMessage(const nlohmann::json& message); // client receives the new clien that logged in and add it to the vector 
		static GameProtocol::ClientIDMessage processClientIdMessage(const nlohmann::json& message);  //client receives his own id from the server

		static std::string createGameStateMessage(int** gameBoard, int turnNumber, int winner, int currentPlayer); //server creates a GameState message using his own value 
		static std::string createMoveMessage(int id, int posX, int poxY); //player sends the move to the server
		static std::string createNewClientMessage(int id, std::string name);// server create a message for clients to introduce new client
		static std::string createClientIdMessage(int playerId, int playerState); //server creates a message to send a clien'ts ID to this client

	};
}

#endif // GAMEPROTOCOL_H