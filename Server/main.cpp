
#include "pch.h"
#include "ServerCore.h"

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
	ServerCore serverApp(hInstance, "6666");

	//if (serverApp.init())
	//	return 1;
	serverApp.run();
	return 0;
}