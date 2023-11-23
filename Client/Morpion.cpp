#include "Morpion.h"
#include "ClientCore.h"

Morpion::Morpion() : _id(-1), _window(NULL), _width(0), _height(0), _currentPlayer(""),
_font(new sf::Font()), _name(""), _hasStart(false), _winner(""), _isTie(false), _isSent(false),
_isEnd(false),  _clientCore(NULL), _requestSent(false)
{
}

Morpion::~Morpion()
{
}

void Morpion::createGameWindow(std::string name, int width, int height)
{
	std::shared_ptr<sf::RenderWindow> window(new sf::RenderWindow(sf::VideoMode(width, height), name));
	std::shared_ptr<sf::Text> win(new sf::Text("You win !!!", *(_font)));
	std::shared_ptr<sf::Text> wait(new sf::Text("Waiting for more players ...", *(_font)));
	std::shared_ptr<sf::Text> text(new sf::Text("", *(_font)));
	std::shared_ptr<sf::Text> hist(new sf::Text("", *(_font)));

	_width = width;
	_height = height;

	_font->loadFromFile("wall\\Wall.ttf");

	hist->setCharacterSize(40);
	//hist->setStyle(sf::Text::Bold);
	hist->setPosition({ (float)(_width / 3) , (float)(_height / 10) });
	hist->setFillColor({ 222, 184, 135 });
	_historicText = hist;

	win->setCharacterSize(70);
	win->setStyle(sf::Text::Bold);
	win->setPosition({ (float)(_width / 3) , (float)(_height / 10) });
	win->setFillColor({ 222, 184, 135 });
	_winMessage = win;
	wait->setCharacterSize(70);
	wait->setStyle(sf::Text::Bold);
	wait->setPosition({ (float)(_width / 15) , (float)(_height / 3) });
	wait->setFillColor({ 222, 184, 135 });
	_waitMessage = wait;
	text->setCharacterSize(80);
	text->setStyle(sf::Text::Bold);
	text->setPosition({ (float)(_width / 15) , (float)(_height / 15) });
	text->setFillColor({ 153, 50, 204 });
	_currentPlayerText = text;
	createGameBoard();
	_window = window;
}

void Morpion::setStart()
{
	_hasStart = true;
}

void Morpion::init(std::string windowName, int width, int height)
{
	_windowName = windowName;
	_width = width;
	_height = height;
	createGameWindow(windowName, width, height);
}

void Morpion::run(sf::Event event)
{
	_window->clear(sf::Color::White);

	if (_hasStart) {
		if (_currentPlayer != _name)
			_isSent = false;
		if (_historicText->getString() != "") {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
				_historicText->setString("");
			_window->clear(sf::Color::White);
			_window->draw(*_historicText);
		}
		else {
			printCurrentPlayer();
			printGameboard();
			AskHistoricToServer();
			if (_isEnd)
				printEndGame();
		}
	}
	else {
		_window->draw(*_waitMessage);
	}
	_window->display();
}

void Morpion::printEndGame()
{
	if (_isTie) {
		_winMessage->setString("It's a tie ;(");
		_window->draw(*_winMessage);
		return;
	}
	else
		(_name == _winner) ? _winMessage->setString("You win, not bad ...") : (_name != _winner && _id < 3)
		? _winMessage->setString("You loose ...") : _winMessage->setString(_winner + " win !!");
	_window->draw(*_winMessage);
}

void Morpion::setWinner(std::string name, bool isTie)
{
	_isEnd = true;
	_isTie = isTie;
	_winner = name;
}

void Morpion::printCurrentPlayer()
{
	if (_currentPlayer != "" && !_isEnd) {
		_currentPlayerText->setString(_currentPlayer + "  it's your turn !!");
		_window->draw(*_currentPlayerText);
	}
}

void Morpion::setCurrentPlayer(std::string player)
{
	_currentPlayer = player;
}

