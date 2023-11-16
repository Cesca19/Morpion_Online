#include "ClientCore.h"

ClientCore::ClientCore()
{
	_game = new Game();
}

ClientCore::~ClientCore() {}

int ClientCore::init(std::string windowName, int width, int height, Client &client) {
	_game->init(windowName, width, height);
	if (client.init())
		return 1;

}

int ClientCore::run() {
	MSG msg = { 0 };
	sf::Event event;
	int i = 0;


	while (msg.message != WM_QUIT && _game->GetWindow()->isOpen()) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			if (!_game->GetWindow()->pollEvent(event)) {
				if (event.type == sf::Event::Closed)
					_game->GetWindow()->close();
			}

			if (i == 0)
			{
				_game->initPlayers(event);
				i++;
			}

			_game->run(event);
		}

	}

	return (int)msg.wParam;
}