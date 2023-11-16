#include "GameWindow.h"

HINSTANCE GameWindow::hInstance = NULL;
HWND GameWindow::hWnd = NULL;
const wchar_t* GameWindow::winName = L"Default Window";
int GameWindow::winWidth = 800;
int GameWindow::winHeight = 600;

HRESULT GameWindow::InitWindow(WNDPROC winProc, HINSTANCE instance, int cmdShow, const wchar_t* windowName = L"Default Window", int windowWidth = winWidth, int windowHeight = winHeight)
{
	winName = windowName;

	hInstance = instance; // Store our app handle

	// Create a window class
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX); // Size of the struct
	wc.style = CS_HREDRAW | CS_VREDRAW; // Style of the window
	wc.lpfnWndProc = winProc; // Our window procedure function, make sure it handles window creation or calls DefWindowProc(..)
	wc.hInstance = hInstance; // Give our app handle to the window
	wc.lpszClassName = L"WindowClass1"; // Windows will sotre our window class under this name
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW; // Set the background colour of the window
	// Register the class with above struct. If it fails, if block will execute
	if (!RegisterClassEx(&wc)) 
		return E_FAIL;

	// Adjust the window dimensions so that top window bar is not taking pixels away from our app
	RECT wr = { 0, 0, winWidth, winHeight }; // Set the size, but not the position
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	// Create the window and use the result as the handle
	hWnd = CreateWindowEx(NULL, // Optional window styles
		L"WindowClass1", // Name of our window class
		windowName, // Title of the window
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
		//WS_OVERLAPPDWINDOW, //Alternative windo style that allows resizing
		100,// x-position of the window
		100, // y-position of the window
		wr.right - wr.left, // Width of the window
		wr.bottom - wr.top, // Height of the window
		NULL, // No parent window, null
		NULL, // No menus, null
		hInstance, // App handle
		NULL);

	if (hWnd == NULL) 
		return E_FAIL;

	// Display the window on the screen
	ShowWindow(hWnd, cmdShow);

	return S_OK;
}
