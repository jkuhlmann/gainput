/**
 * Gainput.
 * Copyright (c) 2013 Johannes Kuhlmann.
 * Licensed under the MIT license. See LICENSE file.
 */

#ifndef GAINPUT_H_
#define GAINPUT_H_


#if defined(__linux) || defined(__linux__) || defined(linux) || defined(LINUX)
	#define GAINPUT_PLATFORM_LINUX
	#define GAINPUT_LIBEXPORT
#elif defined(_WIN32) || defined(__WIN32__) || defined(_MSC_VER)
	#define GAINPUT_PLATFORM_WIN
	#if defined(GAINPUT_LIB_BUILD)
		#define GAINPUT_LIBEXPORT		__declspec(dllexport)
	#else
		#define GAINPUT_LIBEXPORT		__declspec(dllimport)
	#endif
	#define _HAS_EXCEPTIONS 0
#else
	#error Gainput: Unknown/unsupported platform!
#endif

#define GAINPUT_ASSERT assert
#define GAINPUT_TEXT_INPUT_QUEUE_LENGTH 32

//#define GAINPUT_DEBUG

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <new>

#if defined(GAINPUT_PLATFORM_LINUX)
	#include <X11/Xlib.h>
	#include <stdint.h>
#elif defined(GAINPUT_PLATFORM_WIN)
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <windows.h>

	typedef unsigned char uint8_t;
	typedef unsigned long uint32_t;
	typedef unsigned __int64 uint64_t;
#endif


/// Contains all Gainput related classes, types, and functions.
namespace gainput
{

/// ID of an input device.
typedef unsigned int DeviceId;
/// ID of a specific button unique to an input device.
typedef unsigned int DeviceButtonId;
/// ID of a user-defined, mapped button.
typedef unsigned int UserButtonId;
/// ID of a registered gesture.
typedef unsigned int GestureId;

/// Returns the name of the library, should be "Gainput".
const char* GetLibName();
/// Returns the version number of the library.
unsigned GetLibVersion();
/// Returns the version number of the library as a printable string.
const char* GetLibVersionString();

class InputDeltaState;
class InputListener;

}


#include <gainput/GainputAllocator.h>
#include <gainput/GainputContainers.h>
#include <gainput/GainputInputState.h>
#include <gainput/GainputInputDevice.h>
#include <gainput/GainputInputGesture.h>
#include <gainput/GainputInputListener.h>
#include <gainput/GainputInputManager.h>
#include <gainput/GainputInputMap.h>

#include <gainput/GainputInputDeviceMouse.h>
#include <gainput/GainputInputDeviceKeyboard.h>
#include <gainput/GainputInputDevicePad.h>

#endif

