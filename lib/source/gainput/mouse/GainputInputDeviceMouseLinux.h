
#ifndef GAINPUTINPUTDEVICEMOUSELINUX_H_
#define GAINPUTINPUTDEVICEMOUSELINUX_H_

namespace gainput
{

class InputDeviceMouseImpl
{
public:
	InputDeviceMouseImpl(InputManager& manager, DeviceId device);
	~InputDeviceMouseImpl();

	void Update(InputState& state, InputState& previousState, InputDeltaState* delta);

	void HandleEvent(XEvent& event);

	InputManager& GetManager() const { return manager_; }
	DeviceId GetDevice() const { return device_; }

private:
	InputManager& manager_;
	DeviceId device_;
	bool* isWheel_;
	bool* pressedThisFrame_;
	InputState* state_;
	InputState* previousState_;
	InputState nextState_;
	InputDeltaState* delta_;
};

}

#endif

