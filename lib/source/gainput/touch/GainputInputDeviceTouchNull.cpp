
#include <gainput/gainput.h>

#if !defined(GAINPUT_PLATFORM_ANDROID)


namespace gainput
{

const unsigned TouchPointCount = 8;
const unsigned TouchDataElems = 4;



InputDeviceTouch::InputDeviceTouch(InputManager& manager, DeviceId device) :
	InputDevice(manager, device)
{
	state_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), TouchPointCount*TouchDataElems);
	GAINPUT_ASSERT(state_);
	previousState_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), TouchPointCount*TouchDataElems);
	GAINPUT_ASSERT(previousState_);
}

InputDeviceTouch::~InputDeviceTouch()
{
	manager_.GetAllocator().Delete(state_);
	manager_.GetAllocator().Delete(previousState_);
}

void
InputDeviceTouch::Update(InputDeltaState* delta)
{
}

size_t
InputDeviceTouch::GetAnyButtonDown(DeviceButtonSpec* outButtons, size_t maxButtonCount) const
{
	return 0;
}

size_t
InputDeviceTouch::GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const
{
	return 0;
}

ButtonType
InputDeviceTouch::GetButtonType(DeviceButtonId deviceButton) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	return BT_BOOL;
}

DeviceButtonId
InputDeviceTouch::GetButtonByName(const char* name) const
{
	return InvalidDeviceButtonId;
}

}

#endif

