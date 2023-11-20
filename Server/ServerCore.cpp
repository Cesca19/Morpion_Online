#include "ServerCore.h"

ServerCore::ServerCore(HINSTANCE hInstance, std::string port) : _server(new Server(hInstance, port)),
_gameLogic(new Game()), _numPlayers(0), _webServer(new WebServer(hInstance, "8888")), _hasStart(false)
{
}

ServerCore::~ServerCore()
{
}

int ServerCore::init()
{
	DWORD   threadId;	
	_server->init();
		_hThread = CreateThread(
		NULL, 0, WebServer::MyThreadFunction, _webServer.get(), 0, &threadId);
		if (_hThread == NULL) {
			OutputDebugStringA(("Error at thread: " + std::to_string(WSAGetLastError())).c_str());
			ExitProcess(3);
		}
	_server->setCore(this);
	_gameLogic->initGameMap();
	_gameLogic->setCore(this);
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
	WaitForSingleObject(_hThread, INFINITE);
}

void ServerCore::addPlayer(std::string name)
{
	std::vector<std::shared_ptr<Player>> playersVect =  _server->getPlayersList();

	_numPlayers++;
	if (_numPlayers == 2) {
		_gameLogic->initPlayer(playersVect[0]->getName(), playersVect[1]->getName());
		_hasStart = true;
	}
	if (_hasStart)
		_server->sendMessageToPlayer(name, "S#");
}

void ServerCore::sendMessageToPlayers(std::string message)
{
	_server->sendMessageToPlayers(message);
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