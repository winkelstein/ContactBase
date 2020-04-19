#include "Application.h"

Application::~Application()
{
	Application::uploadToFile("contactBase.db", this->contactBase);
	UnregisterClassA(this->className.c_str(), GetModuleHandleA(nullptr));
}