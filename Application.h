#pragma once
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <string>
#include <fstream>
#include <vector>
#include <exception>

#include <Windows.h>
#include <CommCtrl.h>

#include <sqlite/sqlite3.h>

#include "Contact.h"

class Application
{
private:
	HWND window;
	bool isWorking;
	unsigned width, height;

	HWND listbox;
	HWND button;
	HWND edit[3];

	sqlite3* db;

	std::vector<Contact> contacts;

private:
	void createWindowClass();
	void createNativeWindow(std::string name, unsigned width, unsigned height);
	LRESULT WINAPI ApplicationProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void uploadFromDB();
	void uploadToDB(const Contact& contact);
	void uploadToListBox();


	void onCreate();
	void onClose();
	void onCommand(HWND hWnd);

private:
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	Application(std::string window_name, unsigned width, unsigned height);
	~Application();

	void run();
};