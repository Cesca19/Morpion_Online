#include "ClientCore.h"
#include "Utils.h"

ClientCore::ClientCore(HINSTANCE hInstance) : 
	_name(""),
	_map(NULL)
{
	_game = new Morpion();
}

ClientCore::~ClientCore() {}

int ClientCore::init(std::string windowName, int width, int height) {
	_game->setCore(this);
	_game->init(windowName, width, height);

	DWORD threadId;
	_clientThread = CreateThread(NULL, 0, Client::ClientFunctionThread, _client, 0, &threadId);
	
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
	std::vector<std::string> messages = split(data, "#");
	OutputDebugStringA(("messaga at " + _name + ": " + std::to_string(messages.size()) + "\n").c_str());
	nlohmann::json message;
	OutputDebugStringA(("messaga at " + _name + ": " + data + "\n").c_str());
	if (data != "")
	{
		for (int i = 0; i < messages.size(); i++)
		{
			OutputDebugStringA(("message separated at " + _name + ": " + messages[i] + "\n").c_str());
			if (messages[i] == "start")
			{
				OutputDebugStringA("Start message received \n");
				_game->setStart();
				OutputDebugStringA("game started");
			}
			else if (messages[i] != "") {
				OutputDebugStringA(("je recois un auret message" + messages[i]).c_str());
				
				message = nlohmann::json::parse(messages[i]);
				OutputDebugStringA(("message after parse at " + _name + ": " + message.dump() + "\n").c_str());
				if (message["type"].get<std::string>() == "GAME")
				{
					auto msgData = Protocol::GameProtocol::handleGameStateMessage(message.dump());
					setGameMap(msgData.board);
					setCurrentPlayer(msgData.currentPlayer);

					if (msgData.winner != "")
					{
						bool isTie = msgData.winner == "T" ? true : false;
						_game->setWinner(msgData.winner, isTie);
					}

				}
				else if (message["type"].get<std::string>() == "NEW_CLIENT")
				{
					auto msgData = Protocol::GameProtocol::handleNewClientMessage(message.dump());
					_game->setId(msgData.id);
				}
			}
		}
	}
	
 /*
 * T;name
 * B;000111222
 * A;
 * I;id:playerid
 * E;T // tie
 * E;W:name // winner name
 */

	
	/*for (int i = 0; i < messages.size(); i++) {
		
		switch (messages[i][0]) {
		case 'I': {
			infos = split(messages[i], ";");
			std::vector<std::string> id = split(infos[1], ":");
			_game->setId(stoi(id[1]));
			break;
		} case 'S':
			
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
	}*/
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