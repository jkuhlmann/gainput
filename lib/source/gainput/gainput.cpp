
#include <gainput/gainput.h>

/**
\mainpage Gainput Documentation

Gainput is a C++ Open Source input library for games. It collects input data from different sources (like keyboards, mice or gamepads) and makes the inputs accessible through a unified interface. On top of that, inputs can be mapped to user-defined buttons. Gainput aims to be a one-stop solution to acquire input from players for your game.

If there are any problems, please report them on <a href="https://github.com/jkuhlmann/gainput/issues" target="_blank">GitHub</a> or contact: <tt>gainput -a-t- johanneskuhlmann.de</tt>

These pages are Gainput's API documentation. In order to download Gainput go to the <a href="http://jkuhlmann.github.io/gainput/" target="_blank">Gainput website</a>.

\section contents Contents
- \ref page_building
- \ref page_samples
- \ref page_platforms
- \ref page_dependencies

\section using Using Gainput
A minimal usage sample:

\code
#include <gainput/gainput.h>

// Define your user buttons somewhere global
enum Button
{
	ButtonConfirm
};

// Setting up Gainput
gainput::InputManager manager;
const gainput::DeviceId mouseId = manager.CreateDevice<gainput::InputDeviceMouse>();

manager.SetXDisplay(xDisplay, width, height); // on Linux only
manager.SetDisplaySize(width, height); // on Windows only

gainput::InputMap map(manager);
map.MapBool(ButtonConfirm, mouseId, gainput::MOUSE_BUTTON_LEFT);

while (game_running)
{
	// Call every frame
	manager.Update();

	manager.HandleMessage(msg); // on Windows only, for message types WM_CHAR, WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, WM_SYSKEYUP, WM_?BUTTON*, WM_MOUSEMOVE, WM_MOUSEWHEEL

	// Check button state
	if (map.GetBoolWasDown(ButtonConfirm))
	{
		std::cout << "Confirmed!!" << std::endl;
	}
}
\endcode


\section license License

Gainput is licensed under the MIT license:

\include "LICENSE"


\page page_building Building
Gainput is built using Waf which makes it rather easy to build the library.

Simply run these commands:
-# <tt>waf configure</tt>
-# <tt>waf build_CONFIGNAME</tt>

There are two configurations of which you choose one by substituting CONFIGNAME for \c debug or \c release . The debug configuration supports debugging while the release configuration is more optimized.

Building Gainput as shown above, will build a dynamic-link library, a static-link library, and all samples. The executables can be found in \c build/CONFIGNAME/ folder.


\page page_samples Samples Overview

Name | Path | Description
-----|------|------------
Basic Sample | samples/basic/ | Shows the most basic initialization and usage of Gainput. It has separate implementations for all supported platforms.
Dynamic Sample | samples/dynamic/ | Shows how to let the user dynamically change button mappings. Uses the \ref sample_fw.

\section sample_fw Sample Framework
This framework makes it easier to provide succinct samples by taking care of the platform-dependent window creation/destruction.

In a proper project, everything that is handled by the sample framework should be handled for you by some other library or engine. The sample framework is not meant for production use.



\page page_platforms Platform Notes
\tableofcontents

\section platform_android Android NDK
Supported devices: keyboard, multi-touch, gamepad.

The keyboard support is limited to hardware keyboards, including, for example, hardware home buttons.

The gamepad support is limited to the device-internal sensors, i.e. gyroscope, magnetic field, and acceleration. Externally connected gamepads are not support yet.

\section platform_linux Linux
Supported devices: keyboard, mouse, gamepad.

\section platform_windows Windows
Supported devices: keyboard, mouse, gamepad.

The gamepad support is implemented using XINPUT which is Microsoft's most current API for such devices. However, that means that only Xbox 360 pads and compatible devices are supported.


\page page_dependencies Dependencies
Gainput has very few external dependencies in order to make it as self-contained as possible. Normally, the only extra piece of software you might have to install is Python. Anything else should come with your IDE (or regular platform SDK).

\section sect_libs Libraries
Most importantly, Gainput does not depend on the STL or any other unnecessary helper libraries. Input is acquired using the following methods:

Android NDK: All input is acquired through the NDK.

Linux:
- the X11 message loop for keyboard and mouse
- the kernel's joystick API for pads

Windows:
- the Win32 message loop for keyboard and mouse
- XINPUT for gamepads

\section sect_building Building
Gainput is built using <a href="http://code.google.com/p/waf/" target="_blank">Waf</a> which is written in <a href="http://www.python.org/" target="_blank">Python</a>. Therefore you have to have a recent version of Python installed.


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

