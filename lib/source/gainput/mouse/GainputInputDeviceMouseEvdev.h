
#ifndef GAINPUTINPUTDEVICEMOUSEEVDEV_H_
#define GAINPUTINPUTDEVICEMOUSEEVDEV_H_

namespace gainput
{

class InputDeviceMouseImpl
{
public:
	InputDeviceMouseImpl(InputManager& manager, DeviceId device);
	~InputDeviceMouseImpl();

	void Update(InputState& state, InputState& previousState, InputDeltaState* delta);

	InputManager& GetManager() const { return manager_; }
	DeviceId GetDevice() const { return device_; }

private:
	InputManager& manager_;
	DeviceId device_;
	int fd_;
	InputState* state_;
	InputState* previousState_;
	InputDeltaState* delta_;
	Array<DeviceButtonId> buttonsToReset_;
};

}

#endif

