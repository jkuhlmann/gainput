
#include <gainput/gainput.h>

#include "GainputInputDeviceTouchImpl.h"
#include "GainputTouchInfo.h"
#include "../GainputInputDeltaState.h"
#include "../GainputHelpers.h"

#include "GainputInputDeviceTouchNull.h"

#if defined(GAINPUT_PLATFORM_ANDROID)
	#include "GainputInputDeviceTouchAndroid.h"
#endif

namespace gainput
{

InputDeviceTouch::InputDeviceTouch(InputManager& manager, DeviceId device, DeviceVariant variant) :
	InputDevice(manager, device, manager.GetDeviceCountByType(DT_TOUCH))
{
#if defined(GAINPUT_PLATFORM_ANDROID)
	if (variant != DV_NULL)
	{
		impl_ = manager.GetAllocator().New<InputDeviceTouchImplAndroid>(manager, device);
	}
#endif

	if (!impl_)
	{
		impl_ = manager.GetAllocator().New<InputDeviceTouchImplNull>(manager, device);
	}

	GAINPUT_ASSERT(impl_);
	state_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), TouchPointCount*TouchDataElems);
	GAINPUT_ASSERT(state_);
	previousState_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), TouchPointCount*TouchDataElems);
	GAINPUT_ASSERT(previousState_);
}

InputDeviceTouch::~InputDeviceTouch()
{
	manager_.GetAllocator().Delete(state_);
	manager_.GetAllocator().Delete(previousState_);
	manager_.GetAllocator().Delete(impl_);
}

void
InputDeviceTouch::InternalUpdate(InputDeltaState* delta)
{
	impl_->Update(*state_, *previousState_, delta);
}

InputDevice::DeviceState
InputDeviceTouch::InternalGetState() const
{
	return impl_->GetState();
}

InputDevice::DeviceVariant
InputDeviceTouch::GetVariant() const
{
	return impl_->GetVariant();
}

size_t
InputDeviceTouch::GetAnyButtonDown(DeviceButtonSpec* outButtons, size_t maxButtonCount) const
{
	GAINPUT_ASSERT(outButtons);
	GAINPUT_ASSERT(maxButtonCount > 0);
	return CheckAllButtonsDown(outButtons, maxButtonCount, Touch0Down, TouchCount_);
}

size_t
InputDeviceTouch::GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	GAINPUT_ASSERT(buffer);
	GAINPUT_ASSERT(bufferLength > 0);
	strncpy(buffer, deviceButtonInfos[deviceButton].name, bufferLength);
	buffer[bufferLength-1] = 0;
	const size_t nameLen = strlen(deviceButtonInfos[deviceButton].name);
	return nameLen >= bufferLength ? bufferLength : nameLen+1;
}

ButtonType
InputDeviceTouch::GetButtonType(DeviceButtonId deviceButton) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	return deviceButtonInfos[deviceButton].type;
}

DeviceButtonId
InputDeviceTouch::GetButtonByName(const char* name) const
{
	GAINPUT_ASSERT(name);
	for (unsigned i = 0; i < TouchPointCount*TouchDataElems; ++i)
	{
		if (strcmp(name, deviceButtonInfos[i].name) == 0)
		{
			return DeviceButtonId(i);
		}
	}
	return InvalidDeviceButtonId;
}

}

