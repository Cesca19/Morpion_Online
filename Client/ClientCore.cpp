#include "ClientCore.h"
#include "Utils.h"

ClientCore* ClientCore::_clientCore = nullptr;

ClientCore* ClientCore::getClientCore()
{
	return _clientCore;
}

LRESULT CALLBACK
AppWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return (ClientCore::getClientCore()->coreWndProc(hwnd, msg, wParam, lParam));
}

LRESULT ClientCore::coreWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case GAME_CLIENT_ID: 
	{
		setGameClient(wParam);
		break;
	} case NEW_MESSAGE_FROM_SERVER: 
	{
		analyseMessage(wParam);
		break;
	} 
	case DISCONNECT_SERVER:
	{
		_game->GetWindow()->close();
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void ClientCore::analyseMessage(WPARAM wParam)
{
	Data_t* mess = (Data_t*)wParam;
	std::string receiveMess = mess->content;

	analyseMessage(receiveMess);
	delete mess;
}

int ClientCore::initWindow()
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = AppWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = _hInstance;
	wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"ClientApp";
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL, L"Call to RegisterClassEx failed!",
			L"Windows Desktop Guided Tour", NULL);
		return 1;
	}
	_hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"ClientApp",
		L"ClientApp app", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, 100, 100, NULL, NULL, _hInstance, NULL);
	if (!_hwnd) {
		MessageBox(NULL, L"Call to CreateWindow failed!",
			L"Windows Desktop Guided Tour", NULL);
		return 1;
	}
	UpdateWindow(_hwnd);
	return 0;
}

ClientCore::ClientCore() : _game(new Morpion()),
 _name(""), _map(NULL)
{
	_clientCore = this;
}

ClientCore::~ClientCore() {}

int ClientCore::init(std::string windowName, int width, int height) 
{
	DWORD clientThreadId;

	initWindow();

	_game->setCore(this);
	_game->init(windowName, width, height);
	
	_clientThread = CreateThread(NULL, 0, Client::MyThreadFunction, _hwnd, 0, &clientThreadId);
	if (_clientThread == NULL) {
		OutputDebugStringA(("Error at thread: " + std::to_string(WSAGetLastError())).c_str());
		return(1);
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
	std::vector<std::string> messages = split(data, "#");
	nlohmann::json message;

	if (data != "") {
		for (int i = 0; i < messages.size(); i++) {
			if (messages[i] == "start")
				_game->setStart();
			else if (messages[i] != "") {
				message = nlohmann::json::parse(messages[i]);
				if (message["type"].get<std::string>() == "GAME") {
					auto msgData = Protocol::GameProtocol::handleGameStateMessage(message.dump());
					setGameMap(msgData.board);
					setCurrentPlayer(msgData.currentPlayer);
					if (msgData.winner != "") {
						bool isTie = msgData.winner == "T" ? true : false;
						_game->setWinner(msgData.winner, isTie);
					}
				} else if (message["type"].get<std::string>() == "NEW_CLIENT") {
					auto msgData = Protocol::GameProtocol::handleNewClientMessage(message.dump());
					_game->setId(msgData.id);
				}
			}
		}
	}
}

void ClientCore::sendMessage(std::string mess)
{
	Data_t* data = new Data_t;
	data->content = mess;

	PostMessage(_clientHwnd, SEND_MESSAGE_TO_SERVER, (WPARAM)data, 0);
}


int ClientCore::run() 
{
	MSG msg = { 0 };
	sf::Event event;

	while (msg.message != WM_QUIT && _game->GetWindow()->isOpen()) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (!_game->GetWindow()->pollEvent(event))
			if (event.type == sf::Event::Closed)
				_game->GetWindow()->close();
		if (_name == "") {
			Data_t* data = new Data_t;	
			_name = _game->getPlayerName(&event);
			data->content = ("name:" + _name);
			PostMessage(_clientHwnd, SEND_MESSAGE_TO_SERVER, (WPARAM)data, 0);
		}
		_game->run();
	}	
	close();
	return 0;
}

void  ClientCore::setGameClient(WPARAM wParam)
{
	_clientHwnd = (HWND)wParam;
}

int ClientCore::closeClient()
{
	PostMessage(_clientHwnd, DISCONNECT_CLIENT, 0, 0);
	return 0;
}

int ClientCore::close()
{
	closeClient();
	WaitForSingleObject(_clientThread, INFINITE);
	PostMessage(_hwnd, WM_CLOSE, 0, 0);
	OutputDebugStringA("Client core closing ...\n");
	return 0;
}