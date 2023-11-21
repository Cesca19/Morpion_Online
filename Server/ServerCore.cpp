#include "ServerCore.h"
#include <fstream>

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
	_server->setCore(this);

	_gameLogic->initGameMap();
	_gameLogic->setCore(this);

	_webServer->setCore(this);
	_hThread = CreateThread(NULL, 0, WebServer::MyThreadFunction, _webServer.get(), 0, &threadId);
	if (_hThread == NULL) {
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
	WaitForSingleObject(_hThread, INFINITE);
}

void ServerCore::addPlayer(std::string name)
{
	std::vector<std::shared_ptr<Player>> playersVect = _server->getPlayersList();
	std::unordered_map<std::string, std::shared_ptr<Player>> players = _server->getPlayers();

	if (players[name]->getType())
		_gameLogic->addPlayer(name, players[name]->getType());
	else
		_gameLogic->addWatcher(name);
}

void ServerCore::sendMessageToPlayers(std::string message)
{
	_server->sendMessageToPlayers(message);
}

void ServerCore::sendMessageToPlayer(std::string name, std::string Message)
{
	_server->sendMessageToPlayer(name, Message);
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
void ServerCore::SetHistoricMsg(std::string mess)
{
	std::string HistoricMsg = "";
	HistoricMsg += mess + "\n";
	std::ofstream HistoricFile;
	HistoricFile.close();
	HistoricFile.open("historic.txt", std::ofstream::app);
	if (HistoricFile.is_open()) {
		HistoricFile << HistoricMsg << std::endl;
		HistoricFile.close();
	}
	else
		OutputDebugString(L"error");
	

}