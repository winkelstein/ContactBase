#include "Application.h"

Application::Application(std::string windowname, unsigned width, unsigned height)
{
	this->width = width;
	this->height = height;

	try
	{
		this->CreateWindowClass();
		this->CreateNativeWindow(windowname, this->width, this->height);
	}
	catch (std::exception& e)
	{
		MessageBoxA(NULL, e.what(), "Some error occured", MB_OK);
		this->~Application();
	}
}

void Application::CreateWindowClass()
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
	wcex.lpszClassName = this->className.c_str();
	wcex.lpszMenuName = nullptr;
	wcex.style = CS_VREDRAW | CS_HREDRAW;
	
	if (!RegisterClassExA(&wcex))
	{
		throw std::exception("Unable to create window class");
	}
}

void Application::CreateNativeWindow(std::string windowname, unsigned width, unsigned height)
{
	this->windowHandler = CreateWindowA(this->className.c_str(), windowname.c_str(), WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, GetModuleHandleA(nullptr), this);

	if (!this->windowHandler)
	{
		throw std::exception("Unable to create window");
		this->~Application();
	}

	ShowWindow(this->windowHandler, SW_SHOW);
	UpdateWindow(this->windowHandler);
}

LRESULT WINAPI Application::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
		pApp->windowHandler = hWnd;
		return pApp->ApplicationProc(hWnd, msg, wParam, lParam);
	}

	return DefWindowProcA(hWnd, msg, wParam, lParam);
}

void Application::run()
{
	MSG msg;
	this->isWorking = true;

	while (this->isWorking)
	{
		if (PeekMessageA(&msg, this->windowHandler, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}
}

void Application::uploadToMap(std::string filepath, contact& map)
{
	std::ifstream file(filepath);

	std::string phonenumber, name, lastname;

	while (file >> phonenumber)
	{
		file >> name;
		file >> lastname;

		map[phonenumber] = std::pair<std::string, std::string>(name, lastname);
	}
}

void Application::uploadToFile(std::string filepath, contact& map)
{
	std::ofstream file(filepath);

	for (auto& t : map)
	{
		file << t.first << " " << t.second.first << " " << t.second.second << '\n';
	}
}

void Application::uploadToListBox(HWND listbox, contact& map)
{
	std::string buffer;

	SendMessageA(listbox, LB_RESETCONTENT, 0, 0);

	for (auto& t : map)
	{
		buffer = t.second.first + " " + t.second.second + "      " +t.first;

		SendMessageA(listbox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(buffer.c_str()));
	}
}
