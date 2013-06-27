
#ifndef GAINPUTINPUTDEVICETOUCHANDROID_H_
#define GAINPUTINPUTDEVICETOUCHANDROID_H_

namespace gainput
{

class InputDeviceTouchImpl
{
public:
	InputDeviceTouchImpl(InputManager& manager, DeviceId device);

	void Update(InputState& state, InputState& previousState, InputDeltaState* delta);

	InputManager& GetManager() const { return manager_; }
	DeviceId GetDevice() const { return device_; }

	InputDevice::DeviceState GetState() const { return InputDevice::DS_OK; }

	int32_t HandleInput(AInputEvent* event);

private:
	InputManager& manager_;
	DeviceId device_;
	InputState* state_;
	InputState* previousState_;
	InputDeltaState* delta_;

	void HandleBool(DeviceButtonId buttonId, bool value);
	void HandleFloat(DeviceButtonId buttonId, float value);
};

}

#endif

