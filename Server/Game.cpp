#include "Game.h"
#include "ServerCore.h"


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

Game::Game() : _gameMap(NULL), _currentPlayer(""), _isRunning(false)
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
	if (_players[0] != "" && _players[1] != "" && _isRunning) {
		sendMessageToPlayer(name, "start#");
		int win = checkWinner();
		std::string winner = (win == 3) ? "T" : _players[win - 1];
		sendMessageToPlayers(Protocol::GameProtocol::createGameStateMessage(_gameMap, _turn, winner, _currentPlayer) + "#");
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

std::vector<std::string> split(std::string message, std::string delimiter)
{
	std::vector<std::string> mess;
	std::string str = message;
	size_t pos = 0;
	std::string token;

	while ((pos = str.find(delimiter)) != std::string::npos) {
		token = str.substr(0, pos);
		mess.push_back(token);
		str.erase(0, pos + delimiter.length());
	}
	mess.push_back(str);
	return mess;
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
		// attendre son movement
		mov = ((ServerCore*)_core)->getPlayerLastMessage();
		nlohmann::json msg;
		if (mov[0] == '{')
		{
			msg = nlohmann::json::parse(mov);
		}
		if (mov != "" && msg["type"] == "MOVE") {
			auto msgData = Protocol::GameProtocol::handleMoveMessage(mov);
			if (msgData.name == _currentPlayer) {
				move(msgData.posX, msgData.posY);
				int win = checkWinner();
				if (win != 0) {
					std::string winner = (win == 3) ? "T" : _players[win - 1];
					sendMessageToPlayers(Protocol::GameProtocol::createGameStateMessage(_gameMap, _turn, winner, _currentPlayer) + "#");
					//sendMessageToPlayers("B;" + convertBoard(_gameMap) + "#");
					//sendMessageToPlayers("E;" + ((win == 3) ? "T" : "W:" + _players[win - 1]) + "#");
				}
				changePlayer();
			}
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
	if (_gameMap[0][0] == _gameMap[1][1] && _gameMap[1][1] == _gameMap[2][2] && _gameMap[1][1] != 0)
		return _gameMap[0][0];
	if (_gameMap[0][2] == _gameMap[1][1] && _gameMap[1][1] == _gameMap[2][0] && _gameMap[1][1] != 0)
		return _gameMap[1][1];
	for (int j = 0; j < 3; j++)
		if (_gameMap[0][j] == _gameMap[1][j] && _gameMap[1][j] == _gameMap[2][j] && _gameMap[0][j] != 0)
			return _gameMap[0][j];
	for (int i = 0; i < 3; i++)
		if (_gameMap[i][0] == _gameMap[i][1] && _gameMap[i][1] == _gameMap[i][2] && _gameMap[i][0] != 0)
			return _gameMap[i][0];
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