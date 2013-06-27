
#include <gainput/gainput.h>

#if !defined(GAINPUT_PLATFORM_ANDROID)

#include "GainputTouchInfo.h"

namespace gainput
{

class InputDeviceTouchImpl
{
public:
	InputDeviceTouchImpl(InputManager& manager, DeviceId device);

	void Update(InputState& state, InputState& previousState, InputDeltaState* delta);

	InputManager& GetManager() const { return manager_; }
	DeviceId GetDevice() const { return device_; }

	InputDevice::DeviceState GetState() const { return InputDevice::DS_UNAVAILABLE; }

private:
	InputManager& manager_;
	DeviceId device_;

	void HandleBool(DeviceButtonId buttonId, bool value);
	void HandleFloat(DeviceButtonId buttonId, float value);
};


InputDeviceTouchImpl::InputDeviceTouchImpl(InputManager& manager, DeviceId device) :
	manager_(manager),
	device_(device)
{
}

void
InputDeviceTouchImpl::Update(InputState& state, InputState& previousState, InputDeltaState* delta)
{
}

}

#include "GainputTouchCommon.h"

#endif

