#include "Game.h"
#include "ServerCore.h"

Game::Game() : _gameMap(NULL), _currentPlayer(""),
_firstPlayer(""), _secondPlayer(""), _isRunning(false)
{
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


void Game::initPlayer(std::string firstPlayer, std::string secondPlayer)
{
    _firstPlayer = firstPlayer;
    _secondPlayer = secondPlayer;
    _currentPlayer = _firstPlayer;
    _isRunning = true;
}

void Game::changePlayer()
{
    if (_currentPlayer == _firstPlayer)
        _currentPlayer = _secondPlayer;
    else
        _currentPlayer = _firstPlayer;
}

void Game::setCore(void* core)
{
    _core = core;
}

void Game::sendMessageToPlayers(std::string message)
{
    ServerCore* core = (ServerCore *)_core;
    core->sendMessageToPlayers(message);
}

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
    std::cout << str << std::endl;
    return mess;
}

void Game::run()
{
    static std::string prevPlayer = "";
    std::string mov;
    std::vector<std::string> mess;
    if (_isRunning && checkWinner() == 0 )  {
        // dire que c 'est au tour de _current de jouer
       // sendMessageToPlayers("Board:" + convertBoard(_gameMap));
        if (prevPlayer != _currentPlayer) {
            OutputDebugStringA("################################################\n");
            sendMessageToPlayers("Turn:" + _currentPlayer);
            OutputDebugStringA(std::string("---------------------- PLAYER " + _currentPlayer + " TURN ------------------------\n").c_str());
            
            prevPlayer = _currentPlayer;
        }
        // attendre son movement
        mov = ((ServerCore*)_core)->getPlayerLastMessage();
        //OutputDebugStringA("here may be\n");
        if (mov != "" && mov.substr(0, 5) == "move:") {
            OutputDebugStringA("---------------------------------\n");
            mess = split(mov, ":");
            move(stoi(mess[1]), stoi(mess[2]));
            // voir si qqun a gagné
            // 
            if (checkWinner() != 0)
                sendMessageToPlayers("Winner or tie");
            changePlayer();
        }
        OutputDebugStringA(("++++++++++++++++" + _currentPlayer + "+++++++++++++++++++++\n").c_str());
    }
}

void Game::move(int x, int y)
{
   if (_currentPlayer == _firstPlayer)
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
    /*if (CheckWinner() == nullptr && IsFull()) return 3; // tie 
    if (CheckWinner() == m_firstPlayer) return 1; // player 1 wins
    if (CheckWinner() == m_secondPlayer) return 2; // player 2 wins*/
    return 0;
}

/*bool Game::CheckMap(int row, int col, int drow, int dcol)
{
    if (m_gameMap[row][col] == 0)
        return false;
    if (m_gameMap[row][col] != m_gameMap[row + drow][col + dcol])
        return false;
    if (m_gameMap[row][col] != m_gameMap[row + 2 * drow][col + 2 * dcol])
        return false;
    return true;
}
*/

/*
bool Game::IsFull()
{
    /*for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (m_gameMap[i][j] == 0)
                return false;
        }
    }
    return true;
}*/