#include "ClientCore.h"
#include "../Core/Utils.h"

DWORD WINAPI ClientThreadFunction(LPVOID lpParam)
{
	printf("Client Thread is running\n");
	Client* client = (Client*) lpParam;
	return 0;
}

DWORD WINAPI MorpionThreadFunction(LPVOID lpParam)
{
	printf("Morpion Thread is running\n");
	GameParams* params = (GameParams*) lpParam;

	MSG msg = { 0 };
	sf::Event event;
	int i = 0;
	
	while (msg.message != WM_QUIT && params->game->GetWindow()->isOpen()) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			if (!params->game->GetWindow()->pollEvent(event)) {
				if (event.type == sf::Event::Closed)
					params->game->GetWindow()->close();
			}

			if (params->name == "") {
				params->name = params->game->getPlayerName(&event);
				params->client->sendData("name:" + params->name);
			}

			params->game->run(event);
		}
	}

	return (int)msg.wParam;
}

ClientCore::ClientCore(HINSTANCE hInstance) : 
	_client(new  Client(hInstance, "127.0.0.1", "6666")), _name(""),
	_map(NULL)
{
	_game = new Morpion();
}

ClientCore::~ClientCore()
{
	CloseHandle(_hClientThread);
	CloseHandle(_hMorpionThread);
}

int ClientCore::init() {
	_game->setClientCore(this);
	_game->init();
	_client->setCore(this);
	if (_client->init())
		return 1;
	return 0;
}


int ClientCore::initClientThread()
{
	DWORD threadId;
	
	_hClientThread = CreateThread(
		NULL,
		0,
		ClientThreadFunction,
		_client,
		0,
		&threadId);

	if (_hClientThread == NULL) {
		OutputDebugStringA(("Error at thread: " + std::to_string(WSAGetLastError())).c_str());
		ExitProcess(3);
	}
	
	WaitForSingleObject(_hClientThread, 0);
	return 0;
}

int ClientCore::initMorpionThread()
{
	DWORD threadId;
	_gameParams = new GameParams();
	_gameParams->game = _game;
	_gameParams->client = _client;

	_hMorpionThread = CreateThread(
		NULL,
		0,
		MorpionThreadFunction,
		_gameParams,
		0,
		&threadId);

	if (_hMorpionThread == NULL) {
		OutputDebugStringA(("Error at thread: " + std::to_string(WSAGetLastError())).c_str());
		ExitProcess(3);
	}

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


// int ClientCore::run() {
// 	MSG msg = { 0 };
// 	sf::Event event;
// 	int i = 0;
// 	
// 	while (msg.message != WM_QUIT && _game->GetWindow()->isOpen()) {
// 		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
// 			TranslateMessage(&msg);
// 			DispatchMessage(&msg);
// 		}
// 		else {
// 			if (!_game->GetWindow()->pollEvent(event)) {
// 				if (event.type == sf::Event::Closed)
// 					_game->GetWindow()->close();
// 			}
//
// 			if (_name == "") {
// 				_name = _game->getPlayerName(&event);
// 				_client->sendData("name:" + _name);
// 			}
//
// 			_game->run(event);
// 		}
// 	}
//
// 	return (int)msg.wParam;
// }

