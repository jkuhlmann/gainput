

#include <gainput/gainput.h>
#include "GainputInputDeltaState.h"

#if defined(GAINPUT_PLATFORM_LINUX)
#include <time.h>
#elif defined(GAINPUT_PLATFORM_WIN)
#include "keyboard/GainputInputDeviceKeyboardWin.h"
#include "mouse/GainputInputDeviceMouseWin.h"
#elif defined(GAINPUT_PLATFORM_ANDROID)
#include <time.h>
#include "keyboard/GainputInputDeviceKeyboardAndroid.h"
#include "touch/GainputInputDeviceTouchAndroid.h"
#endif


namespace gainput
{

InputManager::InputManager(Allocator& allocator) :
		allocator_(allocator),
		devices_(allocator_),
		nextDeviceId_(0),
		listeners_(allocator_),
		nextListenerId_(0),
		deltaState_(allocator_.New<InputDeltaState>(allocator_))
{

}

InputManager::~InputManager()
{
	allocator_.Delete(deltaState_);

	for (DeviceMap::iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
		allocator_.Delete(it->second);
	}
}

void
InputManager::Update()
{
	InputDeltaState* ds = listeners_.empty() ? 0 : deltaState_;

	for (DeviceMap::iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
		it->second->Update(ds);
	}

	if (ds)
	{
		ds->NotifyListeners(listeners_);
		ds->Clear();
	}
}

uint64_t
InputManager::GetTime() const
{
#if defined(GAINPUT_PLATFORM_LINUX) or defined(GAINPUT_PLATFORM_ANDROID)
	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
	{
		return -1;
	}

	uint64_t t = ts.tv_sec*1000ul + ts.tv_nsec/1000000ul;
	return t;
#elif defined(GAINPUT_PLATFORM_WIN)
	static LARGE_INTEGER perfFreq = { 0 };
	if (perfFreq.QuadPart == 0)
	{
		QueryPerformanceFrequency(&perfFreq);
		GAINPUT_ASSERT(perfFreq.QuadPart != 0);
	}
	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	double t = 1000.0 * double(count.QuadPart) / double(perfFreq.QuadPart);
	return t;
#else
#error No time support
#endif
}

unsigned
InputManager::AddListener(InputListener* listener)
{
	listeners_[nextListenerId_] = listener;
	return nextListenerId_++;
}

void
InputManager::RemoveListener(unsigned listenerId)
{
	listeners_.erase(listenerId);
}

size_t
InputManager::GetAnyButtonDown(DeviceButtonSpec* outButtons, size_t maxButtonCount) const
{
	size_t buttonsFound = 0;
	for (DeviceMap::const_iterator it = devices_.begin();
			it != devices_.end() && maxButtonCount > buttonsFound;
			++it)
	{
		buttonsFound += it->second->GetAnyButtonDown(outButtons+buttonsFound, maxButtonCount-buttonsFound);
	}
	return buttonsFound;
}

unsigned
InputManager::GetDeviceCountByType(InputDevice::DeviceType type) const
{
	unsigned count = 0;
	for (DeviceMap::const_iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
		if (it->second->GetType() == type)
		{
			++count;
		}
	}
	return count;
}


#if defined(GAINPUT_PLATFORM_WIN)
void
InputManager::HandleMessage(const MSG& msg)
{
	for (DeviceMap::const_iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
		if (it->second->GetType() == InputDevice::DT_KEYBOARD)
		{
			InputDeviceKeyboard* keyboard = static_cast<InputDeviceKeyboard*>(it->second);
			InputDeviceKeyboardImpl* keyboardImpl = keyboard->GetPimpl();
			GAINPUT_ASSERT(keyboardImpl);
			keyboardImpl->HandleMessage(msg);
		}
		else if (it->second->GetType() == InputDevice::DT_MOUSE)
		{
			InputDeviceMouse* mouse = static_cast<InputDeviceMouse*>(it->second);
			InputDeviceMouseImpl* mouseImpl = mouse->GetPimpl();
			GAINPUT_ASSERT(mouseImpl);
			mouseImpl->HandleMessage(msg);
		}
	}
}
#endif

#if defined(GAINPUT_PLATFORM_ANDROID)
int32_t
InputManager::HandleInput(AInputEvent* event)
{
	int handled = 0;
	for (DeviceMap::const_iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
		if (it->second->GetType() == InputDevice::DT_TOUCH)
		{
			InputDeviceTouch* touch = static_cast<InputDeviceTouch*>(it->second);
			InputDeviceTouchImpl* touchImpl = touch->GetPimpl();
			GAINPUT_ASSERT(touchImpl);
			handled |= touchImpl->HandleInput(event);
		}
		else if (it->second->GetType() == InputDevice::DT_KEYBOARD)
		{
			InputDeviceKeyboard* keyboard = static_cast<InputDeviceKeyboard*>(it->second);
			InputDeviceKeyboardImpl* keyboardImpl = keyboard->GetPimpl();
			GAINPUT_ASSERT(keyboardImpl);
			handled |= keyboardImpl->HandleInput(event);
		}
	}
	return handled;
}
#endif

}

