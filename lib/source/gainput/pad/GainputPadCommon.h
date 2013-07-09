
#ifndef GAINPUTPADCOMMON_H_
#define GAINPUTPADCOMMON_H_

namespace gainput
{

namespace
{
struct DeviceButtonInfo
{
	ButtonType type;
	const char* name;
};

DeviceButtonInfo deviceButtonInfos[] =
{
		{ BT_FLOAT, "pad_left_stick_x" },
		{ BT_FLOAT, "pad_left_stick_y" },
		{ BT_FLOAT, "pad_right_stick_x" },
		{ BT_FLOAT, "pad_right_stick_y" },
		{ BT_FLOAT, "pad_axis_4" },
		{ BT_FLOAT, "pad_axis_5" },
		{ BT_FLOAT, "pad_axis_6" },
		{ BT_FLOAT, "pad_axis_7" },
		{ BT_FLOAT, "pad_axis_8" },
		{ BT_FLOAT, "pad_axis_9" },
		{ BT_FLOAT, "pad_axis_10" },
		{ BT_FLOAT, "pad_axis_11" },
		{ BT_FLOAT, "pad_axis_12" },
		{ BT_FLOAT, "pad_axis_13" },
		{ BT_FLOAT, "pad_axis_14" },
		{ BT_FLOAT, "pad_axis_15" },
		{ BT_FLOAT, "pad_axis_16" },
		{ BT_FLOAT, "pad_axis_17" },
		{ BT_FLOAT, "pad_axis_18" },
		{ BT_FLOAT, "pad_axis_19" },
		{ BT_FLOAT, "pad_axis_20" },
		{ BT_FLOAT, "pad_axis_21" },
		{ BT_FLOAT, "pad_axis_22" },
		{ BT_FLOAT, "pad_axis_23" },
		{ BT_FLOAT, "pad_axis_24" },
		{ BT_FLOAT, "pad_axis_25" },
		{ BT_FLOAT, "pad_axis_26" },
		{ BT_FLOAT, "pad_axis_27" },
		{ BT_FLOAT, "pad_axis_28" },
		{ BT_FLOAT, "pad_axis_29" },
		{ BT_FLOAT, "pad_axis_30" },
		{ BT_FLOAT, "pad_axis_31" },
		{ BT_FLOAT, "pad_acceleration_x" },
		{ BT_FLOAT, "pad_acceleration_y" },
		{ BT_FLOAT, "pad_acceleration_z" },
		{ BT_FLOAT, "pad_gyroscope_x" },
		{ BT_FLOAT, "pad_gyroscope_y" },
		{ BT_FLOAT, "pad_gyroscope_z" },
		{ BT_FLOAT, "pad_magneticfield_x" },
		{ BT_FLOAT, "pad_magneticfield_y" },
		{ BT_FLOAT, "pad_magneticfield_z" },
		{ BT_BOOL, "pad_button_start"},
		{ BT_BOOL, "pad_button_select"},
		{ BT_BOOL, "pad_button_left"},
		{ BT_BOOL, "pad_button_right"},
		{ BT_BOOL, "pad_button_up"},
		{ BT_BOOL, "pad_button_down"},
		{ BT_BOOL, "pad_button_a"},
		{ BT_BOOL, "pad_button_b"},
		{ BT_BOOL, "pad_button_x"},
		{ BT_BOOL, "pad_button_y"},
		{ BT_BOOL, "pad_button_l1"},
		{ BT_BOOL, "pad_button_r1"},
		{ BT_BOOL, "pad_button_l2"},
		{ BT_BOOL, "pad_button_r2"},
		{ BT_BOOL, "pad_button_l3"},
		{ BT_BOOL, "pad_button_r3"},
		{ BT_BOOL, "pad_button_home"},
		{ BT_BOOL, "pad_button_17"},
		{ BT_BOOL, "pad_button_18"},
		{ BT_BOOL, "pad_button_19"},
		{ BT_BOOL, "pad_button_20"},
		{ BT_BOOL, "pad_button_21"},
		{ BT_BOOL, "pad_button_22"},
		{ BT_BOOL, "pad_button_23"},
		{ BT_BOOL, "pad_button_24"},
		{ BT_BOOL, "pad_button_25"},
		{ BT_BOOL, "pad_button_26"},
		{ BT_BOOL, "pad_button_27"},
		{ BT_BOOL, "pad_button_28"},
		{ BT_BOOL, "pad_button_29"},
		{ BT_BOOL, "pad_button_30"},
		{ BT_BOOL, "pad_button_31"}
};

}

const unsigned PadButtonCount = PAD_BUTTON_COUNT;
const unsigned PadAxisCount = PAD_BUTTON_AXIS_COUNT;


InputDevicePad::InputDevicePad(InputManager& manager, DeviceId device) :
	InputDevice(manager, device, manager.GetDeviceCountByType(DT_PAD))
{
	impl_ = manager.GetAllocator().New<InputDevicePadImpl>(manager, device, index_);
	GAINPUT_ASSERT(impl_);
	state_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), PadButtonCount + PadAxisCount);
	GAINPUT_ASSERT(state_);
	previousState_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), PadButtonCount + PadAxisCount);
	GAINPUT_ASSERT(previousState_);
}

InputDevicePad::~InputDevicePad()
{
	impl_->GetManager().GetAllocator().Delete(state_);
	impl_->GetManager().GetAllocator().Delete(previousState_);
	impl_->GetManager().GetAllocator().Delete(impl_);
}

void
InputDevicePad::InternalUpdate(InputDeltaState* delta)
{
	impl_->Update(*state_, *previousState_, delta);
}

InputDevice::DeviceState
InputDevicePad::InternalGetState() const
{
	return impl_->GetState();
}

bool
InputDevicePad::IsValidButtonId(DeviceButtonId deviceButton) const
{
	return impl_->IsValidButton(deviceButton);
}

size_t
InputDevicePad::GetAnyButtonDown(DeviceButtonSpec* outButtons, size_t maxButtonCount) const
{
	GAINPUT_ASSERT(outButtons);
	GAINPUT_ASSERT(maxButtonCount > 0);
	return CheckAllButtonsDown(outButtons, maxButtonCount, PAD_BUTTON_LEFT_STICK_X, PAD_BUTTON_MAX, impl_->GetDevice());
}

size_t
InputDevicePad::GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const
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
InputDevicePad::GetButtonType(DeviceButtonId deviceButton) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	return deviceButtonInfos[deviceButton].type;
}

DeviceButtonId
InputDevicePad::GetButtonByName(const char* name) const
{
	GAINPUT_ASSERT(name);
	for (unsigned i = 0; i < PadButtonCount + PadAxisCount; ++i)
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

