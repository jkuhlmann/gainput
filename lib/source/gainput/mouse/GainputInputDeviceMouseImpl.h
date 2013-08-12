
#ifndef GAINPUTINPUTDEVICEMOUSEIMPL_H_
#define GAINPUTINPUTDEVICEMOUSEIMPL_H_

namespace gainput
{

class InputDeviceMouseImpl
{
public:
	virtual ~InputDeviceMouseImpl() { }
	virtual InputDevice::DeviceVariant GetVariant() const = 0;
	virtual void Update(InputState& state, InputState& previousState, InputDeltaState* delta) = 0;
};

}

#endif

