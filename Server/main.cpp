
#include "pch.h"
#include <iostream>
#include <string>
#include "ServerCore.h"
#include "WebServer.h"

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
	/*WebServer webServer(hInstance, "8888");
	if(webServer.init())
		return 1;
	webServer.run();*/
	ServerCore serverApp(hInstance, "6666");

	if (serverApp.init())
		return 1;
	serverApp.update();
	return 0;
}