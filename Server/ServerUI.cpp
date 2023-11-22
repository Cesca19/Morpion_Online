#include "ServerUI.h"
#include "ServerCore.h"


ServerUI::ServerUI(std::string name, int width, int height) :
	_name(name), _width(width), _height(height), _buttonFont(new sf::Font()),
	_textFont(new sf::Font())
{
	std::shared_ptr<sf::RenderWindow> window(new sf::RenderWindow(sf::VideoMode(width, height), name));
	std::shared_ptr<sf::Text> head(new sf::Text("Welcome to the server", *(_textFont)));
	std::shared_ptr<sf::Text> run(new sf::Text("The server is running ...", *(_textFont)));

	_textFont->loadFromFile("wall\\Wall.ttf");
	_buttonFont->loadFromFile("wall\\Wall.ttf");
	createTextButton();
	head->setCharacterSize(60);
	head->setStyle(sf::Text::Bold);
	head->setPosition({ (float)(_width / 5) , (float)(_height / 15) });
	head->setFillColor(sf::Color::Cyan);

	run->setCharacterSize(50);
	run->setStyle(sf::Text::Bold);
	run->setPosition({ (float)(_width / 4) , (float)(_height / 4) });
	run->setFillColor(sf::Color::Cyan);
	
	_runningText = run;
	_headText = head;
	_window = window;
}

ServerUI::~ServerUI()
{
}

void ServerUI::createTextButton()
{
	std::shared_ptr<sf::Text> lauch(new sf::Text("Run server", *(_buttonFont)));
	std::shared_ptr<sf::Text> quit(new sf::Text("Quit server", *(_buttonFont)));
	std::shared_ptr<sf::Text> stop(new sf::Text("Stop server", *(_buttonFont)));

	lauch->setCharacterSize(70);
	lauch->setStyle(sf::Text::Bold);
	lauch->setPosition({ (float)(_width / 10) , (float)(_height / 3) });
	lauch->setFillColor({ 222, 184, 135 });
	//lauch->setOutlineThickness(3);
	lauch->setOutlineColor({ 222, 184, 135 });
	//lauch->setOutlineColor(5);
	_button.push_back(lauch);

	quit->setCharacterSize(70);
	quit->setStyle(sf::Text::Bold);
	quit->setPosition({ (float)(_width / 2) , (float)(_height / 3) });
	quit->setFillColor({ 222, 184, 135 });
	//quit->setOutlineThickness(3);
	quit->setOutlineColor({ 222, 184, 135 });
	_button.push_back(quit);

	stop->setCharacterSize(70);
	stop->setStyle(sf::Text::Bold);
	stop->setPosition({ (float)(_width / 3) , (float)(_height / 3) });
	stop->setFillColor({ 222, 184, 135 });
	//stop->setOutlineThickness(3);
	//stop->setOutlineColor({ 222, 184, 135 });
	_button.push_back(stop);
}

std::string ServerUI::getPlayerInput(std::string displayText, sf::Event* event)
{
	std::string input;
	ServerCore* core = ((ServerCore*)(_core));
	std::shared_ptr<sf::Text> text(new sf::Text(displayText, *(_textFont)));
	std::shared_ptr<sf::Text> playerInput(new sf::Text("", *(_textFont)));

	text->setCharacterSize(50);
	text->setStyle(sf::Text::Bold);
	text->setPosition({ (float)(_width / 15) , (float)(_height / 4) });
	text->setFillColor({ 153, 50, 204 });
	playerInput->setCharacterSize(45);
	playerInput->setPosition({ (float)(_width / 2.5) , (float)(_height / 3) });
	playerInput->setFillColor({ 222, 184, 135 });
	while (1) {
		while (_window->pollEvent(*event)) {
			if (event->type == sf::Event::Closed) {
				_window->close();
				//core->close();
				return "";
			}
			if (event->type == sf::Event::TextEntered) {
				if (event->text.unicode == 13 && input.size() != 0) {
					return input;
				}
				if (event->text.unicode == 8 && input.size() > 0)
					input.pop_back();
				if (event->text.unicode != 8 && event->text.unicode < 128)
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
			//core->sendMessage(Protocol::GameProtocol::createMoveMessage(_name, i / 3, i % 3));
			return;
		} else
			_button[2]->setFillColor(sf::Color::Cyan);
	} else
		_button[2]->setFillColor({ 222, 184, 135 });
	_window->clear(sf::Color::White);
	_window->draw(*_headText);
	_window->draw(*_runningText);
	_window->draw(*_button[2]);
	_window->display();
}

void ServerUI::displayAllButtons(sf::Event* event)
{
	ServerCore* core = ((ServerCore*)(_core));

	while (1) {
		while (_window->pollEvent(*event)) {
			if (event->type == sf::Event::Closed) {
				_window->close();
				//core->close();
				return ;
			}
		}
		for (int i = 0; i < 2; i++) {
			sf::FloatRect rect = _button[i]->getGlobalBounds();
			sf::Vector2i position = sf::Mouse::getPosition(*(_window.get()));

			if (rect.contains({ (float)position.x, (float)position.y })) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right) ||
					sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					_button[i]->setFillColor(sf::Color::Magenta);
					core->setState((i == 0) ? SERVER_STATE::RUN : SERVER_STATE::STOP);
					//core->sendMessage(Protocol::GameProtocol::createMoveMessage(_name, i / 3, i % 3));
					return;
				}
				else
					_button[i]->setFillColor(sf::Color::Cyan);
			}
			else
				_button[i]->setFillColor({ 222, 184, 135 });
		}
		_window->clear(sf::Color::White);
		_window->draw(*_headText);
		_window->draw(*_button[0]);
		_window->draw(*_button[1]);
		_window->display();
	}
}