std::string Morpion::getPlayerName(sf::Event* event)
{
	return(getPlayerName("Please", event));
}

void Morpion::createTextButton()
{
	std::shared_ptr<sf::Text> lauch(new sf::Text("Connect", *(_font)));
	std::shared_ptr<sf::Text> quit(new sf::Text("Quit", *(_font)));
	//std::shared_ptr<sf::Text> stop(new sf::Text("Stop server", *(_font)));

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

	/*stop->setCharacterSize(70);
	stop->setStyle(sf::Text::Bold);
	stop->setPosition({ (float)(_width / 3) , _height - (float)(_height / 4) });
	stop->setFillColor({ 210, 180, 140 });
	_button.push_back(stop);*/
}

int Morpion::connectionPage(sf::Event *event)
{
	ClientCore* core = ((ClientCore*)(_clientCore));
	bool end = true;
	std::shared_ptr<sf::Text> head(new sf::Text("Connect to the server", *(_font)));
	
	createTextButton();
	head->setCharacterSize(70);
	head->setStyle(sf::Text::Bold);
	head->setPosition({ (float)(_width / 5) , (float)(_height / 15) });
	head->setFillColor({ 192, 192, 192 });
	while (end && _window->isOpen()) {
		while (_window->pollEvent(*event)) {
			if (event->type == sf::Event::Closed) {
				_window->close();
				core->close();
				return 1;
			}
		}
		for (int i = 0; i < 2; i++) {
			sf::FloatRect rect = _button[i]->getGlobalBounds();
			sf::Vector2i position = sf::Mouse::getPosition(*(_window.get()));

			if (rect.contains({ (float)position.x, (float)position.y })) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right) ||
					sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					_button[i]->setFillColor(sf::Color::Magenta);
					if (i == 0)
						return 0;
					else {
						_window->close();
						core->close();
						return 1;
					}
				}
				else
					_button[i]->setFillColor(sf::Color::Cyan);
			}
			else
				_button[i]->setFillColor({ 210, 180, 140 });
		}
		_window->clear(sf::Color::White);
		_window->draw(*_button[0]);
		_window->draw(*_button[1]);
		_window->draw(*head);
		_window->display();
	}
	return 0;
}

std::string Morpion::getPlayerInput(std::string displayText, sf::Event* event)
{
	std::string input = "";
	ClientCore* core = ((ClientCore*)(_clientCore));
	std::shared_ptr<sf::Text> text(new sf::Text(displayText, *(_font)));
	std::shared_ptr<sf::Text> playerInput(new sf::Text("", *(_font)));

	text->setCharacterSize(50);
	text->setStyle(sf::Text::Bold);
	text->setPosition({ (float)(_width / 6) , (float)(_height / 4) });
	text->setFillColor(sf::Color::Cyan);
	playerInput->setCharacterSize(40);
	playerInput->setStyle(sf::Text::Bold);
	playerInput->setPosition({ (float)(_width / 2.5) , (float)(_height / 2.5) });
	playerInput->setFillColor({ 210, 180, 140 });
	while (_window->isOpen()) {
		while (_window->pollEvent(*event)) {
			if (event->type == sf::Event::Closed) {
				_window->close();
				core->close();
				return "";
			}
			if (event->type == sf::Event::TextEntered) {
				if (event->text.unicode == 13 && input.size() != 0) {
					return input;
				}
				if (event->text.unicode == 8 && input.size() > 0)
					input.pop_back();
				if (event->text.unicode != 8 && event->text.unicode != 13 && event->text.unicode < 128)
					input += event->text.unicode;
				playerInput->setString(input);
			}
		}
		_window->clear(sf::Color::White);
		_window->draw(*(text.get()));
		_window->draw(*(playerInput.get()));
		_window->display();
	}
	return input;
}

