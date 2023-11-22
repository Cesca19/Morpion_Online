#include "ServerUI.h"
#include "ServerCore.h"


ServerUI::ServerUI(std::string name, int width, int height) :
	_name(name), _width(width), _height(height),
	_textFont(new sf::Font())
{
	std::shared_ptr<sf::RenderWindow> window(new sf::RenderWindow(sf::VideoMode(width, height), name));
	std::shared_ptr<sf::Text> head(new sf::Text("Welcome to the server", *(_textFont)));
	std::shared_ptr<sf::Text> web(new sf::Text("", *(_textFont)));
	std::shared_ptr<sf::Text> game(new sf::Text("", *(_textFont)));

	_textFont->loadFromFile("wall\\Wall.ttf");
	web->setCharacterSize(50);
	web->setStyle(sf::Text::Bold);
	web->setPosition({ (float)(_width / 6) , (float)(_height / 3) });
	web->setFillColor({ 143, 188, 143 });
	game->setCharacterSize(50);
	game->setStyle(sf::Text::Bold);
	game->setPosition({ (float)(_width / 6) , (float)(_height / 2) });
	game->setFillColor({ 143, 188, 143 });
	createTextButton();
	head->setCharacterSize(70);
	head->setStyle(sf::Text::Bold);
	head->setPosition({ (float)(_width / 5) , (float)(_height / 15) });
	head->setFillColor({ 192, 192, 192 });
	
	_webText = web;
	_gameText = game;
	_headText = head;
	_window = window;
}

ServerUI::~ServerUI()
{
}

void ServerUI::createTextButton()
{
	std::shared_ptr<sf::Text> lauch(new sf::Text("Run server", *(_textFont)));
	std::shared_ptr<sf::Text> quit(new sf::Text("Quit server", *(_textFont)));
	std::shared_ptr<sf::Text> stop(new sf::Text("Stop server", *(_textFont)));

	lauch->setCharacterSize(70);
	lauch->setStyle(sf::Text::Bold);
	lauch->setPosition({ (float)(_width / 10) , (float)(_height / 3) });
	lauch->setFillColor({ 210, 180, 140 });
	lauch->setOutlineColor({ 210, 180, 140 });
	_button.push_back(lauch);

	quit->setCharacterSize(70);
	quit->setStyle(sf::Text::Bold);
	quit->setPosition({ (float)(_width / 2) , (float)(_height / 3) });
	quit->setFillColor({ 210, 180, 140 });
	quit->setOutlineColor({ 210, 180, 140 });
	_button.push_back(quit);

	stop->setCharacterSize(70);
	stop->setStyle(sf::Text::Bold);
	stop->setPosition({ (float)(_width / 3) , _height - (float)(_height / 4) });
	stop->setFillColor({ 210, 180, 140 });
	_button.push_back(stop);
}

std::string ServerUI::getPlayerInput(std::string displayText, sf::Event* event)
{
	std::string input = "";
	ServerCore* core = ((ServerCore*)(_core));
	std::shared_ptr<sf::Text> text(new sf::Text(displayText, *(_textFont)));
	std::shared_ptr<sf::Text> playerInput(new sf::Text("", *(_textFont)));

	text->setCharacterSize(50);
	text->setStyle(sf::Text::Bold);
	text->setPosition({ (float)(_width / 6) , (float)(_height / 4) });
	text->setFillColor(sf::Color::Cyan);
	playerInput->setCharacterSize(40);
	playerInput->setStyle(sf::Text::Bold);
	playerInput->setPosition({ (float)(_width / 2.5) , (float)(_height / 2.5) });
	playerInput->setFillColor({ 210, 180, 140 });
	while (1) {
		while (_window->pollEvent(*event)) {
			if (event->type == sf::Event::Closed) {
				core->setState(STOP);
				return "";
			}
			if (event->type == sf::Event::TextEntered) {
				if (event->text.unicode == 13 && input.size() != 0) {
					return input;
				}
				if (event->text.unicode == 8 && input.size() > 0)
					input.pop_back();
				if (event->text.unicode != 8  && event->text.unicode != 13 && event->text.unicode < 128)
					input += event->text.unicode;
				playerInput->setString(input);
			}
		}
		_window->clear(sf::Color::White);
		_window->draw(*_headText);
		_window->draw(*(text.get()));
		_window->draw(*(playerInput.get()));
		_window->display();
	}
	return input;
}

void ServerUI::setCore(void* core)
{
	_core = core;
}

std::shared_ptr<sf::RenderWindow> ServerUI::getWindow()
{
	return _window;
}

void ServerUI::display()
{
	ServerCore* core = ((ServerCore*)(_core));

	sf::FloatRect rect = _button[2]->getGlobalBounds();
	sf::Vector2i position = sf::Mouse::getPosition(*(_window.get()));

	if (rect.contains({ (float)position.x, (float)position.y })) {
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right) ||
			sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			_button[2]->setFillColor(sf::Color::Magenta);
			core->setState(SERVER_STATE::STOP);
			return;
		} else
			_button[2]->setFillColor(sf::Color::Cyan);
	} else
		_button[2]->setFillColor({ 210, 180, 140 });
	_window->clear(sf::Color::White);
	_window->draw(*_headText);
	_window->draw(*_webText);
	_window->draw(*_gameText);
	_window->draw(*_button[2]);
	_window->display();
}

void ServerUI::displayAllButtons(sf::Event* event)
{
	ServerCore* core = ((ServerCore*)(_core));

	for (int i = 0; i < 2; i++) {
		sf::FloatRect rect = _button[i]->getGlobalBounds();
		sf::Vector2i position = sf::Mouse::getPosition(*(_window.get()));

		if (rect.contains({ (float)position.x, (float)position.y })) {
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right) ||
				sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				_button[i]->setFillColor(sf::Color::Magenta);
				core->setState((i == 0) ? SERVER_STATE::RUN : SERVER_STATE::STOP);
				return;
			}
			else
				_button[i]->setFillColor(sf::Color::Cyan);
		} else
			_button[i]->setFillColor({ 210, 180, 140 });
	}
	_window->clear(sf::Color::White);
	_window->draw(*_headText);
	_window->draw(*_button[0]);
	_window->draw(*_button[1]);
	_window->display();
}

void ServerUI::setServerPort(std::string gamePort, std::string webPort)
{
	_gamePort = gamePort;
	_webPort = webPort;

	_webText->setString("Web server running on port: " + _webPort);
	_gameText->setString("Game server running on port: " + _gamePort);

}