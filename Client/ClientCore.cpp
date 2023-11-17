#include "ClientCore.h"

std::vector<std::string> split(std::string message, std::string delimiter)
{
	std::vector<std::string> mess;
	std::string str = message;
	size_t pos = 0;
	std::string token;

	while ((pos = str.find(delimiter)) != std::string::npos) {
		token = str.substr(0, pos);
		mess.push_back(token);
		str.erase(0, pos + delimiter.length());
	}
	mess.push_back(str);
	std::cout << str << std::endl;
	return mess;
}

int** convertStringBoard(std::string mess)
{
	int **map = new int* [3];
	for (int i = 0, j = 0; i < 3 && j < mess.size(); i++) {
		map[i] = new int[3];
		map[i][0] = mess[j] - 48; j++;
		map[i][1] = mess[j] - 48; j++;
		map[i][2] = mess[j] - 48; j++;
	}
	return map;
}

ClientCore::ClientCore(HINSTANCE hInstance) : 
	_client(new  Client(hInstance, "127.0.0.1", "6666")), _name(""),
	_map(NULL)
{
	_game = new Morpion();
}

ClientCore::~ClientCore() {}

int ClientCore::init(std::string windowName, int width, int height) {
	_game->setCore(this);
	_game->init(windowName, width, height);
	_client->setCore(this);
	if (_client->init())
		return 1;

}

int **ClientCore::getGameMap()
{
	return _map;
}

void ClientCore::setGameMap(int **map)
{
	_map = map;
}

void ClientCore::setCurrentPlayer(std::string player)
{
	_game->setCurrentPlayer(player);
}

void ClientCore::analyseMessage(std::string data)
{
	std::vector<std::string> infos;
	std::vector<std::string> messages = split(data, "#");
 /*
 * T;name
 * B;000111222
 * A;
 * I;id:playerid
 */
	for (int i = 0; i < messages.size(); i++) {
		OutputDebugStringA(("messaga at " + _name + ": " + messages[i] + "\n").c_str());
		switch (messages[i][0]) {
		case 'I': {
			infos = split(messages[i], ";");
			std::vector<std::string> id = split(infos[1], ":");
			_game->setId(stoi(id[1]));
			break;
		} case 'S':
			_game->setStart();
			break;
		case 'B': {
			infos = split(messages[i], ";");
			setGameMap(convertStringBoard(infos[1]));
			break;
		} case 'T': {
			infos = split(messages[i], ";");
			setCurrentPlayer(infos[1]);
			break;
		}
		default:
			break;
		}
	}
}

void ClientCore::sendMessage(std::string mess)
{
	_client->sendData(mess);
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

			if (_name == "") {
				_name = _game->getPlayerName(&event);
				_client->sendData("name:" + _name);
				//("her\n");
			}
			/*if (i == 0)
			{
				_game->initPlayers(event);
				i++;
			}*/

			_game->run(event);
		}

	}

	return (int)msg.wParam;
}