#pragma once
#include "Player.h"

struct Position
{
    int x, y;
    Position(int x, int y) : x(x), y(y) {}
};

class Core
{
public:
    Core();
    ~Core();

    void InitGameMap();
    void InitPlayer(Player* firstPlayer, Player* secondPlayer);
    void ChangePlayer();
    void Move(Position pos);
    Player* GetWinner();
    int IsOver();

    Player* GetCurrentPlayer() { return m_currentPlayer; }
    Player* GetFirstPlayer() { return m_firstPlayer; }
    Player* GetSecondPlayer() { return m_secondPlayer; }
    int** GetGameMap() { return m_gameMap; } 

 private:
    Player* m_currentPlayer;
    Player* m_firstPlayer;
    Player* m_secondPlayer;
    int** m_gameMap;
    
    bool CheckMap(int row, int col, int drow, int dcol);
    bool IsFull();
};
