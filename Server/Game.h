#pragma once
#include <iostream>
#include <string>
#include <vector>

class Game
{
public:
    Game();
    ~Game();

    void initGameMap();
    void setGameMap(int ** map);
    int** getGameMap();

    std::string getCurrentPlayer();
    void setCurrentPlayer(std::string player);
    
    void setCore(void* core);
    void sendMessageToPlayer(std::string name, std::string message);
    void sendMessageToPlayers(std::string message);

    void addWatcher(std::string);
    void addPlayer(std::string name, int rank);
   // void initPlayer(std::string firstPlayer, std::string secondPlayer);
    void changePlayer();
    void move(int x, int y);
    int checkWinner();
    int isOver();
    void run();

private:
    std::string _currentPlayer;
    std::vector<std::string> _players;
    /*std::string _firstPlayer;
    std::string _secondPlayer;*/

    bool _isRunning;
    void* _core;

    int** _gameMap;
   // bool CheckMap(int row, int col, int drow, int dcol);
    //bool IsFull();
};

