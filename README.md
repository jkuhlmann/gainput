Gainput
=======

Gainput is currently in development and **should NOT be used yet**.

Gainput is the new awesome C++ input library for your game:

- clean, lightweight, and easy to use
- cross-platform: Windows, Linux, Android NDK
- devices: keyboard, mouse, pad, multi-touch
- open source (MIT license)

The generated [API documentation is here](http://jkuhlmann.github.com/gainput/api/).


Usage
-----

```cpp
#include <gainput/gainput.h>

enum Button
{
	ButtonConfirm
};

gainput::InputManager manager;
const gainput::DeviceId mouseId = manager.CreateDevice<gainput::InputDeviceMouse>();
const gainput::DeviceId padId = manager.CreateDevice<gainput::InputDevicePad>();

gainput::InputMap map(manager);
map.MapBool(ButtonConfirm, mouseId, gainput::MOUSE_BUTTON_LEFT);
map.MapBool(ButtonConfirm, padId, gainput::PAD_BUTTON_A);

// May need some more platform-specific setup

// Check button state
if (map.GetBoolWasDown(ButtonConfirm))
{
	// Confirmed!
}
```


Building
--------

By default, Gainput is built using [Waf](http://code.google.com/p/waf/). So make sure you have recent version of [Python](http://www.python.org/) installed.

### Windows & Linux

1. Run `waf configure`
1. Run `waf build_debug` or `waf build_release`
1. The executables should be in `build/debug/` or `build/release/` respectively.

### Android NDK

1. Run `waf configure --cross-android --cross-android-ndk=ANDROID_NDK_PATH`
1. Run `waf build_debug` or `waf build_release`

Replace `ANDROID_NDK_PATH` with your local path.


Dependencies
------------

Gainput has minimal number of external dependencies to make it as self-contained as possible. It doesn't use any STL.

- Windows:
  - the Win32 message loop for keyboard and mouse
  - XINPUT for pads
- Linux:
  - the X11 message loop for keyboard and mouse
  - the kernel's joystick API for pads

[Python](http://www.python.org/) is needed for building.


Alternatives
------------

- [OIS](https://github.com/wgois/Object-oriented-Input-System--OIS-)
- [SDL](http://www.libsdl.org/)


History
-------

*No releases yet.*

