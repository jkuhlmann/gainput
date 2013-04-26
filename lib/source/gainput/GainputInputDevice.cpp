
#include <gainput/gainput.h>

namespace gainput
{


InputDevice::InputDevice(DeviceId device) :
	deviceId_(device)
{
}

size_t
InputDevice::CheckAllButtonsDown(DeviceButtonSpec* outButtons, size_t maxButtonCount, unsigned start, unsigned end, DeviceId device) const
{
	size_t buttonsFound = 0;
	for (unsigned i = start; i < end; ++i)
	{
		DeviceButtonId id(i);
		if (GetButtonType(id) == BT_BOOL && GetBool(id))
		{
			outButtons[buttonsFound].deviceId = device;
			outButtons[buttonsFound].buttonId = id;
			++buttonsFound;
		}
	}
	return buttonsFound;
}

}

