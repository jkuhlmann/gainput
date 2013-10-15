
#ifndef GAINPUTINPUTDEVICEPADWIN_H_
#define GAINPUTINPUTDEVICEPADWIN_H_

// Cf. http://msdn.microsoft.com/en-us/library/windows/desktop/ee417005%28v=vs.85%29.aspx

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <XInput.h>


namespace gainput
{


const float MaxTriggerValue = 255.0f;
const float MaxAxisValue = 32767.0f;
const float MaxMotorSpeed = 65535.0f;


class InputDevicePadImplWin : public InputDevicePadImpl
{
public:
	InputDevicePadImplWin(InputManager& manager, DeviceId device, unsigned index) :
		manager_(manager),
		device_(device),
		deviceState_(InputDevice::DS_UNAVAILABLE),
		lastPacketNumber_(-1),
		hasBattery_(false)
	{
		padIndex_ = index;
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

	InputDevice::DeviceVariant GetVariant() const
	{
		return InputDevice::DV_STANDARD;
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

		HandleButton(device_, state, previousState, delta, PadButtonUp, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
		HandleButton(device_, state, previousState, delta, PadButtonDown, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
		HandleButton(device_, state, previousState, delta, PadButtonLeft, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
		HandleButton(device_, state, previousState, delta, PadButtonRight, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
		HandleButton(device_, state, previousState, delta, PadButtonA, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_A);
		HandleButton(device_, state, previousState, delta, PadButtonB, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_B);
		HandleButton(device_, state, previousState, delta, PadButtonX, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_X);
		HandleButton(device_, state, previousState, delta, PadButtonY, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_Y);
		HandleButton(device_, state, previousState, delta, PadButtonStart, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_START);
		HandleButton(device_, state, previousState, delta, PadButtonSelect, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_BACK);
		HandleButton(device_, state, previousState, delta, PadButtonL3, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
		HandleButton(device_, state, previousState, delta, PadButtonR3, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
		HandleButton(device_, state, previousState, delta, PadButtonL1, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
		HandleButton(device_, state, previousState, delta, PadButtonR1, xstate.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);

		HandleAxis(device_, state, previousState, delta, PadButtonL2, float(xstate.Gamepad.bLeftTrigger)/MaxTriggerValue);
		HandleAxis(device_, state, previousState, delta, PadButtonR2, float(xstate.Gamepad.bRightTrigger)/MaxTriggerValue);
		HandleAxis(device_, state, previousState, delta, PadButtonLeftStickX, float(xstate.Gamepad.sThumbLX)/MaxAxisValue);
		HandleAxis(device_, state, previousState, delta, PadButtonLeftStickY, float(xstate.Gamepad.sThumbLY)/MaxAxisValue);
		HandleAxis(device_, state, previousState, delta, PadButtonRightStickX, float(xstate.Gamepad.sThumbRX)/MaxAxisValue);
		HandleAxis(device_, state, previousState, delta, PadButtonRightStickY, float(xstate.Gamepad.sThumbRY)/MaxAxisValue);
	}

	InputDevice::DeviceState GetState() const
	{
		return deviceState_;
	}

	bool IsValidButton(DeviceButtonId deviceButton) const
	{
		return deviceButton < PadButtonAxis4 || (deviceButton >= PadButtonStart && deviceButton <= PadButtonR3);
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

#endif

