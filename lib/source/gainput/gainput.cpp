
#include <gainput/gainput.h>

/**
\mainpage Gainput Documentation

Gainput is an input library for games. This is its API documentation.

\section using Using Gainput

\code
// Don't forget to include
#include <gainput/gainput.h>

// Define your user buttons somewhere global
enum Button
{
	ButtonConfirm
};

// Setting up Gainput
gainput::InputManager manager;
const gainput::DeviceId mouseId = manager.CreateDevice<gainput::InputDeviceMouse>();

// On Linux only:
manager.SetXDisplay(xDisplay, width, height);
// On Windows only:
manager.SetDisplaySize(width, height);

gainput::InputMap map(manager);
map.MapBool(ButtonConfirm, mouseId, gainput::MOUSE_BUTTON_LEFT);

// Call every frame
manager.Update();

// On Windows only, for message types WM_CHAR, WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, WM_SYSKEYUP, WM_?BUTTON*, WM_MOUSEMOVE, WM_MOUSEWHEEL:
manager.HandleMessage(msg);

// Check button state
if (map.GetBoolWasDown(ButtonConfirm))
{
	std::cout << "Confirmed!!" << std::endl;
}
\endcode


\section license License

Gainput is licensed under the MIT license:

\include "LICENSE"

*/

namespace gainput
{

const char*
GetLibName()
{
	return "Gainput";
}

unsigned
GetLibVersion()
{
	return 0;
}

const char*
GetLibVersionString()
{
	return "0";
}

}

