#pragma once
#include "pch.h"
#include <SFML/Graphics.hpp>

class ServerUI
{
public:
	ServerUI(std::string name, int width, int height);
	~ServerUI();
	std::shared_ptr<sf::RenderWindow> getWindow();
	std::string getPlayerInput(std::string displayText, sf::Event *event);
	void displayAllButtons(sf::Event* event);
	void createTextButton();
	void setCore(void *core);
	void setServerPort(std::string gamePort, std::string webPort);
	void display();
private:
	std::string _name;
	int _width;
	int _height;
	std::vector<std::shared_ptr<sf::Text>> _button;
	std::shared_ptr<sf::RenderWindow> _window;
	std::shared_ptr<sf::Text> _headText;
	std::shared_ptr<sf::Font> _textFont;

	std::shared_ptr<sf::Text> _gameText;
	std::shared_ptr<sf::Text> _webText;

	std::string _gamePort;
	std::string _webPort;

	void* _core;
};

