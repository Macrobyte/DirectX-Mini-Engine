#include "InputHandler.h"
#include <iostream>

void InputHandler::Initialize()
{
	keyboard = std::make_unique<DirectX::Keyboard>();

	mouse = std::make_unique<DirectX::Mouse>();
	mouse->SetWindow(Window::GetWindowHandle());
	mouse->SetMode(DirectX::Mouse::Mode::MODE_RELATIVE);

	std::cout << "Input Handler Initialized" << std::endl;
}

void InputHandler::HandleInput()
{
	kbState = keyboard->GetState();
	keyboardTracker.Update(kbState); 

	mbState = mouse->GetState();
	mouseButtonTracker.Update(mbState);
}

bool InputHandler::GetMouseButton(Button button)
{
	switch (button)
	{
	case B_LeftButton:
		return mouseButtonTracker.leftButton == mouseButtonTracker.HELD;
	break;

	case B_RightButton:
		return mouseButtonTracker.rightButton == mouseButtonTracker.HELD;
	break;

	case B_MiddleButton:
		return mouseButtonTracker.middleButton == mouseButtonTracker.HELD;
	break;

	default:
		return false;
	break;
	}

}

bool InputHandler::GetMouseButtonUp(Button button)
{
	switch (button)
	{
	case B_LeftButton:
		return mouseButtonTracker.leftButton == mouseButtonTracker.RELEASED;
	break;

	case B_RightButton:
		return mouseButtonTracker.rightButton == mouseButtonTracker.RELEASED;
	break;

	case B_MiddleButton:
		return mouseButtonTracker.middleButton == mouseButtonTracker.RELEASED;
	break;

	default:
		return false;
	break;
	}
}

bool InputHandler::GetMouseButtonDown(Button button)
{
	switch (button)
	{
	case B_LeftButton:
		return mouseButtonTracker.leftButton == mouseButtonTracker.PRESSED;
	break;

	case B_RightButton:
		return mouseButtonTracker.rightButton == mouseButtonTracker.PRESSED;
	break;

	case B_MiddleButton:
		return mouseButtonTracker.middleButton == mouseButtonTracker.PRESSED;
	break;

	default:
		return false;
	break;
	}
}

bool InputHandler::ScrolledUp()
{
	DirectX::Mouse::State mouseState = mouse.get()->GetState();
	bool scrolledUp = (mouseState.scrollWheelValue > 0);
	if (scrolledUp)
	{
		mouse.get()->ResetScrollWheelValue();
	}
	return scrolledUp;
}

bool InputHandler::ScrolledDown()
{
	DirectX::Mouse::State mouseState = mouse.get()->GetState();
	bool scrolledUp = (mouseState.scrollWheelValue < 0);
	if (scrolledUp)
	{
		mouse.get()->ResetScrollWheelValue();
	}
	return scrolledUp;
}
