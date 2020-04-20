#include "Application.h"

LRESULT WINAPI Application::ApplicationProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		this->onCreate();
		break;
	case WM_COMMAND:
		this->onCommand(reinterpret_cast<HWND>(lParam));
		break;
	case WM_DESTROY:
		this->onClose();
		break;
	}

	return DefWindowProcA(hWnd, message, wParam, lParam);
}

void Application::onCreate()
{
	this->listbox = CreateWindowA(WC_LISTBOXA, "ListBox", WS_CHILD | WS_VSCROLL | WS_BORDER | WS_VISIBLE | LBS_SORT, 0, 0, this->width/2, this->height - 39, this->windowHandler, nullptr, GetModuleHandleA(nullptr), 0);

	this->button = CreateWindowA(WC_BUTTONA, "Add new contact", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, this->width / 2 + this->width / 7, this->height / 2 + this->height / 4, 150, 20, this->windowHandler, reinterpret_cast<HMENU>(0x1), GetModuleHandleA(nullptr), 0);
	this->delbutton = CreateWindowA(WC_BUTTONA, "Delete contact", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, this->width / 2 + this->width / 7, this->height / 2 + this->height / 3, 150, 20, this->windowHandler, reinterpret_cast<HMENU>(0x2), GetModuleHandleA(nullptr), 0);

	this->edit[0] = CreateWindowA(WC_EDITA, "", WS_CHILD | WS_VISIBLE | WS_BORDER, this->width / 2 + this->width / 9, this->height / 6, 200, 20, this->windowHandler, 0, GetModuleHandleA(nullptr), 0);
	this->edit[1] = CreateWindowA(WC_EDITA, "", WS_CHILD | WS_VISIBLE | WS_BORDER, this->width / 2 + this->width / 9, this->height / 4, 200, 20, this->windowHandler, 0, GetModuleHandleA(nullptr), 0);
	this->edit[2] = CreateWindowA(WC_EDITA, "", WS_CHILD | WS_VISIBLE | WS_BORDER, this->width / 2 + this->width / 9, this->height / 2, 200, 20, this->windowHandler, 0, GetModuleHandleA(nullptr), 0);

	Application::uploadToMap("contactBase.db", this->contactBase);
	Application::uploadToListBox(this->listbox, this->contactBase);
}

void Application::onClose()
{
	this->isWorking = false;
	PostQuitMessage(0);
}

void Application::onCommand(HWND hWnd)
{
	if (hWnd == this->button)
	{
		std::string name, lastname, number;
		char buffer[64];

		GetWindowTextA(this->edit[0], buffer, 64);
		name = buffer;
		GetWindowTextA(this->edit[1], buffer, 64);
		lastname = buffer;
		GetWindowTextA(this->edit[2], buffer, 64);
		number = buffer;

		unsigned short a = false;
		for (unsigned short i = 0; i < number.length(); i++)
		{
			if (number[i] == '+') continue;

			if (!isdigit(number[i])) break;
			else a++;
		}

		if (name.empty() || lastname.empty() || number.empty() || a != number.length()-1 || number[0] != '+') MessageBoxA(this->windowHandler, "Enter valid data!", "Some error occured", MB_OK | MB_ICONERROR);
		else if (MessageBoxA(this->windowHandler, (name + " " + lastname).c_str(), "Are you really want to add this contact?", MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			for (size_t i = 0; i < name.length(); i++) if (name[i] == ' ') name[i] = '\255';
			for (size_t i = 0; i < lastname.length(); i++) if (lastname[i] == ' ') lastname[i] = '\255';
			this->contactBase[number] = std::pair<std::string, std::string>(name, lastname);
			Application::uploadToFile("contactBase.db", this->contactBase);
			Application::uploadToListBox(this->listbox, this->contactBase);
			for (unsigned short int i = 0; i < 3; i++) SetWindowTextA(this->edit[i], "");
		}
	}

	if (hWnd == this->delbutton)
	{
		std::string number;
		DWORD index = SendMessageA(this->listbox, LB_GETCURSEL, 0, 0);
		
		if ((long)index >= 0 && MessageBoxA(this->windowHandler, "Are you really want to delete selected contact?", "Delete contact", MB_YESNO) == IDYES)
		{
			char buffer[64];
			SendMessageA(this->listbox, LB_GETTEXT, index, (LPARAM)buffer);
			number = buffer;
			
			number = number.substr(number.find('+'));

			this->contactBase.erase(number);

			SendMessageA(this->listbox, LB_DELETESTRING, index, 0);
		}
	}
}
