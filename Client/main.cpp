#include "ClientCore.h"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{	
	ClientCore clientCore(hInstance);

	clientCore.init("TikTakToe", 800, 600);
	return 0;
}