
#ifndef GAINPUTMOUSECOMMON_H_
#define GAINPUTMOUSECOMMON_H_

namespace gainput
{

InputDeviceMouse::InputDeviceMouse(InputManager& manager, DeviceId device) :
	InputDevice(manager, device, manager.GetDeviceCountByType(DT_MOUSE))
{
	impl_ = manager.GetAllocator().New<InputDeviceMouseImpl>(manager, device);
	GAINPUT_ASSERT(impl_);
	state_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), MouseButtonCount + MouseAxisCount);
	GAINPUT_ASSERT(state_);
	previousState_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), MouseButtonCount + MouseAxisCount);
	GAINPUT_ASSERT(previousState_);
}

InputDeviceMouse::~InputDeviceMouse()
{
	impl_->GetManager().GetAllocator().Delete(state_);
	impl_->GetManager().GetAllocator().Delete(previousState_);
	impl_->GetManager().GetAllocator().Delete(impl_);
}

void
InputDeviceMouse::Update(InputDeltaState* delta)
{
	*previousState_ = *state_;
	impl_->Update(*state_, *previousState_, delta);
}

size_t
InputDeviceMouse::GetAnyButtonDown(DeviceButtonSpec* outButtons, size_t maxButtonCount) const
{
	GAINPUT_ASSERT(outButtons);
	GAINPUT_ASSERT(maxButtonCount > 0);
	return CheckAllButtonsDown(outButtons, maxButtonCount, MOUSE_BUTTON_0, MOUSE_BUTTON_COUNT, impl_->GetDevice());
}

size_t
InputDeviceMouse::GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const
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
InputDeviceMouse::GetButtonType(DeviceButtonId deviceButton) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	return deviceButtonInfos[deviceButton].type;
}

DeviceButtonId
InputDeviceMouse::GetButtonByName(const char* name) const
{
	GAINPUT_ASSERT(name);
	for (unsigned i = 0; i < MouseButtonCount + MouseAxisCount; ++i)
	{
		if (strcmp(name, deviceButtonInfos[i].name) == 0)
		{
			return DeviceButtonId(i);
		}
	}
	return InvalidDeviceButtonId;
}

}

#endif

