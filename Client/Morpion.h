#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "Client.h"
#include "../Core/Core.h"

class Morpion
{
public:
	Morpion();
	~Morpion();
	void init(std::string windowName, int width, int height);
	void run(sf::Event event);
	std::string getPlayerName(sf::Event *event);
	
	void createGameWindow(std::string name, int width, int height);

	void setId(int id);
	void setCore(void* core);
	void setStart();
	void setCurrentPlayer(std::string player);
	
	void setWinner(std::string name, bool isTie);
	
	void createGameBoard();
	int printGameboard();
	void printCurrentPlayer();

	void printEndGame();
	int turn(std::shared_ptr<sf::Text> mess, int number, sf::Event* event);
	void launchGame(sf::Event* event);
	std::shared_ptr<sf::RenderWindow> GetWindow() { return _window; }

private:
	std::string getPlayerName(std::string displayText, sf::Event* event);
	
private:
	int _id;
	bool _hasStart;
	void* _clientCore;

	std::string _winner;
	bool _isTie;
	bool _isEnd;
	
	std::shared_ptr<sf::RenderWindow> _window;
	std::vector<std::shared_ptr<sf::RectangleShape>> _gameBoard;
	std::vector<std::shared_ptr<sf::CircleShape>> _shapes;
	std::shared_ptr<sf::Font> _font;
	std::shared_ptr<sf::Text> _winMessage;

	std::shared_ptr<sf::Text> _currentPlayerText;
	std::shared_ptr<sf::Text> _waitMessage;

	std::string _windowName;
	int _width;
	int _height;

	std::string _name;
	std::string _currentPlayer;
};

