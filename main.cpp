#include <Windows.h>
#include "Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR nCmdLine, int nShow)
{
	Application app("Contact database", 800, 600);
	app.run();
}