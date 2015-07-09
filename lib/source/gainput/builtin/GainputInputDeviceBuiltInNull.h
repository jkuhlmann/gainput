
#ifndef GAINPUTINPUTDEVICEBUILTINNULL_H_
#define GAINPUTINPUTDEVICEBUILTINNULL_H_


namespace gainput
{

class InputDeviceBuiltInImplNull : public InputDeviceBuiltInImpl
{
public:
	InputDeviceBuiltInImplNull(InputManager& manager, InputDevice& device, unsigned /*index*/, InputState& /*state*/, InputState& /*previousState*/) :
		manager_(manager),
		device_(device)
	{
	}

	InputDevice::DeviceVariant GetVariant() const
	{
		return InputDevice::DV_NULL;
	}

	void Update(InputDeltaState* /*delta*/)
	{
	}

	InputDevice::DeviceState GetState() const
	{
		return InputDevice::DS_OK;
	}

	bool IsValidButton(DeviceButtonId /*deviceButton*/) const
	{
		return false;
	}

private:
	InputManager& manager_;
	InputDevice& device_;

};

}

#endif
