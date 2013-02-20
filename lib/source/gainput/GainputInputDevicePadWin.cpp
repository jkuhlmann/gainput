
#include <gainput/gainput.h>

#if defined(GAINPUT_PLATFORM_WIN)

// Cf. http://msdn.microsoft.com/en-us/library/windows/desktop/ee417005%28v=vs.85%29.aspx

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <XInput.h>

#include "GainputInputDeltaState.h"


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
		{ BT_FLOAT, "pad_button_start"},
		{ BT_FLOAT, "pad_button_select"},
		{ BT_FLOAT, "pad_button_left"},
		{ BT_FLOAT, "pad_button_right"},
		{ BT_FLOAT, "pad_button_up"},
		{ BT_FLOAT, "pad_button_down"},
		{ BT_FLOAT, "pad_button_a"},
		{ BT_FLOAT, "pad_button_b"},
		{ BT_FLOAT, "pad_button_x"},
		{ BT_FLOAT, "pad_button_y"},
		{ BT_FLOAT, "pad_button_l1"},
		{ BT_FLOAT, "pad_button_r1"},
		{ BT_FLOAT, "pad_button_l2"},
		{ BT_FLOAT, "pad_button_r2"},
		{ BT_FLOAT, "pad_button_l3"},
		{ BT_FLOAT, "pad_button_r3"},
		{ BT_FLOAT, "pad_button_home"},
		{ BT_FLOAT, "pad_button_17"},
		{ BT_FLOAT, "pad_button_18"},
		{ BT_FLOAT, "pad_button_19"},
		{ BT_FLOAT, "pad_button_20"},
		{ BT_FLOAT, "pad_button_21"},
		{ BT_FLOAT, "pad_button_22"},
		{ BT_FLOAT, "pad_button_23"},
		{ BT_FLOAT, "pad_button_24"},
		{ BT_FLOAT, "pad_button_25"},
		{ BT_FLOAT, "pad_button_26"},
		{ BT_FLOAT, "pad_button_27"},
		{ BT_FLOAT, "pad_button_28"},
		{ BT_FLOAT, "pad_button_29"},
		{ BT_FLOAT, "pad_button_30"},
		{ BT_FLOAT, "pad_button_31"}
};
}

const float MaxTriggerValue = 255.0f;
const float MaxAxisValue = 32767.0f;
const float MaxMotorSpeed = 65535.0f;

const unsigned PadButtonCount = PAD_BUTTON_COUNT;
const unsigned PadAxisCount = PAD_BUTTON_AXIS_COUNT;


class InputDevicePadImpl
{
public:
	InputDevicePadImpl(InputManager& manager, DeviceId device) :
		manager_(manager),
		device_(device),
		deviceState_(InputDevice::DS_UNAVAILABLE),
		lastPacketNumber_(-1),
		hasBattery_(false)
	{
		padIndex_ = manager.GetDeviceCountByType(InputDevice::DT_PAD);
		GAINPUT_ASSERT(padIndex_ < MaxPadCount);

#if 0
		XINPUT_BATTERY_INFORMATION xbattery;
		DWORD result = XInputGetBatteryInformation(padIndex, BATTERY_DEVTYPE_GAMEPAD, &xbattery);
		if (result == ERROR_SUCCESS)
		{
			hasBattery = (xbattery.BatteryType == BATTERY_TYPE_ALKALINE
				|| xbattery.BatteryType == BATTERY_TYPE_NIMH);
		}
#endif
	}

	~InputDevicePadImpl()
	{
	}

