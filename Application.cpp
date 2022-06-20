#include "Application.h"

void Application::createWindowClass()
{
	WNDCLASSEXA wcex;

	wcex.cbSize = sizeof(WNDCLASSEXA);
	wcex.lpfnWndProc = Application::WndProc; //this static function will call none-static ApplicationProc class method
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandleA(nullptr);
	wcex.hIcon = LoadIconW(wcex.hInstance, IDI_APPLICATION); //YOUR ICON
	wcex.hIconSm = LoadIconW(wcex.hInstance, IDI_APPLICATION); //SAME BUT SMALLER
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wcex.hCursor = LoadCursorW(wcex.hInstance, IDC_ARROW);
	wcex.lpszClassName = "ApplicationClass";
	wcex.lpszMenuName = nullptr;
	wcex.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassExA(&wcex))
	{
		throw std::exception("Unable to create window class");
	}
}

void Application::createNativeWindow(std::string name, unsigned width, unsigned height)
{
	this->window = CreateWindowA("ApplicationClass", name.c_str(), WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, GetModuleHandleA(nullptr), this);

	if (!this->window)
	{
		throw std::exception("Unable to create window");
		this->~Application();
	}

	ShowWindow(this->window, SW_SHOW);
	UpdateWindow(this->window);
}

LRESULT __stdcall Application::ApplicationProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

void Application::uploadFromDB()
{
	sqlite3_stmt* statement = nullptr;
	sqlite3_prepare_v3(db, "SELECT * FROM contact", -1, 0, &statement, 0);

	while (sqlite3_step(statement) != SQLITE_DONE)
	{
		Contact contact;
		contact.deserialize(statement);
		this->contacts.push_back(contact);
	}
}

void Application::uploadToDB(const Contact& contact)
{
	contact.serialize(this->db);
}

void Application::uploadToListBox()
{
	std::string buffer;

	SendMessageA(listbox, LB_RESETCONTENT, 0, 0);

	for (auto& t : this->contacts)
	{
		buffer = t.phone() + "  " + t.name();
		SendMessageA(listbox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(buffer.c_str()));
	}
}

void Application::onCreate()
{
	this->listbox = CreateWindowA(WC_LISTBOXA, "ListBox", WS_CHILD | WS_VSCROLL | WS_BORDER | WS_VISIBLE | LBS_SORT, 0, 0, this->width / 2, this->height - 39, this->window, nullptr, GetModuleHandleA(nullptr), 0);

	this->button = CreateWindowA(WC_BUTTONA, "Add new contact", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, this->width / 2 + this->width / 7, this->height / 2 + this->height / 4, 150, 20, this->window, reinterpret_cast<HMENU>(0x1), GetModuleHandleA(nullptr), 0);

	this->edit[0] = CreateWindowA(WC_EDITA, "", WS_CHILD | WS_VISIBLE | WS_BORDER, this->width / 2 + this->width / 9, this->height / 6, 200, 20, this->window, 0, GetModuleHandleA(nullptr), 0);
	this->edit[1] = CreateWindowA(WC_EDITA, "", WS_CHILD | WS_VISIBLE | WS_BORDER, this->width / 2 + this->width / 9, this->height / 4, 200, 20, this->window, 0, GetModuleHandleA(nullptr), 0);
	this->edit[2] = CreateWindowA(WC_EDITA, "", WS_CHILD | WS_VISIBLE | WS_BORDER, this->width / 2 + this->width / 9, this->height / 2, 200, 20, this->window, 0, GetModuleHandleA(nullptr), 0);
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

		if (name.empty() || lastname.empty() || number.empty() || a != number.length() - 1 || number[0] != '+') MessageBoxA(this->window, "Enter valid data!", "Some error occured", MB_OK | MB_ICONERROR);
		else if (MessageBoxA(this->window, (name + " " + lastname).c_str(), "Are you really want to add this contact?", MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			for (size_t i = 0; i < name.length(); i++) if (name[i] == ' ') name[i] = '\255';
			for (size_t i = 0; i < lastname.length(); i++) if (lastname[i] == ' ') lastname[i] = '\255';
			
			this->contacts.push_back(Contact(number, name + ' ' + lastname));
			Application::uploadToDB(this->contacts.front());
			Application::uploadToListBox();
			for (unsigned short int i = 0; i < 3; i++) SetWindowTextA(this->edit[i], "");
		}
	}
}

LRESULT __stdcall Application::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Application* pApp;
	if (msg == WM_NCCREATE)
	{
		pApp = static_cast<Application*>(reinterpret_cast<CREATESTRUCTA*>(lParam)->lpCreateParams);

		if (!SetWindowLongPtrA(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pApp)))
		{
			if (GetLastError()) MessageBoxA(nullptr, "SetWindowLongPtrA error", "Some error occured", MB_OK);
		}
	}
	else
	{
		pApp = reinterpret_cast<Application*>(GetWindowLongPtrA(hWnd, GWLP_USERDATA));
	}

	if (pApp)
	{
		pApp->window = hWnd;
		return pApp->ApplicationProc(hWnd, msg, wParam, lParam);
	}

	return DefWindowProcA(hWnd, msg, wParam, lParam);
}

Application::Application(std::string window_name, unsigned width, unsigned height) : width(width), height(height)
{
	try
	{
		this->createWindowClass();
		this->createNativeWindow(window_name, width, height);
	}
	catch (std::exception& e)
	{
		MessageBoxA(NULL, e.what(), "Some error occured", MB_OK);
		this->~Application();
	}

	if (sqlite3_open("database.db", &this->db))
		this->~Application();

	std::string query = "CREATE TABLE IF NOT EXISTS contact(phone VARCHAR, name VARCHAR);";

	if (sqlite3_exec(this->db, query.c_str(), 0, 0, 0))
		this->~Application();

	this->uploadFromDB();
	this->uploadToListBox();
}

Application::~Application()
{
	sqlite3_close(this->db);
	UnregisterClassA("ApplicationClass", GetModuleHandleA(nullptr));
}

void Application::run()
{
	MSG msg;
	this->isWorking = true;

	while (this->isWorking)
	{
		if (PeekMessageA(&msg, this->window, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}
}
