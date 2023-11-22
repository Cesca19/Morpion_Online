#pragma once

#include <string>

enum PlayerType {
	SPECTATOR,
	PLAYER1,
	PLAYER2
};

class Player
{
public:
	Player(int id, PlayerType type);
	~Player();
	void setName(std::string name);
	std::string getName();
	int getType();
	int getId();
	
private:
	int _id;
	PlayerType _type;
	std::string _name;
	
};

