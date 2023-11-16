#pragma once

#include <memory>
#include <iostream>
#include <SFML/Graphics.hpp>

class Game
{
public:
	Game();
	~Game();
	void init(std::string windowName, int width, int height);
	void run(sf::Event event);
	void createGameWindow(std::string name, int width, int height);
	std::string getPlayerName(std::string displayText, sf::Event* event);
	void createGameBoard();
	int printGameboard(int shape);
	int checkWinner();
	int turn(std::shared_ptr<sf::Text> mess, int number, sf::Event* event);
	int launchGame(std::string player1, std::string player2, sf::Event* event);
	std::shared_ptr<sf::RenderWindow> _window;
	int** _gameMap;
	std::vector<std::shared_ptr<sf::RectangleShape>> _gameBoard;
	std::vector<std::shared_ptr<sf::CircleShape>> _shapes;
	std::shared_ptr<sf::Font> _font;
	//std::shared_ptr<sf::Text> _winnerName;
	std::shared_ptr<sf::Text> _winMessage;

	std::string _windowName;
	int _width;
	int _height;
	
};



