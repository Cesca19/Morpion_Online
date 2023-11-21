#include "ServerCore.h"

ServerCore* ServerCore::_serverCore = nullptr;

ServerCore* ServerCore::getServerCore()
{
	return _serverCore;
}

LRESULT CALLBACK
AppWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return (ServerCore::getServerCore()->wndProc(hwnd, msg, wParam, lParam));
}

ServerCore::ServerCore(HINSTANCE hInstance, std::string port) :
_gameLogic(new Game()), _numPlayers(0), _hasStart(false)
{
	_serverCore = this;
}

ServerCore::~ServerCore()
{
}

void ServerCore::dispatchGameMessage(WPARAM wParam, LPARAM lParam)
{
	int id = (int)lParam;
	Data_t* mess = (Data_t *)wParam;
	std::string receiveMess(mess->content);

	if (std::string("name:") == receiveMess.substr(0, 5)) {
		_playersMap[id]->setName(receiveMess.substr(5, receiveMess.size()));
		_playersNameMap[receiveMess.substr(5, receiveMess.size())] = _playersMap[id];
		addPlayer(receiveMess.substr(5, receiveMess.size()));
	}
	_lastPlayerMessage = receiveMess;
	delete mess;
}

void ServerCore::addNewGameClient(WPARAM wParam, LPARAM lParam)
{
	SOCKET ClientSocket = (SOCKET)wParam;
	int id = (int)lParam;

	PlayerType type = (_playersVect.size() == 0) ? PLAYER1 : (_playersVect.size() == 1) ? PLAYER2 : SPECTATOR;
	std::shared_ptr<Player> player(new Player(id, type));
	_playersVect.push_back(player);
	_playersMap[id] = player;
}

void ServerCore::dispatchWebMessage(WPARAM wParam, LPARAM lParam)
{
	Data_t* mess = (Data_t*)wParam;
	std::string receiveMess(mess->content);
	GameMap_t* map = new GameMap_t;

	map->map = getGameMap();
	PostMessage(_webServerHwnd, SEND_GAME_MAP, (WPARAM)map, lParam);
	delete mess;
}

LRESULT ServerCore::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case GAME_SERVER_ID: {
		setGameServer(wParam, lParam);
		break;
	} case NEW_GAME_CLIENT: {
		addNewGameClient(wParam, lParam);
		break;
	} case (NEW_MESSAGE): {
		dispatchGameMessage(wParam, lParam);
		break;
	} case WEB_SERVER_ID : {
		setWebServer(wParam, lParam);
		break;
	} case NEW_WEB_CLIENT: {
		break;
	} case NEW_WEB_MESSAGE: {
		dispatchWebMessage(wParam, lParam);
		break;
	} case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}

int ServerCore::initWindow()
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
	wcex.lpszClassName = L"App";
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL, L"Call to RegisterClassEx failed!",
			L"Windows Desktop Guided Tour", NULL);
		return 1;
	}
	_hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"App",
		L"App app", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, 100, 100, NULL, NULL, _hInstance, NULL);
	if (!_hwnd) {
		MessageBox(NULL, L"Call to CreateWindow failed!",
			L"Windows Desktop Guided Tour", NULL);
		return 1;
	}
	UpdateWindow(_hwnd);
}

int ServerCore::init()
{
	DWORD   webthreadId;
	DWORD   gamethreadId;

	initWindow();
	_serversThreadArray[0] = CreateThread(NULL, 0, Server::MyThreadFunction, _hwnd, 0, &gamethreadId);
	if (_serversThreadArray[0] == NULL) {
		OutputDebugStringA(("Error at thread: " + std::to_string(WSAGetLastError())).c_str());
		ExitProcess(2);
	}

	_gameLogic->initGameMap();
	_gameLogic->setCore(this);

	_serversThreadArray[1] = CreateThread(NULL, 0, WebServer::MyThreadFunction, _hwnd, 0, &webthreadId);
	if (_serversThreadArray[1] == NULL) {
		OutputDebugStringA(("Error at thread: " + std::to_string(WSAGetLastError())).c_str());
		ExitProcess(3);
	}
	return 0;
}

void ServerCore::update()
{
	MSG msg = { 0 };
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		_gameLogic->run();
	}
	WaitForMultipleObjects(2, _serversThreadArray, true, INFINITE);
}

void ServerCore::addPlayer(std::string name)
{
	if ( _playersNameMap[name]->getType())
		_gameLogic->addPlayer(name, _playersNameMap[name]->getType());
	else
		_gameLogic->addWatcher(name);
}

void ServerCore::sendMessageToPlayers(std::string message)
{
	Data_t* myData = new Data_t;
	myData->content = message;
	PostMessage(_gameServerHwnd, SEND_MESSAGE_TO_PLAYERS, (WPARAM)myData, 0);
}

void ServerCore::sendMessageToPlayer(std::string name, std::string Message)
{
	Data_t* myData = new Data_t;
	myData->content = Message;
	PostMessage(_gameServerHwnd, SEND_MESSAGE_TO_PLAYER, (WPARAM)myData, (WPARAM)_playersNameMap[name]->getId());
}

std::string ServerCore::getPlayerLastMessage()
{
	std::string tmp = _lastPlayerMessage;
	_lastPlayerMessage = "";
	return tmp;
}

void ServerCore::setLastPlayerMessage(std::string mess)
{
	_lastPlayerMessage = mess;
}

int** ServerCore::getGameMap()
{
	return _gameLogic->getGameMap();
}

void ServerCore::setGameServer(WPARAM wParam, LPARAM lParam)
{
	_gameServerHwnd = (HWND)wParam;
}

void ServerCore::setWebServer(WPARAM wParam, LPARAM lParam)
{
	_webServerHwnd = (HWND)wParam;
}