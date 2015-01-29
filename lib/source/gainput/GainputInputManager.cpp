

#include <gainput/gainput.h>
#include "GainputInputDeltaState.h"

#if defined(GAINPUT_PLATFORM_LINUX)
#include <time.h>
#include <X11/Xlib.h>
#include "keyboard/GainputInputDeviceKeyboardLinux.h"
#include "mouse/GainputInputDeviceMouseLinux.h"
#elif defined(GAINPUT_PLATFORM_WIN)
#include "keyboard/GainputInputDeviceKeyboardWin.h"
#include "keyboard/GainputInputDeviceKeyboardWinRaw.h"
#include "mouse/GainputInputDeviceMouseWin.h"
#include "mouse/GainputInputDeviceMouseWinRaw.h"
#elif defined(GAINPUT_PLATFORM_ANDROID)
#include <time.h>
#include "keyboard/GainputInputDeviceKeyboardAndroid.h"
#include "touch/GainputInputDeviceTouchAndroid.h"
#endif

#include "dev/GainputDev.h"


namespace gainput
{

InputManager::InputManager(Allocator& allocator) :
		allocator_(allocator),
		devices_(allocator_),
		nextDeviceId_(0),
		listeners_(allocator_),
		nextListenerId_(0),
		modifiers_(allocator_),
		nextModifierId_(0),
		deltaState_(allocator_.New<InputDeltaState>(allocator_))
{
	GAINPUT_DEV_INIT(this);
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

	GAINPUT_DEV_SHUTDOWN(this);
}

void
InputManager::Update()
{
	InputDeltaState* ds = listeners_.empty() ? 0 : deltaState_;

	for (DeviceMap::iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
		if (!it->second->IsLateUpdate())
		{
			it->second->Update(ds);
		}
	}

	GAINPUT_DEV_UPDATE(ds);

	for (HashMap<ModifierId, DeviceStateModifier*>::iterator it = modifiers_.begin();
			it != modifiers_.end();
			++it)
	{
		it->second->Update(ds);
	}

	for (DeviceMap::iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
		if (it->second->IsLateUpdate())
		{
			it->second->Update(ds);
		}
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
#if defined(GAINPUT_PLATFORM_LINUX) || defined(GAINPUT_PLATFORM_ANDROID)
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
	return static_cast<uint64_t>(t);
#else
#error No time support
#endif
}

DeviceId
InputManager::FindDeviceId(const char* typeName, unsigned index) const
{
	for (DeviceMap::const_iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
		if (strcmp(typeName, it->second->GetTypeName()) == 0
			&& it->second->GetIndex() == index)
		{
			return it->first;
		}
	}
	return InvalidDeviceId;
}

DeviceId
InputManager::FindDeviceId(InputDevice::DeviceType type, unsigned index) const
{
	for (DeviceMap::const_iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
		if (it->second->GetType() == type
			&& it->second->GetIndex() == index)
		{
			return it->first;
		}
	}
	return InvalidDeviceId;
}

ListenerId
InputManager::AddListener(InputListener* listener)
{
	listeners_[nextListenerId_] = listener;
	return nextListenerId_++;
}

void
InputManager::RemoveListener(ListenerId listenerId)
{
	listeners_.erase(listenerId);
}

ModifierId
InputManager::AddDeviceStateModifier(DeviceStateModifier* modifier)
{
	modifiers_[nextModifierId_] = modifier;
	return nextModifierId_++;
}

void
InputManager::RemoveDeviceStateModifier(ModifierId modifierId)
{
	modifiers_.erase(modifierId);
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

void
InputManager::DeviceCreated(InputDevice* device)
{
	GAINPUT_DEV_NEW_DEVICE(device);
}

#if defined(GAINPUT_PLATFORM_LINUX)
void
InputManager::HandleEvent(XEvent& event)
{
	for (DeviceMap::const_iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
#if defined(GAINPUT_DEV)
		if (it->second->IsSynced())
		{
			continue;
		}
#endif
		if (it->second->GetType() == InputDevice::DT_KEYBOARD
			&& it->second->GetVariant() == InputDevice::DV_STANDARD)
		{
			InputDeviceKeyboard* keyboard = static_cast<InputDeviceKeyboard*>(it->second);
			InputDeviceKeyboardImplLinux* keyboardImpl = static_cast<InputDeviceKeyboardImplLinux*>(keyboard->GetPimpl());
			GAINPUT_ASSERT(keyboardImpl);
			keyboardImpl->HandleEvent(event);
		}
		else if (it->second->GetType() == InputDevice::DT_MOUSE
			&& it->second->GetVariant() == InputDevice::DV_STANDARD)
		{
			InputDeviceMouse* mouse = static_cast<InputDeviceMouse*>(it->second);
			InputDeviceMouseImplLinux* mouseImpl = static_cast<InputDeviceMouseImplLinux*>(mouse->GetPimpl());
			GAINPUT_ASSERT(mouseImpl);
			mouseImpl->HandleEvent(event);
		}
	}
}
#endif

#if defined(GAINPUT_PLATFORM_WIN)
void
InputManager::HandleMessage(const MSG& msg)
{
	for (DeviceMap::const_iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
#if defined(GAINPUT_DEV)
		if (it->second->IsSynced())
		{
			continue;
		}
#endif
		if (it->second->GetType() == InputDevice::DT_KEYBOARD)
		{
			InputDeviceKeyboard* keyboard = static_cast<InputDeviceKeyboard*>(it->second);
			if (it->second->GetVariant() == InputDevice::DV_STANDARD)
			{
				InputDeviceKeyboardImplWin* keyboardImpl = static_cast<InputDeviceKeyboardImplWin*>(keyboard->GetPimpl());
				GAINPUT_ASSERT(keyboardImpl);
				keyboardImpl->HandleMessage(msg);
			}
			else if (it->second->GetVariant() == InputDevice::DV_RAW)
			{
				InputDeviceKeyboardImplWinRaw* keyboardImpl = static_cast<InputDeviceKeyboardImplWinRaw*>(keyboard->GetPimpl());
				GAINPUT_ASSERT(keyboardImpl);
				keyboardImpl->HandleMessage(msg);
			}
		}
		else if (it->second->GetType() == InputDevice::DT_MOUSE)
		{
			InputDeviceMouse* mouse = static_cast<InputDeviceMouse*>(it->second);
			if (it->second->GetVariant() == InputDevice::DV_STANDARD)
			{
				InputDeviceMouseImplWin* mouseImpl = static_cast<InputDeviceMouseImplWin*>(mouse->GetPimpl());
				GAINPUT_ASSERT(mouseImpl);
				mouseImpl->HandleMessage(msg);
			}
			else if (it->second->GetVariant() == InputDevice::DV_RAW)
			{
				InputDeviceMouseImplWinRaw* mouseImpl = static_cast<InputDeviceMouseImplWinRaw*>(mouse->GetPimpl());
				GAINPUT_ASSERT(mouseImpl);
				mouseImpl->HandleMessage(msg);
			}
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
#if defined(GAINPUT_DEV)
		if (it->second->IsSynced())
		{
			continue;
		}
#endif
		if (it->second->GetType() == InputDevice::DT_TOUCH)
		{
			InputDeviceTouch* touch = static_cast<InputDeviceTouch*>(it->second);
			InputDeviceTouchImplAndroid* touchImpl = static_cast<InputDeviceTouchImplAndroid*>(touch->GetPimpl());
			GAINPUT_ASSERT(touchImpl);
			handled |= touchImpl->HandleInput(event);
		}
		else if (it->second->GetType() == InputDevice::DT_KEYBOARD)
		{
			InputDeviceKeyboard* keyboard = static_cast<InputDeviceKeyboard*>(it->second);
			InputDeviceKeyboardImplAndroid* keyboardImpl = static_cast<InputDeviceKeyboardImplAndroid*>(keyboard->GetPimpl());
			GAINPUT_ASSERT(keyboardImpl);
			handled |= keyboardImpl->HandleInput(event);
		}
	}
	return handled;
}
#endif

void
InputManager::ConnectForStateSync(const char* ip, unsigned port)
{
	GAINPUT_DEV_CONNECT(this, ip, port);
}

void
InputManager::StartDeviceStateSync(DeviceId deviceId)
{
	GAINPUT_ASSERT(GetDevice(deviceId));
	GAINPUT_ASSERT(GetDevice(deviceId)->GetType() != InputDevice::DT_GESTURE);
	GAINPUT_DEV_START_SYNC(deviceId);
}

}

