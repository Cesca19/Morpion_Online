#include "ClientCore.h"
#include "../Core/Utils.h"

DWORD WINAPI ClientCoreThreadFunction(LPVOID lpParam)
{
	printf("Client Thread is running\n");
	ClientCore* core = static_cast<ClientCore*>(lpParam);
	core->run();
	printf("Finished\n");
	return 0;
}

ClientCore::ClientCore(HINSTANCE hInstance) : 
	_name(""),
	_map(NULL)
{
	_client = new Client(hInstance);
	_game = new Morpion();
}

ClientCore::~ClientCore()
{
	CloseHandle(_hClientCoreThread);
}

int ClientCore::init() {
	_game->setClientCore(this);
	_game->init();
	_client->setCore(this);
	if (_client->init())
		return 1;
	return initClientCoreThread();
}


int ClientCore::initClientCoreThread()
{
	DWORD threadId;
	
	_hClientCoreThread = CreateThread(
		NULL,
		0,
		ClientCoreThreadFunction,
		this,
		0,
		&threadId);

	if (_hClientCoreThread == NULL) {
		OutputDebugStringA(("Error at thread: " + std::to_string(WSAGetLastError())).c_str());
		ExitProcess(3);
	}
	
	WaitForSingleObject(_hClientCoreThread, 1000);
	return 0;
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
 * E;T // tie
 * E;W:name // winner name
 */
	for (int i = 0; i < messages.size(); i++) {
		//OutputDebugStringA(("messaga at " + _name + ": " + messages[i] + "\n").c_str());
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
		} case 'E': {
			infos = split(messages[i], ";");
			if (infos[1][0] == 'T')
				_game->setWinner("", true);
			else {
				std::vector<std::string> win = split(infos[1], ":");
				_game->setWinner(win[1], false);
			}
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
			}

			_game->run(event);
		}
	}

	return (int)msg.wParam;
}

