#include "GameProtocol.h"

namespace Protocol
{
	GameProtocol::GameProtocol(){}
	GameProtocol::~GameProtocol(){}


	GameProtocol::GameStateMessage GameProtocol::handleGameStateMessage(std::string mess)
	{
		GameStateMessage msg{};
		nlohmann::json message = nlohmann::json::parse(mess);
		msg = message["type"].get<std::string>() == "GAME" ? processGameStateMessage(message) : msg;
		return msg;
	}	
	GameProtocol::MoveMessage GameProtocol::handleMoveMessage(std::string mess)
	{
		MoveMessage msg;
		nlohmann::json message = nlohmann::json::parse(mess);
		msg = message["type"].get<std::string>() == "MOVE" ? processMoveMessage(message) : msg;
		return msg;
	}
	GameProtocol::NewClientMessage GameProtocol::handleNewClientMessage(std::string mess)
	{
		NewClientMessage msg{};
		nlohmann::json message = nlohmann::json::parse(mess);

		msg = message["type"].get<std::string>() == "NEW_CLIENT" ? processNewClientMessage(message) : msg;
		return msg;
	}
	GameProtocol::ClientIDMessage GameProtocol::handleClientIdMessage(std::string mess)
	{
		ClientIDMessage msg{};
		nlohmann::json message = nlohmann::json::parse(mess);
		msg = message["type"].get<std::string>() == "CLIENT_ID" ? processClientIdMessage(message) : msg;
		return msg;
	}

	GameProtocol::GameStateMessage GameProtocol::processGameStateMessage(const nlohmann::json& message)
	{
		GameStateMessage msg;

		auto board = message["game"]["board"].get<std::vector<std::vector<int>>>();
		int rows = board.size();
		int col = board[0].size();
		msg.board = new int*[rows];

		for (int i = 0; i < rows; ++i) {
			msg.board[i] = new int[col];
			std::copy(board[i].begin(), board[i].end(), msg.board[i]);
		}
		
		msg.currentPlayer = message["game"]["currentPlayer"].get<int>();
		msg.turnNumber = message["game"]["turnNumber"].get<int>();
		msg.winner = message["game"]["winner"].get<int>();

		return msg;
	}	
	GameProtocol::MoveMessage GameProtocol::processMoveMessage(const nlohmann::json& message)
	{
		MoveMessage msg;

		msg.playerId = message["data"]["playerID"].get<int>();
		msg.posX = message["data"]["posX"].get<int>();
		msg.posY = message["data"]["posY"].get<int>();

		return msg;
	}

	GameProtocol::NewClientMessage GameProtocol::processNewClientMessage(const nlohmann::json& message)
	{
		NewClientMessage msg;

		msg.id = message["data"]["id"].get<int>();
		msg.name = message["data"]["name"].get<std::string>();

		return msg;
	}
	GameProtocol::ClientIDMessage GameProtocol::processClientIdMessage(const nlohmann::json& message)
	{
		ClientIDMessage msg;

		msg.id = message["data"]["id"].get<int>();
		msg.playerState = message["data"]["playerState"].get<int>();

		return msg;
	}

	std::string GameProtocol::createGameStateMessage(int** gameBoard, int turnNumber, int winner, int currentPlayer)
	{

		std::vector<std::vector<int>> boardVector;
		int rows = std::extent<decltype(gameBoard), 0>::value;
		int cols = std::extent<decltype(gameBoard), 1>::value;
		// Copy the contents of the array to the vector
		for (int i = 0; i < rows; ++i) {
			std::vector<int> row;
			for (int j = 0; j < cols; ++j) {
				row.push_back(gameBoard[i][j]);
			}
			boardVector.push_back(row);
		}
		//boardVector just for the message 

		nlohmann::json message;

		message = {
		 {"type", "GAME"},
		 {"game", {
			 {"board", boardVector},
			 {"currentPlayer", currentPlayer},
			 {"turnNumber", turnNumber},
			 {"winner", winner}
		 }}
		};

		return message.dump();
	}
	std::string GameProtocol::createMoveMessage(int id, int posX, int poxY)
	{
		nlohmann::json message;

		message = {
		 {"type", "MOVE"},
		 {"data", {
			 {"playerID", id},
			 {"posX", posX},
			 {"posY", poxY}
		 }}
		};

		return message.dump();
	}
	std::string GameProtocol::createNewClientMessage(int id, std::string name)
	{
		nlohmann::json message;

		message = {
		 {"type", "NEW_CLIENT"},
		 {"data", {
			 {"id", id},
			 {"name", name}
		 }}
		};

		return message.dump();
	}
	std::string GameProtocol::createClientIdMessage(int playerId, int playerState)
	{
		nlohmann::json message;
		message = {
		 {"type", "CLIENT_ID"},
		 {"data", {
			 {"playerID", playerId},
			 {"playerState", playerState}
		 }}
		};
		return message.dump();
	}

}