	void Update(InputState& state, InputState& previousState, InputDeltaState* delta)
	{
		XINPUT_STATE xstate;
		DWORD result = XInputGetState(padIndex_, &xstate);

		if (result != ERROR_SUCCESS)
		{
			deviceState_ = InputDevice::DS_UNAVAILABLE;
			return;
		}

		deviceState_ = InputDevice::DS_OK;

#if 0
		if (hasBattery)
		{
			XINPUT_BATTERY_INFORMATION xbattery;
			result = XInputGetBatteryInformation(padIndex, BATTERY_DEVTYPE_GAMEPAD, &xbattery);
			if (result == ERROR_SUCCESS)
			{
				if (xbattery.BatteryType == BATTERY_TYPE_ALKALINE
					|| xbattery.BatteryType == BATTERY_TYPE_NIMH)
				{
					if (xbattery.BatteryLevel == BATTERY_LEVEL_EMPTY
						|| xbattery.BatteryLevel == BATTERY_LEVEL_LOW)
					{
						deviceState = InputDevice::DS_LOW_BATTERY;
					}
				}
			}
		}
#endif

		if (xstate.dwPacketNumber == lastPacketNumber_)
		{
			// Not changed
			return;
		}

		lastPacketNumber_ = xstate.dwPacketNumber;

		HandleButton(state, previousState, delta, PAD_BUTTON_UP, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
		HandleButton(state, previousState, delta, PAD_BUTTON_DOWN, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
		HandleButton(state, previousState, delta, PAD_BUTTON_LEFT, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
		HandleButton(state, previousState, delta, PAD_BUTTON_RIGHT, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
		HandleButton(state, previousState, delta, PAD_BUTTON_A, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_A);
		HandleButton(state, previousState, delta, PAD_BUTTON_B, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_B);
		HandleButton(state, previousState, delta, PAD_BUTTON_X, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_X);
		HandleButton(state, previousState, delta, PAD_BUTTON_Y, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_Y);
		HandleButton(state, previousState, delta, PAD_BUTTON_START, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_START);
		HandleButton(state, previousState, delta, PAD_BUTTON_SELECT, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_BACK);
		HandleButton(state, previousState, delta, PAD_BUTTON_L3, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
		HandleButton(state, previousState, delta, PAD_BUTTON_R3, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
		HandleButton(state, previousState, delta, PAD_BUTTON_L1, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
		HandleButton(state, previousState, delta, PAD_BUTTON_R1, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);

		HandleAxis(state, previousState, delta, PAD_BUTTON_L2, float(xstate.Gamepad.bLeftTrigger)/MaxTriggerValue);
		HandleAxis(state, previousState, delta, PAD_BUTTON_R2, float(xstate.Gamepad.bRightTrigger)/MaxTriggerValue);
		HandleAxis(state, previousState, delta, PAD_BUTTON_LEFT_STICK_X, float(xstate.Gamepad.sThumbLX)/MaxAxisValue);
		HandleAxis(state, previousState, delta, PAD_BUTTON_LEFT_STICK_Y, float(xstate.Gamepad.sThumbLY)/MaxAxisValue);
		HandleAxis(state, previousState, delta, PAD_BUTTON_RIGHT_STICK_X, float(xstate.Gamepad.sThumbRX)/MaxAxisValue);
		HandleAxis(state, previousState, delta, PAD_BUTTON_RIGHT_STICK_Y, float(xstate.Gamepad.sThumbRY)/MaxAxisValue);
	}

	inline void HandleButton(InputState& state, InputState& previousState, InputDeltaState* delta, DeviceButtonId buttonId, bool value)
	{
		state.Set(buttonId, value);
		
#ifdef GAINPUT_DEBUG
		if (value != previousState.GetBool(buttonId))
		{
			char buf[256];
			sprintf(buf, "Pad changed: %d, %i\n", buttonId, value);
			OutputDebugStringA(buf);
		}
#endif

		if (delta)
		{
			const bool oldValue = previousState.GetBool(buttonId);
			if (value != oldValue)
			{
				delta->AddChange(device_, buttonId, oldValue, value);
			}
		}
	}

	inline void HandleAxis(InputState& state, InputState& previousState, InputDeltaState* delta, DeviceButtonId buttonId, float value)
	{
		if (value < -1.0f) // Because theoretical min value is -32768
		{
			value = -1.0f;
		}

		state.Set(buttonId, value);
		
#ifdef GAINPUT_DEBUG
		if (value != previousState.GetFloat(buttonId))
		{
			char buf[256];
			sprintf(buf, "Pad changed: %d, %f\n", buttonId, value);
			OutputDebugStringA(buf);
		}
#endif

		if (delta)
		{
			const float oldValue = previousState.GetFloat(buttonId);
			if (value != oldValue)
			{
				delta->AddChange(device_, buttonId, oldValue, value);
			}
		}
	}

	DeviceId GetDevice() const { return device_; }

	InputDevice::DeviceState GetState() const
	{
		return deviceState_;
	}

	bool Vibrate(float leftMotor, float rightMotor)
	{
		GAINPUT_ASSERT(leftMotor >= 0.0f && leftMotor <= 1.0f);
		GAINPUT_ASSERT(rightMotor >= 0.0f && rightMotor <= 1.0f);
		XINPUT_VIBRATION xvibration;
		xvibration.wLeftMotorSpeed = leftMotor*MaxMotorSpeed;
		xvibration.wRightMotorSpeed = rightMotor*MaxMotorSpeed;
		DWORD result = XInputSetState(padIndex_, &xvibration);
		return result == ERROR_SUCCESS;
	}

private:
	InputManager& manager_;
	DeviceId device_;
	InputDevice::DeviceState deviceState_;
	unsigned padIndex_;
	DWORD lastPacketNumber_;
	bool hasBattery_;

};



InputDevicePad::InputDevicePad(InputManager& manager, DeviceId device) :
	impl_(new InputDevicePadImpl(manager, device))
{
	GAINPUT_ASSERT(impl_);
	state_ = new InputState(manager.GetAllocator(), PadButtonCount + PadAxisCount);
	GAINPUT_ASSERT(state_);
	previousState_ = new InputState(manager.GetAllocator(), PadButtonCount + PadAxisCount);
	GAINPUT_ASSERT(previousState_);
}

InputDevicePad::~InputDevicePad()
{
	delete state_;
	delete previousState_;
	delete impl_;
}

void
InputDevicePad::Update(InputDeltaState* delta)
{
	*previousState_ = *state_;
	impl_->Update(*state_, *previousState_, delta);
}

InputDevice::DeviceState
InputDevicePad::GetState() const
{
	return impl_->GetState();
}

size_t
InputDevicePad::GetAnyButtonDown(DeviceButtonSpec* outButtons, size_t maxButtonCount) const
{
	return CheckAllButtonsDown(outButtons, maxButtonCount, PAD_BUTTON_LEFT_STICK_X, PAD_BUTTON_MAX, impl_->GetDevice());
}

size_t
InputDevicePad::GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
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

bool
InputDevicePad::Vibrate(float leftMotor, float rightMotor)
{
	return impl_->Vibrate(leftMotor, rightMotor);
}

}

#endif
