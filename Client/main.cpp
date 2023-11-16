#include "ClientCore.h"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{	
	ClientCore clientCore;	
	Client client(hInstance, "127.0.0.1", "6666");
	clientCore.init("TikTakToe", 1000, 1000, client);
	clientCore.run();
}