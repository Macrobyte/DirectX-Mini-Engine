#pragma comment(lib, "dxguid.lib")

#include <iostream>
#include "Game.h"


// Console window for debug output
void OpenConsole();

// Entry point for our windows application
int WINAPI WinMain(
	_In_ HINSTANCE hInstance, // Handle to this instance (our app loaded into memory)
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, // Command line arguments
	_In_ int nCmdShow) // How the window is to be shown (e.g. maximized, minimized, etc.
{
	OpenConsole();

	Game::GetInstance().Initialize(hInstance, nCmdShow);

	// Used to hold windows event messages
	MSG msg;
	
	// Enter main loop
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Translate certain messages into correct format, namely key presses
			TranslateMessage(&msg);

			// Send the message to the WindowProc function
			DispatchMessage(&msg);

			//Break out of the loop if a quite message is detected
			if (msg.message == WM_QUIT)
				break;
		}
		else
		{
			Game::GetInstance().HandleInput();
			Game::GetInstance().Update();
			Game::GetInstance().Render();
		}
	}

	Game::GetInstance().Shutdown();

	return 0;
}


void OpenConsole()
{
	if (AllocConsole())
	{
		// Redirecting input, output and error streams to the console
		FILE* fp = nullptr;
		freopen_s(&fp, "CONIN$", "r", stdin);
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);
		std::ios::sync_with_stdio(true);
	}
}

