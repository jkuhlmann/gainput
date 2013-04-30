
#include <gainput/gainput.h>

#if defined(GAINPUT_PLATFORM_WIN)

// Cf. http://msdn.microsoft.com/en-us/library/windows/desktop/ee417005%28v=vs.85%29.aspx

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <XInput.h>

#include "../GainputInputDeltaState.h"


namespace gainput
{


const float MaxTriggerValue = 255.0f;
const float MaxAxisValue = 32767.0f;
const float MaxMotorSpeed = 65535.0f;


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

	void HandleButton(InputState& state, InputState& previousState, InputDeltaState* delta, DeviceButtonId buttonId, bool value)
	{
		state.Set(buttonId, value);

#ifdef GAINPUT_DEBUG
		if (value != previousState.GetBool(buttonId))
		{
			GAINPUT_LOG("Pad changed: %d, %i\n", buttonId, value);
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

	void HandleAxis(InputState& state, InputState& previousState, InputDeltaState* delta, DeviceButtonId buttonId, float value)
	{
		if (value < -1.0f) // Because theoretical min value is -32768
		{
			value = -1.0f;
		}

		state.Set(buttonId, value);
		
#ifdef GAINPUT_DEBUG
		if (value != previousState.GetFloat(buttonId))
		{
			GAINPUT_LOG("Pad changed: %d, %f\n", buttonId, value);
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

	InputManager& GetManager() const { return manager_; }
	DeviceId GetDevice() const { return device_; }

	InputDevice::DeviceState GetState() const
	{
		return deviceState_;
	}

	bool IsValidButton(DeviceButtonId deviceButton) const
	{
		return deviceButton < PAD_BUTTON_AXIS_4 || (deviceButton >= PAD_BUTTON_START && deviceButton <= PAD_BUTTON_R3);
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

}

#include "GainputPadCommon.h"

namespace gainput
{

bool
InputDevicePad::Vibrate(float leftMotor, float rightMotor)
{
	return impl_->Vibrate(leftMotor, rightMotor);
}

}

#endif
