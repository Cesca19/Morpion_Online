#pragma once
#include "Player.h"

struct Position
{
    int x, y;
    Position(int x, int y) : x(x), y(y);
};

class Core
{
public:
    Core();
    ~Core();
    Player* GetCurrentPlayer();

    void InitGameMap();
    void InitPlayer(std::string firstPlayer, std::string secondPlayer);
    void ChangePlayer();
    void Move(Position pos);
    Player* CheckWinner();
    int IsOver();
    void Run();

private:
    Player* m_currentPlayer;
    Player* m_firstPlayer;
    Player* m_secondPlayer;
    
    int** m_gameMap;
    bool CheckMap(int row, int col, int drow, int dcol);
    bool IsFull();
};
