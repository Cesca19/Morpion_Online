#include "ClientCore.h"

ClientCore::ClientCore()
{
	std::shared_ptr<sf::Text> win(new sf::Text("You win !!!", *(_game._font)));

	_game._font->loadFromFile("wall\\Wall.ttf");
	win->setCharacterSize(70);
	win->setStyle(sf::Text::Bold);
	win->setPosition({ (float)(_game._width / 2.5) , (float)(_game._height / 8) });
	win->setFillColor({ 222, 184, 135 });
	_game._winMessage = win;
	_game._gameMap = new int* [3];
	for (int j = 0; j < 3; j++) {
		_game._gameMap[j] = new int[3];
		_game._gameMap[j][0] = _game._gameMap[j][1] = _game._gameMap[j][2] = 0;
	}
}

ClientCore::~ClientCore() {}

int ClientCore::init(std::string windowName, int width, int height, Client &client) {
	_game.init(windowName, width, height);
	if (client.init())
		return 1;

}

int ClientCore::run() {
	MSG msg = { 0 };
	sf::Event event;
	while (msg.message != WM_QUIT && _game._window->isOpen()) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (!_game._window->pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				_game._window->close();
		}


		_game.run(event);
	}

	for (int i = 0; i < 3; i++)
		delete _game._gameMap[i];
	delete[] _game._gameMap;


	return (int)msg.wParam;
}