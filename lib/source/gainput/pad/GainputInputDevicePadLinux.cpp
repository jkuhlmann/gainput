
#include <gainput/gainput.h>


#if defined(GAINPUT_PLATFORM_LINUX)

#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <errno.h>

#include "../GainputInputDeltaState.h"
#include "../GainputHelpers.h"


// Cf. http://www.kernel.org/doc/Documentation/input/joystick-api.txt
// Cf. http://ps3.jim.sh/sixaxis/usb/


namespace gainput
{

/// Maximum negative and positive value for an axis.
const float MaxAxisValue = 32767.0f;

static const char* PadDeviceIds[MaxPadCount] =
{
	"/dev/input/js0",
	"/dev/input/js1",
	"/dev/input/js2",
	"/dev/input/js3",
	"/dev/input/js4",
	"/dev/input/js5",
	"/dev/input/js6",
	"/dev/input/js7",
	"/dev/input/js8",
	"/dev/input/js9"
};

class InputDevicePadImpl
{
public:
	InputDevicePadImpl(InputManager& manager, DeviceId device, unsigned index) :
		manager_(manager),
		device_(device),
		index_(index),
		state_(InputDevice::DS_UNAVAILABLE),
		fd_(-1),
		buttonDialect_(manager_.GetAllocator())
	{
		GAINPUT_ASSERT(index_ < MaxPadCount);
		CheckForDevice();
	}

	~InputDevicePadImpl()
	{
		if (fd_ != -1)
		{
			close(fd_);
		}
	}

	void Update(InputState& state, InputState& previousState, InputDeltaState* delta)
	{
		CheckForDevice();

		if (fd_ < 0)
		{
			return;
		}

		js_event event;
		int c;

		while ( (c = read(fd_, &event, sizeof(js_event))) == sizeof(js_event) )
		{
			event.type &= ~JS_EVENT_INIT;
			if (event.type == JS_EVENT_AXIS)
			{
				GAINPUT_ASSERT(event.number >= PAD_BUTTON_LEFT_STICK_X);
				GAINPUT_ASSERT(event.number < PAD_BUTTON_AXIS_COUNT);
				DeviceButtonId buttonId = event.number;

				const float value = float(event.value)/MaxAxisValue;

				if (axisDialect_.count(buttonId))
				{
					buttonId = axisDialect_[buttonId];
				}

				if (buttonId == PAD_BUTTON_UP)
				{
					HandleButton(device_, state, previousState, delta, PAD_BUTTON_UP, value < 0.0f);
					HandleButton(device_, state, previousState, delta, PAD_BUTTON_DOWN, value > 0.0f);
				}
				else if (buttonId == PAD_BUTTON_LEFT)
				{
					HandleButton(device_, state, previousState, delta, PAD_BUTTON_LEFT, value < 0.0f);
					HandleButton(device_, state, previousState, delta, PAD_BUTTON_RIGHT, value > 0.0f);
				}
				else
				{
					HandleAxis(device_, state, previousState, delta, buttonId, value);
				}
			}
			else if (event.type == JS_EVENT_BUTTON)
			{
				GAINPUT_ASSERT(event.number >= 0);
				GAINPUT_ASSERT(event.number < PAD_BUTTON_COUNT);
				if (buttonDialect_.count(event.number))
				{
					DeviceButtonId buttonId = buttonDialect_[event.number];
					const bool value(event.value);
					state.Set(buttonId, value);

					HandleButton(device_, state, previousState, delta, buttonId, value);
				}
#ifdef GAINPUT_DEBUG
				else
				{
					GAINPUT_LOG("Unknown pad button #%d: %d\n", int(event.number), event.value);
				}
#endif
			}
		}
		GAINPUT_ASSERT(c == -1);

		if (c == -1 
			&& (errno == EBADF || errno == ECONNRESET || errno == ENOTCONN || errno == EIO || errno == ENXIO || errno == ENODEV))
		{
#ifdef GAINPUT_DEBUG
			GAINPUT_LOG("Pad lost.\n");
#endif
			state_ = InputDevice::DS_UNAVAILABLE;
			fd_ = -1;
		}
	}

	InputManager& GetManager() const { return manager_; }
	DeviceId GetDevice() const { return device_; }

	InputDevice::DeviceState GetState() const
	{
		return state_;
	}

	bool IsValidButton(DeviceButtonId deviceButton) const
	{
		if (buttonDialect_.empty())
		{
			return deviceButton >= PAD_BUTTON_LEFT_STICK_X && deviceButton < PAD_BUTTON_MAX;
		}

		for (HashMap<unsigned, DeviceButtonId>::const_iterator it = buttonDialect_.begin();
				it != buttonDialect_.end();
				++it)
		{
			if (it->second == deviceButton)
			{
				return true;
			}
		}

		for (HashMap<unsigned, DeviceButtonId>::const_iterator it = axisDialect_.begin();
				it != axisDialect_.end();
				++it)
		{
			if (it->second == deviceButton)
			{
				return true;
			}
		}

		return false;
	}

private:
	InputManager& manager_;
	DeviceId device_;
	unsigned index_;
	InputDevice::DeviceState state_;
	int fd_;
	HashMap<unsigned, DeviceButtonId> buttonDialect_;
	HashMap<unsigned, DeviceButtonId> axisDialect_;

