#include "Game.h"

#include <iostream>

Game::Game() : _window(NULL), _width(100), _height(100),
_font(new sf::Font())
{
	std::shared_ptr<sf::Text> win(new sf::Text("You win !!!", *(_font)));

	_font->loadFromFile("wall\\Wall.ttf");
	win->setCharacterSize(70);
	win->setStyle(sf::Text::Bold);
	win->setPosition({ (float)(_width / 2.5) , (float)(_height / 8) });
	win->setFillColor({ 222, 184, 135 });
	_winMessage = win;

	m_core = new Core();
	// m_client = new Client();
}

Game::~Game()
{
}

void Game::createGameWindow(std::string name, int width, int height)
{
	std::shared_ptr<sf::RenderWindow> window(new sf::RenderWindow(sf::VideoMode(width, height), name));
	_window = window;
}

void Game::init(std::string windowName, int width, int height)
{
	_windowName = windowName;
	_width = width;
	_height = height;
	createGameWindow(windowName, width, height);
}

void Game::run(sf::Event event)
{
	std::string firstPlayerName, secondPlayerName;

	createGameBoard();

	_window->clear(sf::Color::White);



	launchGame(&event);
	_window->clear(sf::Color::White);

	if (m_core->IsOver() == 1 || m_core->IsOver() == 2)
		_winMessage->setString("Congrats " + m_core->GetWinner()->GetName() + " You win !!!");
	else if (m_core->IsOver() == 3)
		_winMessage->setString("It's a tie, try harder next time ;(");

	printGameboard(0);

	_window->draw(*_winMessage);
	_window->display();
}

void Game::initPlayers(sf::Event event) {
	_firstPlayer = new Player(getPlayerName("Player 1", &event));
	_secondPlayer = new Player(getPlayerName("Player 2", &event));
	m_core->InitPlayer(_firstPlayer, _secondPlayer);
}

std::string  Game::getPlayerName(std::string displayText, sf::Event* event)
{
	std::string name;
	std::shared_ptr<sf::Text> text(new sf::Text(displayText + " enter your name ...", *(_font)));
	std::shared_ptr<sf::Text> playerName(new sf::Text("", *(_font)));

	text->setCharacterSize(80);
	text->setStyle(sf::Text::Bold);
	text->setPosition({ (float)(_width / 15) , (float)(_height / 8) });
	text->setFillColor({ 153, 50, 204 });
	playerName->setCharacterSize(70);
	playerName->setStyle(sf::Text::Bold);
	playerName->setPosition({ (float)(_width / 2.5) , (float)(_height / 3) });
	playerName->setFillColor({ 222, 184, 135 });
	while (1) {
		while (_window->pollEvent(*event)) {
			if (event->type == sf::Event::Closed)
				_window->close();
			if (event->type == sf::Event::TextEntered) {
				if (event->text.unicode == 13) {
					if (name.size() == 0)
						name = displayText;
					return name;
				}
				if (event->text.unicode == 8 && name.size() > 0)
					name.pop_back();
				if (event->text.unicode != 8 && event->text.unicode < 128)
					name += event->text.unicode;
				playerName->setString(name);
			}
		}
		_window->clear(sf::Color::White);
		_window->draw(*(text.get()));
		_window->draw(*(playerName.get()));
		_window->display();
	}
	return name;
}

int Game::turn(std::shared_ptr<sf::Text> mess, int number, sf::Event* event)
{
	int ret = 0;
	while (!ret) {
		while (_window->pollEvent(*event))
			if (event->type == sf::Event::Closed)
				_window->close();
		_window->clear(sf::Color::White);
		_window->draw(*mess);
		ret = printGameboard((number == 1) ? 3 : 8);
		_window->display();
	}
	return 0;
}

void Game::launchGame(sf::Event* event)
{
	std::string mess(m_core->GetCurrentPlayer()->GetName() + ", it's your turn !!");
	std::shared_ptr<sf::Text> text(new sf::Text(mess, *(_font)));

	text->setCharacterSize(80);
	text->setStyle(sf::Text::Bold);
	text->setPosition({ (float)(_width / 15) , (float)(_height / 15) });
	text->setFillColor({ 153, 50, 204 });
	while (m_core->IsOver() == 0) {
		if (m_core->GetCurrentPlayer() == m_core->GetFirstPlayer())
		{
			text->setString(m_core->GetCurrentPlayer()->GetName() + "  it's your turn !!");
			turn(text, 1, event);
		}
		else if (m_core->GetCurrentPlayer() == m_core->GetSecondPlayer())
		{
			text->setString(m_core->GetCurrentPlayer()->GetName() + "  it's your turn !!");
			turn(text, 2, event);
		}
	}

}

void Game::createGameBoard()
{
	float side = 100.0f;
	float startX = (float)_width / 2 - (side * 1.5f);
	float startY = (float)_height / 2 - (side * 1.5f);

	float y = startY;
	for (int i = 0; i < 3; i++, y += side) {
		float x = startX;
		for (int j = 0; j < 3; j++, x += side) {
			std::shared_ptr<sf::RectangleShape> shape(new sf::RectangleShape({ side, side }));
			shape->setOutlineColor(sf::Color::Cyan);
			shape->setOutlineThickness(5);
			shape->setFillColor(sf::Color::Black);
			shape->setPosition({ x + j * 10, y + i * 10 });
			_gameBoard.push_back(shape);
		}
	}
}

int Game::printGameboard(int sides)
{
	sf::Color color1 = { 255, 105, 180 };
	sf::Color color2 = { 255, 215, 0 };
	int** gameMap = m_core->GetGameMap();

	for (int i = 0; i < _gameBoard.size(); i++) {
		sf::FloatRect rect = _gameBoard[i]->getGlobalBounds();
		sf::Vector2i position = sf::Mouse::getPosition(*(_window.get()));

		if (rect.contains({ (float)position.x, (float)position.y })
			&& gameMap[i / 3][i % 3] == 0 && sides != 0) {
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right) ||
				sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				_gameBoard[i]->setOutlineColor(sf::Color::Magenta);

				int row = i / 3;
				int col = i % 3;

				m_core->Move({ row, col });

				for (int x = 0; x < 3; x++)
				{
					for (int y = 0; y < 3; y++)
					{
						std::cout << gameMap[x][y];
					}
					std::cout << std::endl;
				}

				// m_client->SendMove(row, col);

				float radius = (rect.width / 3);
				std::shared_ptr<sf::CircleShape>
					shape(new sf::CircleShape(radius, sides));
				shape->setPosition({ rect.left + rect.height / 5,
					rect.top + rect.width / 5 });
				shape->setOutlineThickness(3);
				shape->setOutlineColor((sides == 3) ? color1 : color2);
				shape->setFillColor(sf::Color::Black);
				_shapes.push_back(shape);
				return 1;
			}
			else
				_gameBoard[i]->setOutlineColor(sf::Color::Yellow);
		}
		else
			_gameBoard[i]->setOutlineColor(sf::Color::Cyan);
		_window->draw(*(_gameBoard[i].get()));
	}
	for (int i = 0; i < _shapes.size(); i++)
		_window->draw(*(_shapes[i].get()));

	return 0;
}