#include "Core.h"

Core::Core()
{
    InitGameMap();
}

Core::~Core()
{
    for (int i = 0; i < 3; i++)
        delete m_gameMap[i];
    delete [] m_gameMap;
}

Player* Core::GetCurrentPlayer()
{
    return m_currentPlayer;
}

void Core::InitGameMap()
{
    m_gameMap = new int*[3];
    for (int i = 0; i < 3; i++)
    {
        m_gameMap[i] = new int[3];
        m_gameMap[i][0] = m_gameMap[i][1] = m_gameMap[i][2] = 0;
    }
}

void Core::InitPlayer(std::string firstPlayer, std::string secondPlayer)
{
    m_firstPlayer = new Player(firstPlayer);
    m_secondPlayer = new Player(secondPlayer);
    m_currentPlayer = m_firstPlayer;
}

void Core::ChangePlayer()
{
    if (m_currentPlayer == m_firstPlayer)
        m_currentPlayer = m_secondPlayer;
    m_currentPlayer = m_secondPlayer;
}

void Core::Move(Position pos)
{
    if (m_currentPlayer == m_firstPlayer)
        m_gameMap[pos.x][pos.y] = 1;
    else
        m_gameMap[pos.x][pos.y] = 2;
    
    if (CheckWinner() == nullptr)
        ChangePlayer();
    // send game state
}

Player* Core::CheckWinner()
{
    if (CheckMap(0, 0, 0, 1)) return m_currentPlayer;
    if (CheckMap(1, 0, 0, 1)) return m_currentPlayer;
    if (CheckMap(2, 0, 0, 1)) return m_currentPlayer;

    if (CheckMap(0, 0, 1, 0)) return m_currentPlayer;
    if (CheckMap(0, 1, 1, 0)) return m_currentPlayer;
    if (CheckMap(0, 2, 1, 0)) return m_currentPlayer;
    
    if (CheckMap(0, 0, 1, 1)) return m_currentPlayer;
    if (CheckMap(2, 0, -1, 1)) return m_currentPlayer;
    
    return nullptr;
}

int Core::IsOver()
{
    if (CheckWinner() == nullptr && IsFull()) return 3; // tie 
    if (CheckWinner() == m_firstPlayer) return 1; // player 1 wins
    if (CheckWinner() == m_secondPlayer) return 2; // player 2 wins
    return 0;
}

bool Core::CheckMap(int row, int col, int drow, int dcol)
{
    if (m_gameMap[row][col] == 0)
        return false;
    if (m_gameMap[row][col] != m_gameMap[row + drow][col + dcol])
        return false;
    if (m_gameMap[row][col] != m_gameMap[row + 2 * drow][col + 2 * dcol])
        return false;
    return true;
}

bool Core::IsFull()
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (m_gameMap[i][j] == 0)
                return false;
        }
    }
    return true;
}