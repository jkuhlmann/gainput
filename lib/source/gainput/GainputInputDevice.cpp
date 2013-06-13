
#include <gainput/gainput.h>

namespace gainput
{


InputDevice::InputDevice(InputManager& manager, DeviceId device, unsigned index) :
	manager_(manager),
	deviceId_(device),
	index_(index)
{
}

size_t
InputDevice::CheckAllButtonsDown(DeviceButtonSpec* outButtons, size_t maxButtonCount, unsigned start, unsigned end, DeviceId device) const
{
	size_t buttonsFound = 0;
	for (unsigned i = start; i < end; ++i)
	{
		DeviceButtonId id(i);
		if (IsValidButtonId(id) && GetBool(id))
		{
			outButtons[buttonsFound].deviceId = device;
			outButtons[buttonsFound].buttonId = id;
			++buttonsFound;
		}
	}
	return buttonsFound;
}

}