	void CheckForDevice()
	{
		if (fd_ != -1)
		{
			return;
		}

		state_ = InputDevice::DS_UNAVAILABLE;

		fd_ = open(PadDeviceIds[index_], O_RDONLY | O_NONBLOCK);
		if (fd_ < 0)
		{
			return;
		}
		GAINPUT_ASSERT(fd_ >= 0);

#ifdef GAINPUT_DEBUG
		char axesCount;
		ioctl(fd_, JSIOCGAXES, &axesCount);
		GAINPUT_LOG("Axes count: %d\n", int(axesCount));

		int driverVersion;
		ioctl(fd_, JSIOCGVERSION, &driverVersion);
		GAINPUT_LOG("Driver version: %d\n", driverVersion);
#endif

		char name[128] = "";
		if (ioctl(fd_, JSIOCGNAME(sizeof(name)), name) < 0)
		{
			strncpy(name, "Unknown", sizeof(name));
		}
#ifdef GAINPUT_DEBUG
		GAINPUT_LOG("Name: %s\n", name);
#endif

		for (unsigned i = PAD_BUTTON_LEFT_STICK_X; i < PAD_BUTTON_AXIS_COUNT; ++i)
		{
			axisDialect_[i] = i;
		}

		if (strcmp(name, "Sony PLAYSTATION(R)3 Controller") == 0)
		{
#ifdef GAINPUT_DEBUG
			GAINPUT_LOG("  --> known controller\n");
#endif
			buttonDialect_[0] = PAD_BUTTON_SELECT;
			buttonDialect_[1] = PAD_BUTTON_L3;
			buttonDialect_[2] = PAD_BUTTON_R3;
			buttonDialect_[3] = PAD_BUTTON_START;
			buttonDialect_[4] = PAD_BUTTON_UP;
			buttonDialect_[5] = PAD_BUTTON_RIGHT;
			buttonDialect_[6] = PAD_BUTTON_DOWN;
			buttonDialect_[7] = PAD_BUTTON_LEFT;
			buttonDialect_[8] = PAD_BUTTON_L2;
			buttonDialect_[9] = PAD_BUTTON_R2;
			buttonDialect_[10] = PAD_BUTTON_L1;
			buttonDialect_[11] = PAD_BUTTON_R1;
			buttonDialect_[12] = PAD_BUTTON_Y;
			buttonDialect_[13] = PAD_BUTTON_B;
			buttonDialect_[14] = PAD_BUTTON_A;
			buttonDialect_[15] = PAD_BUTTON_X;
			buttonDialect_[16] = PAD_BUTTON_HOME;
		}
		else if (strcmp(name, "Microsoft X-Box 360 pad") == 0)
		{
#ifdef GAINPUT_DEBUG
			GAINPUT_LOG("  --> known controller\n");
#endif
			buttonDialect_[6] = PAD_BUTTON_SELECT;
			buttonDialect_[9] = PAD_BUTTON_L3;
			buttonDialect_[10] = PAD_BUTTON_R3;
			buttonDialect_[7] = PAD_BUTTON_START;
			buttonDialect_[4] = PAD_BUTTON_L1;
			buttonDialect_[5] = PAD_BUTTON_R1;
			buttonDialect_[3] = PAD_BUTTON_Y;
			buttonDialect_[1] = PAD_BUTTON_B;
			buttonDialect_[0] = PAD_BUTTON_A;
			buttonDialect_[2] = PAD_BUTTON_X;
			buttonDialect_[8] = PAD_BUTTON_HOME;

			axisDialect_[3] = PAD_BUTTON_RIGHT_STICK_X;
			axisDialect_[4] = PAD_BUTTON_RIGHT_STICK_Y;
			axisDialect_[2] = PAD_BUTTON_AXIS_4;
			axisDialect_[5] = PAD_BUTTON_AXIS_5;
			axisDialect_[7] = PAD_BUTTON_UP;
			axisDialect_[6] = PAD_BUTTON_LEFT;

			// Dummy entries for IsValidButton
			axisDialect_[-1] = PAD_BUTTON_DOWN;
			axisDialect_[-2] = PAD_BUTTON_RIGHT;
		}

		state_ = InputDevice::DS_OK;
	}
};

}

#include "GainputPadCommon.h"

namespace gainput
{

bool
InputDevicePad::Vibrate(float leftMotor, float rightMotor)
{
	return false; // TODO
}

}

#endif
