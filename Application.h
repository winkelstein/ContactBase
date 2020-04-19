#pragma once
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include <fstream>
#include <map>
#include <exception>

typedef std::map<std::string, std::pair<std::string, std::string>> contact; //map of phone number and pair of name, lastname

class Application
{
private:
	//data:
	HWND windowHandler;
	std::string className = "Application";
	bool isWorking;
	unsigned width, height;

	HWND listbox;
	HWND button;
	HWND edit[3];

	contact contactBase;

	//functions:
	void CreateWindowClass();
	void CreateNativeWindow(std::string windowname, unsigned width, unsigned height);
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT WINAPI ApplicationProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void uploadToMap(std::string filepath, contact& map);
	static void uploadToFile(std::string filepath, contact& map);

	static void uploadToListBox(HWND listbox, contact& map);

	//methods:
	void onCreate();
	void onClose();
	void onCommand(HWND hWnd);
public:
	Application(std::string windowname, unsigned width, unsigned height);
	~Application();

	void run();
};

