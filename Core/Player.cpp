#include "Player.h"

Player::Player(std::string name) : name(name)
{
}

Player::~Player()
{
}

std::string Player::GetName()
{
    return this->name;
}

void Player::SetName(std::string name)
{
    this->name = name;
}


