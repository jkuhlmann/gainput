
#ifndef GAINPUTINPUTDEVICETOUCHNULL_H_
#define GAINPUTINPUTDEVICETOUCHNULL_H_

namespace gainput
{

class InputDeviceTouchImplNull : public InputDeviceTouchImpl
{
public:
	InputDeviceTouchImplNull(InputManager& manager, DeviceId device) :
		manager_(manager),
		device_(device)
	{
	}

	InputDevice::DeviceVariant GetVariant() const
	{
		return InputDevice::DV_NULL;
	}

	void Update(InputDeltaState* delta)
	{
	}

	InputDevice::DeviceState GetState() const { return InputDevice::DS_OK; }

private:
	InputManager& manager_;
	DeviceId device_;

	void HandleBool(DeviceButtonId buttonId, bool value);
	void HandleFloat(DeviceButtonId buttonId, float value);
};

}

#endif

