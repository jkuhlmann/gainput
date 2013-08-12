
#ifndef GAINPUTKEYBOARDCOMMON_H_
#define GAINPUTKEYBOARDCOMMON_H_

namespace gainput
{

InputDeviceKeyboard::InputDeviceKeyboard(InputManager& manager, DeviceId device, DeviceVariant variant) :
	InputDevice(manager, device, manager.GetDeviceCountByType(DT_KEYBOARD))
{
	impl_ = manager.GetAllocator().New<InputDeviceKeyboardImpl>(manager, device);
	GAINPUT_ASSERT(impl_);
	state_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), KeyboardButtonCount);
	GAINPUT_ASSERT(state_);
	previousState_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), KeyboardButtonCount);
	GAINPUT_ASSERT(previousState_);
}

InputDeviceKeyboard::~InputDeviceKeyboard()
{
	impl_->GetManager().GetAllocator().Delete(state_);
	impl_->GetManager().GetAllocator().Delete(previousState_);
	impl_->GetManager().GetAllocator().Delete(impl_);
}

void
InputDeviceKeyboard::InternalUpdate(InputDeltaState* delta)
{
	impl_->Update(*state_, *previousState_, delta);
}

size_t
InputDeviceKeyboard::GetAnyButtonDown(DeviceButtonSpec* outButtons, size_t maxButtonCount) const
{
	GAINPUT_ASSERT(outButtons);
	GAINPUT_ASSERT(maxButtonCount > 0);
	return CheckAllButtonsDown(outButtons, maxButtonCount, 0, KeyboardButtonCount);
}

size_t
InputDeviceKeyboard::GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	GAINPUT_ASSERT(buffer);
	GAINPUT_ASSERT(bufferLength > 0);
	return impl_->GetKeyName(deviceButton, buffer, bufferLength);
}

ButtonType
InputDeviceKeyboard::GetButtonType(DeviceButtonId deviceButton) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	return BT_BOOL;
}

DeviceButtonId
InputDeviceKeyboard::GetButtonByName(const char* name) const
{
	GAINPUT_ASSERT(name);
	return impl_->GetButtonByName(name);
}

bool
InputDeviceKeyboard::IsTextInputEnabled() const
{
	return impl_->IsTextInputEnabled();
}

void
InputDeviceKeyboard::SetTextInputEnabled(bool enabled)
{
	impl_->SetTextInputEnabled(enabled);
}

char
InputDeviceKeyboard::GetNextCharacter()
{
	return impl_->GetNextCharacter();
}

}

#endif

