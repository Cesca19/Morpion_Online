#include "ServerCore.h"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	ServerCore server(hInstance, "6666");

	if (server.init())
		return 1;
	server.run();
	return 0;
}