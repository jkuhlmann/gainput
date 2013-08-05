
#ifndef GAINPUTINPUTDEVICEMOUSEWIN_H_
#define GAINPUTINPUTDEVICEMOUSEWIN_H_

namespace gainput
{

class InputDeviceMouseImpl
{
public:
	InputDeviceMouseImpl(InputManager& manager, DeviceId device);

	void Update(InputState& state, InputState& previousState, InputDeltaState* delta);

	void HandleMessage(const MSG& msg);

	InputManager& GetManager() const { return manager_; }
	DeviceId GetDevice() const { return device_; }

private:
	InputManager& manager_;
	DeviceId device_;
	InputState* state_;
	InputState* previousState_;
	InputState nextState_;
	InputDeltaState* delta_;
	Array<DeviceButtonId> buttonsToReset_;
};

}

#endif

