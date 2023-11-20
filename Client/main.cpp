#include "ClientCore.h"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{	
	ClientCore clientCore(hInstance);
	if (clientCore.init() != 0) {
		return 1; 
	}
	
}

