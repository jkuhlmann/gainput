
#include <gainput/gainput.h>

#if defined(GAINPUT_PLATFORM_LINUX) && defined(GAINPUT_ENABLE_RAW_INPUT)

#include "GainputInputDeviceMouseEvdev.h"
#include "GainputMouseInfo.h"
#include "../GainputInputDeltaState.h"
#include "../GainputHelpers.h"
#include "../GainputHelpersEvdev.h"

namespace gainput
{

InputDeviceMouseImpl::InputDeviceMouseImpl(InputManager& manager, DeviceId device) :
	manager_(manager),
	device_(device),
	fd_(-1),
	state_(0),
	previousState_(0),
	delta_(0),
	buttonsToReset_(manager.GetAllocator())
{
	unsigned matchingDeviceCount = 0;
	for (unsigned i = 0; i < EvdevDeviceCount; ++i)
	{
		fd_ = open(EvdevDeviceIds[i], O_RDONLY|O_NONBLOCK);
		if (fd_ == -1)
		{
			continue;
		}

		EvdevDevice evdev(fd_);

		if (evdev.IsValid())
		{
			if (evdev.GetDeviceType() == InputDevice::DT_MOUSE)
			{
				if (matchingDeviceCount == manager_.GetDeviceCountByType(InputDevice::DT_MOUSE))
				{
					break;
				}
				++matchingDeviceCount;
			}
		}

		close(fd_);
		fd_ = -1;
	}

	GAINPUT_ASSERT(fd_ != -1);
}

InputDeviceMouseImpl::~InputDeviceMouseImpl()
{
	if (fd_ != -1)
	{
		close(fd_);
	}
}

void
InputDeviceMouseImpl::Update(InputState& state, InputState& previousState, InputDeltaState* delta)
{
	for (Array<DeviceButtonId>::const_iterator it = buttonsToReset_.begin();
			it != buttonsToReset_.end();
			++it)
	{
		HandleButton(device_, state, previousState, delta, *it, false);
	}
	buttonsToReset_.clear();

	if (fd_ < 0)
	{
		return;
	}

	struct input_event event;

	for (;;)
	{
		int len = read(fd_, &event, sizeof(struct input_event));
		if (len != sizeof(struct input_event))
		{
			break;
		}

		if (event.type == EV_KEY)
		{
			int button = -1;
			if (event.code == BTN_LEFT)
				button = MouseButtonLeft;
			else if (event.code == BTN_MIDDLE)
				button = MouseButtonMiddle;
			else if (event.code == BTN_RIGHT)
				button = MouseButtonRight;
			else if (event.code == BTN_SIDE)
				button = MouseButton5;
			else if (event.code == BTN_EXTRA)
				button = MouseButton6;

			if (button != -1)
			{
				HandleButton(device_, state, previousState, delta, DeviceButtonId(button), bool(event.value));
			}
		}
		else if (event.type == EV_REL)
		{
			int button = -1;
			if (event.code == REL_X)
				button = MouseAxisX;
			else if (event.code == REL_Y)
				button = MouseAxisY;
			else if (event.code == REL_HWHEEL)
				button = MouseButton7;
			else if (event.code == REL_WHEEL)
				button = MouseButtonWheelUp;

			if (button == MouseButtonWheelUp)
			{
				if (event.value < 0)
					button = MouseButtonWheelDown;
				HandleButton(device_, state, previousState, delta, DeviceButtonId(button), true);
				buttonsToReset_.push_back(button);
			}
			else if (button == MouseButton7)
			{
				if (event.value < 0)
					button = MouseButton8;
				HandleButton(device_, state, previousState, delta, DeviceButtonId(button), true);
				buttonsToReset_.push_back(button);
			}
			else if (button != -1)
			{
				const DeviceButtonId buttonId(button);
				const float prevValue = previousState.GetFloat(buttonId);
				HandleAxis(device_, state, previousState, delta, DeviceButtonId(button), prevValue + float(event.value));
			}
		}
	}
}

}

#include "GainputMouseCommon.h"

#endif

