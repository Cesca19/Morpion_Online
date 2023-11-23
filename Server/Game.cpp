#include "Game.h"
#include "ServerCore.h"
#include <fstream>
#include <string>

std::string convertBoard(int** board)
{
	std::string mess;
	for (int i = 0; i < 3; i++) {
		mess += board[i][0] + '0';
		mess += board[i][1] + '0';
		mess += board[i][2] + '0';
	}
	return mess;
}

Game::Game() :_isRunning(false), _core(NULL), _gameMap(NULL),
_currentPlayer("")
{
	_players.push_back("");
	_players.push_back("");
}

Game::~Game()
{
	if (_gameMap != NULL) {
		for (int i = 0; i < 3; i++)
			delete _gameMap[i];
		delete[] _gameMap;
	}
}

void Game::initGameMap()
{
	_gameMap = new int* [3];
	for (int i = 0; i < 3; i++) {
		_gameMap[i] = new int[3];
		_gameMap[i][0] = _gameMap[i][1] = _gameMap[i][2] = 0;
	}
}

void Game::setGameMap(int** map)
{
	_gameMap = map;
}

int** Game::getGameMap()
{
	return _gameMap;
}

std::string Game::getGameInfos()
{
	return _gameInfos;
}

std::string Game::getCurrentPlayer()
{
	return _currentPlayer;
}

void Game::setCurrentPlayer(std::string player)
{
	_currentPlayer = player;
}

void Game::addPlayer(std::string name, int rank)
{
	_players[rank - 1] = name;
	if (_players[0] != "" && _players[1] != "") {
		_isRunning = true;
		_currentPlayer = _players[0];
		sendMessageToPlayers("start#");
	}
}

void Game::addWatcher(std::string name)
{
	std::string winner = "";
	if (_players[0] != "" && _players[1] != "" && _isRunning) {
		sendMessageToPlayer(name, "start#");
		int win = checkWinner();
		if (win)
			winner = (win == 3) ? "T" : _players[win - 1];
		sendMessageToPlayer(name, Protocol::GameProtocol::createGameStateMessage(_gameMap, _turn, winner, _currentPlayer) + "#");
	}
}

void Game::changePlayer()
{
	if (_currentPlayer == _players[0]) {
		_currentPlayer = _players[1];
		return;
	}
	else if (_currentPlayer == _players[1])
		_currentPlayer = _players[0];
}

void Game::setCore(void* core)
{
	_core = core;
}

void Game::sendMessageToPlayers(std::string message)
{
	ServerCore* core = (ServerCore*)_core;
	core->sendMessageToPlayers(message);
}

void Game::sendMessageToPlayer(std::string name, std::string message)
{
	ServerCore* core = (ServerCore*)_core;
	core->sendMessageToPlayer(name, message);
}

void Game::run()
{
	static std::string prevPlayer = "";
	std::string mov;
	std::vector<std::string> mess;
	if (_isRunning && checkWinner() == 0) {

		if (prevPlayer != _currentPlayer) {
			std::string winner = "";

			sendMessageToPlayers(Protocol::GameProtocol::createGameStateMessage(_gameMap, _turn, winner, _currentPlayer) + "#");
			prevPlayer = _currentPlayer;
		}
		// wait for move
		mov = ((ServerCore*)_core)->getPlayerLastMessage();
		//OutputDebugStringA(("Game:: 2  " + mov).c_str());

		nlohmann::json msg;
		if (mov[0] == '{')
		{
			msg = nlohmann::json::parse(mov);
		}
		if (mov != "" && msg["type"] == "MOVE") {
			//parse mov 
			auto msgData = Protocol::GameProtocol::handleMoveMessage(mov);

			//create a string with the player move to be saved and send with SetHistoricMsg()
			std::string histMsg = "";
			histMsg += "Player:" + msgData.name;
			histMsg += "Column:" + std::to_string(msgData.posX);
			histMsg += "Line:" + std::to_string(msgData.posY);
			SetHistoricMsg(histMsg);

			if (msgData.name == _currentPlayer) {
				move(msgData.posX, msgData.posY);
				int win = checkWinner();
				//if win/tie
				if (win != 0) {
					std::string winner = (win == 3) ? "T" : _players[win - 1];
					sendMessageToPlayers(Protocol::GameProtocol::createGameStateMessage(_gameMap, _turn, winner, _currentPlayer) + "#");
					SetHistoricMsg("Winner:" + winner);
					//sendMessageToPlayers("B;" + convertBoard(_gameMap) + "#");
					//sendMessageToPlayers("E;" + ((win == 3) ? "T" : "W:" + _players[win - 1]) + "#");
				}
				changePlayer();
			}
		}
		if (split(mov, "#")[0] == "historic")
		{
			OutputDebugStringA("Game:: 3  Historic \n");
			sendMessageToPlayer(split(mov, "#")[1], Protocol::GameProtocol::createAllMoveMessage("")); //bug, maybe trouble with id cause it works sometime with senMessageToPlayers
		}
	}

}

void Game::move(int x, int y)
{
	if (_currentPlayer == _players[0])
		_gameMap[x][y] = 1;
	else
		_gameMap[x][y] = 2;
}

int Game::checkWinner()
{
	int zero = 0;
	//check diagonals
	if (_gameMap[0][0] == _gameMap[1][1] && _gameMap[1][1] == _gameMap[2][2] && _gameMap[1][1] != 0)
		return _gameMap[0][0];
	if (_gameMap[0][2] == _gameMap[1][1] && _gameMap[1][1] == _gameMap[2][0] && _gameMap[1][1] != 0)
		return _gameMap[1][1];
	//check lines and columns
	for (int j = 0; j < 3; j++)
		if (_gameMap[0][j] == _gameMap[1][j] && _gameMap[1][j] == _gameMap[2][j] && _gameMap[0][j] != 0)
			return _gameMap[0][j];
	for (int i = 0; i < 3; i++)
		if (_gameMap[i][0] == _gameMap[i][1] && _gameMap[i][1] == _gameMap[i][2] && _gameMap[i][0] != 0)
			return _gameMap[i][0];
	//check for tie
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (_gameMap[i][j] == 0)
				zero++;
		}
	}
	if (zero == 0)
		return 3;
	return 0;
}

int Game::isOver()
{
	return 0;
}

void Game::SetHistoricMsg(std::string mess)
{
	std::string HistoricMsg = "";
	HistoricMsg += mess;
	//if last message is the same just ignore it
	if (HistoricMsg == LastHistoricMsg) return;
	std::fstream HistoricFile;
	//close the file in case it was open, it's to avoid bug even if it's not supposed to be open at first.
	HistoricFile.close();
	HistoricFile.open("historic.txt", std::fstream::app);
	if (HistoricFile.is_open()) {
		HistoricFile << HistoricMsg << std::endl;
		LastHistoricMsg = HistoricMsg;
		//_gameInfos will get the message and will be used to be sent to WebServer
		_gameInfos += ("<p><br>" + LastHistoricMsg + "</p>");
		HistoricFile.close();
	}
	else
		OutputDebugString(L"Game:: ERROR : Couldn't open historic.txt");
}

void Game::GetHistoricFile()
{
	std::string historic;
	std::fstream HistoricFile;
	HistoricFile.close();
	HistoricFile.open("historic.txt", std::fstream::in);
	if (HistoricFile.is_open()) {
		while (HistoricFile) {
			historic += HistoricFile.get();
		}
	}
	HistoricFile.close();
}
