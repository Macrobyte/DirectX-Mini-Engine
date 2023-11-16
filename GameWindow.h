#pragma once
#include <Windows.h>
#include <string>


class GameWindow
{
private:
	static HINSTANCE hInstance;
	static HWND hWnd;
	static const wchar_t* winName;
	static int winWidth;
	static int winHeight;

public:
	static HRESULT InitWindow(WNDPROC winProc, HINSTANCE instance , int cmdShow, const wchar_t* windowName, int windowWidth, int windowHeight);

	static HWND GetWindowHandle() { return hWnd; }

	static int GetWindowWidth() { return winWidth; }
	static int GetWindowHeight() { return winHeight; }
};

