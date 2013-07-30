/**
 * Gainput.
 * Copyright (c) 2013 Johannes Kuhlmann.
 * Licensed under the MIT license. See LICENSE file.
 */

#ifndef GAINPUT_H_
#define GAINPUT_H_

#if defined(__ANDROID__) || defined(ANDROID)
	#define GAINPUT_PLATFORM_ANDROID
	#define GAINPUT_LIBEXPORT
#elif defined(__linux) || defined(__linux__) || defined(linux) || defined(LINUX)
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


//#define GAINPUT_DEBUG
//#define GAINPUT_DEV
#define GAINPUT_ENABLE_ALL_GESTURES
#define GAINPUT_ENABLE_RECORDER

//#define GAINPUT_ENABLE_RAW_INPUT


#include <cassert>
#include <cstring>
#include <new>


#define GAINPUT_ASSERT assert
#define GAINPUT_TEXT_INPUT_QUEUE_LENGTH 32


#if defined(GAINPUT_PLATFORM_LINUX)

#include <cstdlib>
#include <X11/Xlib.h>
#include <stdint.h>

#if defined(GAINPUT_DEBUG) || defined(GAINPUT_DEV)
	#include <stdio.h>
	#define GAINPUT_LOG(...) printf(__VA_ARGS__);
#endif

#elif defined(GAINPUT_PLATFORM_WIN)

#include <cstdlib>

#ifndef GAINPUT_NO_WINDOWS_H
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <windows.h>
#endif

namespace gainput
{
	typedef unsigned __int8 uint8_t;
	typedef __int8 int8_t;
	typedef unsigned __int32 uint32_t;
	typedef unsigned __int64 uint64_t;
}

#if defined(GAINPUT_DEBUG) || defined(GAINPUT_DEV)
	#include <stdio.h>
	#define GAINPUT_LOG(...) { char buf[1024]; sprintf(buf, __VA_ARGS__); OutputDebugStringA(buf); }
#endif

#elif defined(GAINPUT_PLATFORM_ANDROID)

#include <stdint.h>
#include <android/sensor.h>
#include <android/native_activity.h>
#include <android/log.h>

#if defined(GAINPUT_DEBUG) || defined(GAINPUT_DEV)
	#define GAINPUT_LOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, "gainput", __VA_ARGS__))
#endif

#endif

#ifndef GAINPUT_LOG
#define GAINPUT_LOG(...)
#endif


/// Contains all Gainput related classes, types, and functions.
namespace gainput
{

/// ID of an input device.
typedef unsigned int DeviceId;
/// ID of a specific button unique to an input device.
typedef unsigned int DeviceButtonId;

/// Describes a device button on a specific device.
struct DeviceButtonSpec
{
	/// ID of the input device.
	DeviceId deviceId;
	/// ID of the button on the given input device.
	DeviceButtonId buttonId;
};

/// ID of a user-defined, mapped button.
typedef unsigned int UserButtonId;
/// ID of an input listener.
typedef unsigned int ListenerId;
/// ID of a device state modifier.
typedef unsigned int ModifierId;

/// An invalid device ID.
static const DeviceId InvalidDeviceId = -1;
/// An invalid device button ID.
static const DeviceButtonId InvalidDeviceButtonId = -1;
/// An invalid user button ID.
static const UserButtonId InvalidUserButtonId = -1;

/// Returns the name of the library, should be "Gainput".
const char* GetLibName();
/// Returns the version number of the library.
uint32_t GetLibVersion();
/// Returns the version number of the library as a printable string.
const char* GetLibVersionString();

class InputDeltaState;
class InputListener;
class InputManager;
class DeviceStateModifier;

template <class T> T Abs(T a) { return a < T() ? -a : a; }
}

#define GAINPUT_VER_MAJOR_SHIFT		16
#define GAINPUT_VER_GET_MAJOR(ver)	(ver >> GAINPUT_VER_MAJOR_SHIFT)
#define GAINPUT_VER_GET_MINOR(ver)	(ver & (uint32_t(-1) >> GAINPUT_VER_MAJOR_SHIFT))


#include <gainput/GainputAllocator.h>
#include <gainput/GainputContainers.h>
#include <gainput/GainputInputState.h>
#include <gainput/GainputInputDevice.h>
#include <gainput/GainputInputListener.h>
#include <gainput/GainputInputManager.h>
#include <gainput/GainputInputMap.h>

#include <gainput/GainputInputDeviceMouse.h>
#include <gainput/GainputInputDeviceKeyboard.h>
#include <gainput/GainputInputDevicePad.h>
#include <gainput/GainputInputDeviceTouch.h>

#include <gainput/gestures/GainputGestures.h>

#include <gainput/recorder/GainputInputRecording.h>
#include <gainput/recorder/GainputInputPlayer.h>
#include <gainput/recorder/GainputInputRecorder.h>


#endif

