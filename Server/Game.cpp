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
    for (int i = 0; ; );
}

void Game::run()
{
    //OutputDebugStringA("hereeeeeeeeeeeeeeeeeeeeee\n");
    if (_isRunning) {
        OutputDebugStringA("hereeeeeeeeeeeeeeeeeeeeee             tooooooooooooo\n");
        // dire que c 'est au tour de _current de jouer
        sendMessageToPlayers("Board:" + convertBoard(_gameMap));
        sendMessageToPlayers("Turn:" + _currentPlayer);
        // attendre son movement
        while (((ServerCore*)_core)->getPlayerLastMessage() != "move")
        { }
        // move(x, y)
        // voir si qqun a gagné
        //if (checkWinner)
        //sendMessage("Winner or tie")
        
        // changer le current player
    }
}

void Game::move(int x, int y)
{
   /* if (m_currentPlayer == m_firstPlayer)
        m_gameMap[pos.x][pos.y] = 1;
    else
        m_gameMap[pos.x][pos.y] = 2;

    if (CheckWinner() == nullptr)
        ChangePlayer();*/
    // send game state
}

int Game::checkWinner()
{
    /*if (CheckMap(0, 0, 0, 1)) return m_currentPlayer;
    if (CheckMap(1, 0, 0, 1)) return m_currentPlayer;
    if (CheckMap(2, 0, 0, 1)) return m_currentPlayer;

    if (CheckMap(0, 0, 1, 0)) return m_currentPlayer;
    if (CheckMap(0, 1, 1, 0)) return m_currentPlayer;
    if (CheckMap(0, 2, 1, 0)) return m_currentPlayer;

    if (CheckMap(0, 0, 1, 1)) return m_currentPlayer;
    if (CheckMap(2, 0, -1, 1)) return m_currentPlayer;

    return nullptr;*/
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