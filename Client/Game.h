#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "Client.h"
#include "../Core/Core.h"

class Game
{
public:
	Game();
	~Game();
	void init(std::string windowName, int width, int height);
	void run(sf::Event event);
	void initPlayers(sf::Event event);
	void createGameWindow(std::string name, int width, int height);
	std::string getPlayerName(std::string displayText, sf::Event *event);
	void createGameBoard();
	int printGameboard(int shape);
	int turn(std::shared_ptr<sf::Text> mess, int number, sf::Event* event);
	void launchGame(sf::Event* event);
	std::shared_ptr<sf::RenderWindow> GetWindow() { return _window; }
	
private:
	Core* m_core;
	Client* m_client;
	
	std::shared_ptr<sf::RenderWindow> _window;
	std::vector<std::shared_ptr<sf::RectangleShape>> _gameBoard;
	std::vector<std::shared_ptr<sf::CircleShape>> _shapes;
	std::shared_ptr<sf::Font> _font;
	std::shared_ptr<sf::Text> _winMessage;

	std::string _windowName;
	int _width;
	int _height;

	Player* _firstPlayer;
	Player* _secondPlayer;
};

