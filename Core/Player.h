#pragma once
#include <string>

class Player
{
public:
    Player(std::string name);
    ~Player();
    void SetName(std::string name);
    std::string GetName();

private:
    std::string name;
    
};
