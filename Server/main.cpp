
#include "pch.h"
#include "ServerCore.h"

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
	ServerCore serverApp(hInstance);
	serverApp.run();
	return 0;
}