#include "Player.h"

Player::Player(int id, PlayerType type) :
	_id(id), _type(type)
{
}

Player::~Player()
{
}

void Player::setName(std::string name)
{
	_name = name;
}

std::string Player::getName()
{
	return _name;
}

int Player::getType()
{
	return (_type);
}

int Player::getId()
{
	return (_id);
}
