
#ifndef GAINPUTINPUTDEVICEPADIOS_H_
#define GAINPUTINPUTDEVICEPADIOS_H_


namespace gainput
{

class InputDevicePadImplIos : public InputDevicePadImpl
{
public:
	InputDevicePadImplIos(InputManager& manager, InputDevice& device, unsigned index, InputState& state, InputState& previousState);
	~InputDevicePadImplIos();

	InputDevice::DeviceVariant GetVariant() const
	{
		return InputDevice::DV_STANDARD;
	}

	void Update(InputDeltaState* delta);

	InputDevice::DeviceState GetState() const
	{
		return deviceState_;
	}

	bool IsValidButton(DeviceButtonId deviceButton) const;

	bool Vibrate(float leftMotor, float rightMotor)
	{
		return false;
	}

	bool pausePressed_;

private:
	InputManager& manager_;
	InputDevice& device_;
	unsigned index_;
	bool padFound_;
	InputState& state_;
	InputState& previousState_;
	InputDevice::DeviceState deviceState_;

	void* motionManager_;
	bool isExtended_;
	bool supportsMotion_;

};

}

#endif

