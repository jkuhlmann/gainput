
#include <gainput/gainput.h>

namespace gainput
{


InputDevice::InputDevice(InputManager& manager, DeviceId device, unsigned index) :
	manager_(manager),
	deviceId_(device),
	index_(index),
	debugRenderingEnabled_(false)
#if defined(GAINPUT_DEV) || defined(GAINPUT_ENABLE_RECORDER)
	, synced_(false)
#endif
{
}

void
InputDevice::Update(InputDeltaState* delta)
{
	*previousState_ = *state_;
#if defined(GAINPUT_DEV)
	if (synced_)
	{
		return;
	}
#endif
	InternalUpdate(delta);
}

InputDevice::DeviceState
InputDevice::GetState() const
{
#if defined(GAINPUT_DEV)
	if (synced_)
	{
		return DS_OK;
	}
#endif
	return InternalGetState();
}

void
InputDevice::SetDebugRenderingEnabled(bool enabled)
{
	debugRenderingEnabled_ = enabled;
}

size_t
InputDevice::CheckAllButtonsDown(DeviceButtonSpec* outButtons, size_t maxButtonCount, unsigned start, unsigned end) const
{
	size_t buttonsFound = 0;
	for (unsigned i = start; i < end; ++i)
	{
		DeviceButtonId id(i);
		if (IsValidButtonId(id) && GetBool(id))
		{
			outButtons[buttonsFound].deviceId = deviceId_;
			outButtons[buttonsFound].buttonId = id;
			++buttonsFound;
		}
	}
	return buttonsFound;
}

}

