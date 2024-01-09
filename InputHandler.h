#pragma once
#include "Engine\Core\Window.h"
#include <Mouse.h>
#include <Keyboard.h>
#include "Singleton.h"

enum Button
{
	B_LeftButton,
	B_RightButton,
	B_MiddleButton,
	B_ScrollUp,
	B_ScrollDown
};

class InputHandler : public Singleton<InputHandler>
{
	friend class Singleton<InputHandler>;

private:
	std::unique_ptr<DirectX::Keyboard> keyboard;
	DirectX::Keyboard::State kbState;
	DirectX::Keyboard::KeyboardStateTracker keyboardTracker;
	
	std::unique_ptr<DirectX::Mouse> mouse;
	DirectX::Mouse::State mbState;
	DirectX::Mouse::ButtonStateTracker mouseButtonTracker;
	

public:
	
	void Initialize();
	void HandleInput();

	DirectX::Keyboard* GetKeyboard() { return keyboard.get(); }
	DirectX::Mouse* GetMouse() { return mouse.get(); }

	DirectX::Keyboard::State GetKey() { return keyboardTracker.lastState; }
	DirectX::Keyboard::State GetKeyDown() { return keyboardTracker.pressed; }
	DirectX::Keyboard::State GetKeyUp() { return keyboardTracker.released; }

	bool GetMouseButton(Button button);
	bool GetMouseButtonUp(Button button);
	bool GetMouseButtonDown(Button button);

	bool ScrolledUp();
	bool ScrolledDown();

	DirectX::Mouse::State GetMouseXY() { return mbState; }
};