std::string Morpion::getPlayerName(std::string displayText, sf::Event* event)
{
	std::string name;
	ClientCore* core = ((ClientCore*)(_clientCore));
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
	while (_window->isOpen()) {
		while (_window->pollEvent(*event)) {
			if (event->type == sf::Event::Closed) {
				_window->close();
				core->close();
				return "";
			}
			if (event->type == sf::Event::TextEntered) {
				if (event->text.unicode == 13 && name.size() != 0) {
					_name = name;
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
	_name = name;
	return name;
}

int Morpion::turn(std::shared_ptr<sf::Text> mess, int number, sf::Event* event)
{
	return 0;
}

void Morpion::launchGame(sf::Event* event)
{
}

void Morpion::createGameBoard()
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

void Morpion::setCore(void* core)
{
	_clientCore = core;
}

void Morpion::setId(int id)
{
	_id = id;
}

std::string convertBoard(int** board)
{
	std::string mess;
	for (int i = 0; i < 3; i++) {
		mess += board[i][0] + '0';
		mess += board[i][1] + '0';
		mess += board[i][2] + '0';
	}
	return mess;
}

int Morpion::printGameboard()
{
	float radius = 100.0f / 3.0f;
	int player1 = (_id < 3) ? _id : 1;
	int player2 = (_id == 2) ? 1 : 2;
	ClientCore* core = ((ClientCore*)(_clientCore));
	sf::Color color1 = { 255, 105, 180 };
	sf::Color color2 = { 255, 215, 0 };
	int** gameMap = core->getGameMap();

	_shapes.clear();
	for (int i = 0; i < _gameBoard.size(); i++) {
		sf::FloatRect rect = _gameBoard[i]->getGlobalBounds();
		sf::Vector2i position = sf::Mouse::getPosition(*(_window.get()));

		if (gameMap) {
			if (gameMap[i / 3][i % 3] != 0 && gameMap[i / 3][i % 3] == player1) {
				std::shared_ptr<sf::CircleShape>
					shape(new sf::CircleShape(radius, 8));
				shape->setPosition({ rect.left + rect.height / 5,
					rect.top + rect.width / 5 });
				shape->setOutlineThickness(3);
				shape->setOutlineColor(color2);
				shape->setFillColor(sf::Color::Black);
				_shapes.push_back(shape);

			}
			else if (gameMap[i / 3][i % 3] != 0 && gameMap[i / 3][i % 3] == player2) {
				std::shared_ptr<sf::CircleShape>
					shape(new sf::CircleShape(radius, 3));
				shape->setPosition({ rect.left + rect.height / 5,
					rect.top + rect.width / 5 });
				shape->setOutlineThickness(3);
				shape->setOutlineColor(color1);
				shape->setFillColor(sf::Color::Black);
				_shapes.push_back(shape);
			}

			if (rect.contains({ (float)position.x, (float)position.y })
				&& gameMap[i / 3][i % 3] == 0 && _currentPlayer == _name && !_isEnd && !_isSent) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Right) ||
					sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					_gameBoard[i]->setOutlineColor(sf::Color::Magenta);
					core->sendMessage(Protocol::GameProtocol::createMoveMessage(_name, i / 3, i % 3) );
					_isSent = true;
					return 1;
				}
				else
					_gameBoard[i]->setOutlineColor(sf::Color::Yellow);
			}
			else
				_gameBoard[i]->setOutlineColor(sf::Color::Cyan);
		}
		_window->draw(*(_gameBoard[i].get()));
	}

	for (int i = 0; i < _shapes.size(); i++)
		_window->draw(*(_shapes[i].get()));

	return 0;
}

void Morpion::DisplayHistoric(std::string historic)
{
	_historicText->setString(historic);
	_requestSent = false;
}

void Morpion::AskHistoricToServer()
{
	ClientCore* core = ((ClientCore*)(_clientCore));

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::H) && !_requestSent) {
		OutputDebugStringA((_name + " Asking histtttttttttttttttttttttttttttttttttttttttttttttttttttttt\n").c_str());
		core->sendMessage("historic#" + _name + "#");
		_requestSent = true;
	}
}