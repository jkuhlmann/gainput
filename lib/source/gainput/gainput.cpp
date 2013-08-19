
#include <gainput/gainput.h>

/**
\mainpage Gainput Documentation

Gainput is a C++ Open Source input library for games. It collects input data from different devices (like keyboards, mice or gamepads) and makes the inputs accessible through a unified interface. On top of that, inputs can be mapped to user-defined buttons and other advanced features are offered. Gainput aims to be a one-stop solution to acquire input from players for your game.

If there are any problems, please report them on <a href="https://github.com/jkuhlmann/gainput/issues" target="_blank">GitHub</a> or contact: <tt>gainput -a-t- johanneskuhlmann.de</tt>.

These pages are Gainput's API documentation. In order to download Gainput go to the <a href="http://gainput.johanneskuhlmann.de/" target="_blank">Gainput website</a>.

\section contents Contents
- \ref page_start
- \ref page_building
- \ref page_samples
- \ref page_platforms
- \ref page_dependencies
- \ref page_faq

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

manager.SetDisplaySize(width, height);

gainput::InputMap map(manager);
map.MapBool(ButtonConfirm, mouseId, gainput::MouseButtonLeft);

while (game_running)
{
	// Call every frame
	manager.Update();

	manager.HandleMessage(msg); // on Windows only, for message types WM_CHAR, WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, WM_SYSKEYUP, WM_?BUTTON*, WM_MOUSEMOVE, WM_MOUSEWHEEL
	manager.HandleEvent(event); // on Linux only, for event types MotionNotify, ButtonPress, ButtonRelease, KeyPress, KeyRelease

	// Check button state
	if (map.GetBoolWasDown(ButtonConfirm))
	{
		// Confirmed!
	}
}
\endcode


\section license License

Gainput is licensed under the MIT license:

\include "LICENSE"



\page page_start Getting Started
\tableofcontents
This page gives an overview on how to get Gainput into your game and use it for getting your players' input.


\section sect_obtaining Obtaining Gainput
Gainput can be obtained in source from <a href="https://github.com/jkuhlmann/gainput">GitHub</a>.

\section sect_building Building
Build Gainput as described on the \ref page_building page.


\section sect_integrating Integration Into Your Project
To begin with, your project should link to the dynamic or static version of the Gainput library. On Linux, the files are \c libgainput.so (dynamic library) and \c libgainputstatic.so (static library). On Windows, the filenames are \c gainput.lib (used with \c gainput.dll) and \c gainputstatic.lib. In case you decide to use the dynamic library, make sure to distribute the dynamic library together with your executable.

To have the API availale, you have to include Gainput's main header file:

\code
#include <gainput/gainput.h>
\endcode

You should have the \c lib/include/ folder as an include folder in your project settings for this to work. The file includes most of Gainput's other header files so that you shouldn't need to include anything else.


\section sect_setup Setting up Gainput
Gainput's most important class is gainput::InputManager. You should create one that you use throughout your game. Create some input devices using gainput::InputManager::CreateDevice(). And then, during your game loop, call gainput::InputManager::Update() every frame.

Some platform-specific function calls may be necessary, like gainput::InputManager::HandleMessage().

On top of your gainput::InputManager, you should create at least one gainput::InputMap and map some device-specific buttons to your custom user buttons using gainput::InputMap::MapBool() or gainput::InputMap::MapFloat(). This will allow you to more conventienly provide alternative input methods or enable the player to change button mappings.


\section sect_using Using Gainput
After everything has been set up, use gainput::InputMap::GetBool() or gainput::InputMap::GetFloat() (and related functions) anywhere in your game to get player input.




\page page_building Building
Gainput is built using Waf which makes it rather easy to build the library.

Simply run these commands:
-# <tt>waf configure</tt>
-# <tt>waf build_CONFIGNAME</tt>

There are three configurations of which you choose one by substituting CONFIGNAME for one of these:
- \c debug
- \c dev
- \c release

The \c debug configuration supports debugging while the \c release configuration is optimized. The \c dev configuration features a server that external tools can connect to (see \ref page_devprotocol).

Building Gainput as shown above, will build a dynamic-link library, a static-link library, and all samples. The executables can be found in the \c build/CONFIGNAME/ folder.

\section sect_defines Build Configuration Defines
There is a number of defines that determine what is included in the library and how it behaves. Normally, most of these are set by the build scripts or in gainput.h, but it may be necessary to set these when doing custom builds or modifying the build process. All defines must be set during compilation of the library itself.

Name | Description
-----|------------
\c GAINPUT_DEBUG | Enables debugging of the library itself, i.e. enables a lot of internal console output and checks.
\c GAINPUT_DEV | Enables the built-in development tool server that external tools or other Gainput instances can connect to. Automatically enabled in the \c debug and \c dev build configuration.
\c GAINPUT_ENABLE_ALL_GESTURES | Enables all gestures. Note that there is also an individual define for each gesture (see gainput::InputGesture).
\c GAINPUT_ENABLE_RECORDER | Enables recording of inputs.
\c GAINPUT_LIB_BUILD | Should be set if Gainput is being built as library.

\section sect_android_build Android NDK
In order to cross-compile for Android, the build has to be configured differently.

Naturally, the Android NDK must be installed. Replace \c ANDROID_NDK_PATH with the complete absolute path to your installation.

-# <tt>waf configure \-\-cross-android \-\-cross-android-ndk=ANDROID_NDK_PATH</tt>
-# <tt>waf build_CONFIGNAME</tt>

Executing these commands will also yield both a dynamic and static library in the \c build/CONFIGNAME/ folder.

The samples for Android need to be treated a little more to be deployed to a device even though there are some provisions present. For the time being, this is a manual process. If, for example, you want to check out the basic sample, build Gainput as described above and then do:

-# <tt>cp build/debug/samples/basic/libbasicsample.so samples/android/libs/armeabi/libgainputsample.so</tt>
-# <tt>cd samples/android/</tt>
-# <tt>ANDROID_SDK_PATH/tools/android update project -p . -s \-\-name GainputSample \-\-target android-17</tt>
-# <tt>ant debug</tt>
-# <tt>ANDROID_SDK_PATH/platform-tools/adb install bin/GainputSample-debug.apk</tt>

Where ANDROID_SDK_PATH is the path to your Android SDK installation.


\page page_samples Samples Overview

Name | Path | Description
-----|------|------------
Basic Sample | samples/basic/ | Shows the most basic initialization and usage of Gainput. It has separate implementations for all supported platforms.
Dynamic Sample | samples/dynamic/ | Shows how to let the user dynamically change button mappings as well as how to load and save mappings. Uses the \ref sample_fw.
Gesture Sample | samples/gesture/ | Shows how to use input gestures. Also shows how to implement your own custom input device. Uses the \ref sample_fw.
Listener Sample | samples/listener/ | Shows how to use device button listeners as well as user button listeners. Uses the \ref sample_fw.
Recording Sample | samples/recording/ | Shows how to record, play and serialize/deserialize input sequences. Uses the \ref sample_fw.
Sync Sample | samples/sync/ | Shows how to connect two Gainput instances to each other and send the device state over the network. Uses the \ref sample_fw. Works only in the \c dev build configuration.

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
Supported devices: keyboard (standard and raw variants), mouse (standard and raw variants), gamepad.

Evdev is used for the raw input variants. Evdev has permission issues on some Linux distributions where the devices (\c /dev/input/event*) are only readable by root or a specific group. If a raw device's state is gainput::InputDevice::DS_UNAVAILABLE this may very well be the cause.

\section platform_windows Windows
Supported devices: keyboard (standard and raw variants), mouse (standard and raw variants), gamepad.

The gamepad support is implemented using XINPUT which is Microsoft's most current API for such devices. However, that means that only Xbox 360 pads and compatible devices are supported.


\page page_dependencies Dependencies
Gainput has very few external dependencies in order to make it as self-contained as possible. Normally, the only extra piece of software you might have to install is Python. Anything else should come with your IDE (or regular platform SDK).

\section sect_libs Libraries
Most importantly, Gainput does not depend on the STL or any other unnecessary helper libraries. Input is acquired using the following methods:

Android NDK: All input is acquired through the NDK.

Linux:
- the X11 message loop is used for keyboard and mouse
- the kernel's joystick API is used for pads

Windows:
- the Win32 message loop is used for keyboard and mouse
- XINPUT is used for gamepads

\section sect_building Building
Gainput is built using <a href="http://code.google.com/p/waf/" target="_blank">Waf</a> which is written in <a href="http://www.python.org/" target="_blank">Python</a>. Therefore you have to have a recent version of Python installed.



\page page_faq FAQ

\tableofcontents

\section faq0 Why another library when input is included in most engines/libraries?
There are lots of other ways to acquire input, most are part of more complete engines or more comprehensive libraries. For one, Gainput is meant for those who are using something without input capabilities (for example, pure rendering engines) or those who are developing something themselves and want to skip input handling.

In the long run, Gainput aims to be better and offer more advanced features than built-in input solutions. That's the reason why more advanced features, like input recording/playback, remote syncing, gestures and external tool support, are already part of the library.

\section faq1 Why don't you support OS X or iOS?
I would love to. However, I don't have the proper hardware at the moment. The plan is to provide implementations for these platforms eventually. If anyone wants to contribute implementations, they are welcome to do so.


\page page_devprotocol Development Tool Protocol
If Gainput is built with \c GAINPUT_DEV defined, it features a server that external tools can connect to obtain information on devices, mappings and button states. The underlying protocol is TCP/IP and the default port 1211.

The following messages are defined:

\code
hello
{
	uint8_t cmd
	uint32_t protocolVersion
	uint32_t libVersion
}

device
{
	uint8_t cmd
	uint32_t deviceId
	string name
}

device button
{
	uint8_t cmd
	uint32_t deviceId
	uint32_t buttonId
	string name
	uint8_t type
}

map
{
	uint8_t cmd
	uint32_t mapId
	string name
}

remove map
{
	uint8_t cmd
	uint32_t mapId
}

user button
{
	uint8_t cmd
	uint32_t mapId
	uint32_t buttonId
	uint32_t deviceId
	uint32_t deviceButtonId
	float value
}

remove user button
{
	uint8_t cmd
	uint32_t mapId
	uint32_t buttonId
}

user button changed
{
	uint8_t cmd
	uint32_t mapId
	uint32_t buttonId
	uint8_t type
	uint8_t/float value
}

ping
{
	uint8_t cmd
}

get all infos
{
	uint8_t cmd
}

start device sync
{
	uint8_t cmd
	uint8_t deviceType
	uint8_t deviceIndex
}

set device button
{
	uint8_t cmd
	uint8_t deviceType
	uint8_t deviceIndex
	uint32_t deviceButtonId
	uint8_t/float value
}
\endcode

The message IDs (\c cmd) are defined in GainputDevProtocol.h.

All integers are in network byte order.

Strings are represented like this:

\code
{
	uint8_t length
	char text[length] // without the trailing \0
}
\endcode

*/

namespace gainput
{

const char*
GetLibName()
{
	return "Gainput";
}

uint32_t
GetLibVersion()
{
	return ((0 << GAINPUT_VER_MAJOR_SHIFT) | (8) );
}

const char*
GetLibVersionString()
{
	return "0.8.0-beta";
}

}

