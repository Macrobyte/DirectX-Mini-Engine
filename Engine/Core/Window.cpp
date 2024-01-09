#include "Window.h"
#include "../../InputHandler.h"
#include "../../Renderer.h"

HINSTANCE Window::hInstance = NULL;
HWND Window::hWnd = NULL;
const wchar_t* Window::winName = L"Default Window";
int Window::winWidth = 0;
int Window::winHeight = 0;

HRESULT Window::Initialize(WNDPROC winProc, HINSTANCE instance, int cmdShow, const wchar_t* windowName = L"Default Window", int windowWidth = 800, int windowHeight = 600)
{
	winName = windowName;

	hInstance = instance; // Store our app handle

	winWidth = windowWidth;

	winHeight = windowHeight;

	// Create a window class
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX); // Size of the struct
	wc.style = CS_HREDRAW | CS_VREDRAW; // Style of the window
	wc.lpfnWndProc = winProc; // Our window procedure function, make sure it handles window creation or calls DefWindowProc(..)
	wc.hInstance = instance; // Give our app handle to the window
	wc.lpszClassName = L"WindowClass1"; // Windows will sotre our window class under this name
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW; // Set the background colour of the window
	// Register the class with above struct. If it fails, if block will execute
	if (!RegisterClassEx(&wc)) 
		return E_FAIL;

	// Adjust the window dimensions so that top window bar is not taking pixels away from our app
	RECT wr = { 0, 0, windowWidth, windowHeight }; // Set the size, but not the position
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	// Create the window and use the result as the handle
	hWnd = CreateWindowEx(NULL, // Optional window styles
		L"WindowClass1", // Name of our window class
		windowName, // Title of the window
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
		//WS_OVERLAPPDWINDOW, //Alternative windo style that allows resizing
		0,// x-position of the window
		0, // y-position of the window
		wr.right - wr.left, // Width of the window
		wr.bottom - wr.top, // Height of the window
		NULL, // No parent window, null
		NULL, // No menus, null
		instance, // App handle
		NULL);

	if (hWnd == NULL) 
		return E_FAIL;

	// Display the window on the screen
	ShowWindow(hWnd, cmdShow);

	return S_OK;
}

LRESULT Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	case WM_DESTROY: // This message is sent when the user closes the window
		PostQuitMessage(0);
		return 0;
		break;

	case WM_ACTIVATE:
	case WM_ACTIVATEAPP:
	case WM_INPUT:
		InputHandler::GetInstance().GetKeyboard()->ProcessMessage(message, wParam, lParam);
		InputHandler::GetInstance().GetMouse()->ProcessMessage(message, wParam, lParam);
		break;

	case WM_SYSKEYDOWN:
		if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
		{
			Renderer::GetInstance().ToggleFullscreen();
				
		}
		InputHandler::GetInstance().GetKeyboard()->ProcessMessage(message, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		InputHandler::GetInstance().GetKeyboard()->ProcessMessage(message, wParam, lParam);
		break;


	case WM_MOUSEACTIVATE:
		return MA_ACTIVATEANDEAT; // This will ignore mouse clicks that regain focus on the window
		break;

	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
		InputHandler::GetInstance().GetMouse()->ProcessMessage(message, wParam, lParam);
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		InputHandler::GetInstance().GetMouse()->ProcessMessage(message, wParam, lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

