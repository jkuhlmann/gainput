
#include <gainput/gainput.h>


#if defined(GAINPUT_PLATFORM_LINUX)

#include <iostream>
#include <fcntl.h>
#include <linux/joystick.h>

#include "GainputInputDeltaState.h"


// Cf. http://www.kernel.org/doc/Documentation/input/joystick-api.txt
// Cf. http://ps3.jim.sh/sixaxis/usb/


namespace gainput
{

namespace
{
struct DeviceButtonInfo
{
	ButtonType type;
	const char* name;
};

DeviceButtonInfo deviceButtonInfos[] =
{
		{ BT_FLOAT, "pad_left_stick_x" },
		{ BT_FLOAT, "pad_left_stick_y" },
		{ BT_FLOAT, "pad_right_stick_x" },
		{ BT_FLOAT, "pad_right_stick_y" },
		{ BT_FLOAT, "pad_axis_4" },
		{ BT_FLOAT, "pad_axis_5" },
		{ BT_FLOAT, "pad_axis_6" },
		{ BT_FLOAT, "pad_axis_7" },
		{ BT_FLOAT, "pad_axis_8" },
		{ BT_FLOAT, "pad_axis_9" },
		{ BT_FLOAT, "pad_axis_10" },
		{ BT_FLOAT, "pad_axis_11" },
		{ BT_FLOAT, "pad_axis_12" },
		{ BT_FLOAT, "pad_axis_13" },
		{ BT_FLOAT, "pad_axis_14" },
		{ BT_FLOAT, "pad_axis_15" },
		{ BT_FLOAT, "pad_axis_16" },
		{ BT_FLOAT, "pad_axis_17" },
		{ BT_FLOAT, "pad_axis_18" },
		{ BT_FLOAT, "pad_axis_19" },
		{ BT_FLOAT, "pad_axis_20" },
		{ BT_FLOAT, "pad_axis_21" },
		{ BT_FLOAT, "pad_axis_22" },
		{ BT_FLOAT, "pad_axis_23" },
		{ BT_FLOAT, "pad_axis_24" },
		{ BT_FLOAT, "pad_axis_25" },
		{ BT_FLOAT, "pad_axis_26" },
		{ BT_FLOAT, "pad_axis_27" },
		{ BT_FLOAT, "pad_axis_28" },
		{ BT_FLOAT, "pad_axis_29" },
		{ BT_FLOAT, "pad_axis_30" },
		{ BT_FLOAT, "pad_axis_31" },
		{ BT_FLOAT, "pad_button_start"},
		{ BT_FLOAT, "pad_button_select"},
		{ BT_FLOAT, "pad_button_left"},
		{ BT_FLOAT, "pad_button_right"},
		{ BT_FLOAT, "pad_button_up"},
		{ BT_FLOAT, "pad_button_down"},
		{ BT_FLOAT, "pad_button_a"},
		{ BT_FLOAT, "pad_button_b"},
		{ BT_FLOAT, "pad_button_x"},
		{ BT_FLOAT, "pad_button_y"},
		{ BT_FLOAT, "pad_button_l1"},
		{ BT_FLOAT, "pad_button_r1"},
		{ BT_FLOAT, "pad_button_l2"},
		{ BT_FLOAT, "pad_button_r2"},
		{ BT_FLOAT, "pad_button_l3"},
		{ BT_FLOAT, "pad_button_r3"},
		{ BT_FLOAT, "pad_button_home"},
		{ BT_FLOAT, "pad_button_17"},
		{ BT_FLOAT, "pad_button_18"},
		{ BT_FLOAT, "pad_button_19"},
		{ BT_FLOAT, "pad_button_20"},
		{ BT_FLOAT, "pad_button_21"},
		{ BT_FLOAT, "pad_button_22"},
		{ BT_FLOAT, "pad_button_23"},
		{ BT_FLOAT, "pad_button_24"},
		{ BT_FLOAT, "pad_button_25"},
		{ BT_FLOAT, "pad_button_26"},
		{ BT_FLOAT, "pad_button_27"},
		{ BT_FLOAT, "pad_button_28"},
		{ BT_FLOAT, "pad_button_29"},
		{ BT_FLOAT, "pad_button_30"},
		{ BT_FLOAT, "pad_button_31"}
};
}

const unsigned PadButtonCount = PAD_BUTTON_COUNT;
const unsigned PadAxisCount = PAD_BUTTON_AXIS_COUNT;
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
	InputDevicePadImpl(InputManager& manager, DeviceId device) :
		manager_(manager),
		device_(device),
		state_(InputDevice::DS_UNAVAILABLE)
	{
		unsigned padIndex = manager_.GetDeviceCountByType(InputDevice::DT_PAD);
		GAINPUT_ASSERT(padIndex < MaxPadCount);

		fd_ = open(PadDeviceIds[padIndex], O_RDONLY | O_NONBLOCK);
		if (fd_ < 0)
		{
			state_ = InputDevice::DS_UNAVAILABLE;
			return;
		}
		GAINPUT_ASSERT(fd_ >= 0);

		char axesCount;
		ioctl(fd_, JSIOCGAXES, &axesCount);
		std::cout << "Axes count: " << int(axesCount) << std::endl;

		int driverVersion;
		ioctl(fd_, JSIOCGVERSION, &driverVersion);
		std::cout << "Driver version: " << driverVersion << std::endl;

		char name[128];
		if (ioctl(fd_, JSIOCGNAME(sizeof(name)), name) < 0)
			strncpy(name, "Unknown", sizeof(name));
		std::cout << "Name: " << name << std::endl;

		if (strcmp(name, "Sony PLAYSTATION(R)3 Controller") == 0)
		{
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

		state_ = InputDevice::DS_OK;
	}

	~InputDevicePadImpl()
	{
		close(fd_);
	}

	void Update(InputState& state, InputState& previousState, InputDeltaState* delta)
	{
		js_event event;
		int c;

		while ( (c = read(fd_, &event, sizeof(js_event))) == sizeof(js_event))
		{
			event.type &= ~JS_EVENT_INIT;
			if (event.type == JS_EVENT_AXIS)
			{
				GAINPUT_ASSERT(event.number >= PAD_BUTTON_LEFT_STICK_X);
				GAINPUT_ASSERT(event.number < PAD_BUTTON_AXIS_COUNT);
				DeviceButtonId buttonId = event.number;
				const float value = float(event.value)/MaxAxisValue;
				state.Set(buttonId, value);

				if (delta)
				{
					const float oldValue = previousState.GetFloat(buttonId);
					if (oldValue != value)
					{
						delta->AddChange(device_, buttonId, oldValue, value);
					}
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

					if (delta)
					{
						const bool oldValue = previousState.GetBool(buttonId);
						if (oldValue != value)
						{
							delta->AddChange(device_, buttonId, oldValue, value);
						}
					}
				}
				else
				{
					//std::cout << "Unknown pad button #" << int(event.number) << ": " << event.value << std::endl;
				}
			}
		}
		GAINPUT_ASSERT(c == -1);
	}

	InputDevice::DeviceState GetState() const
	{
		return state_;
	}


private:
	InputManager& manager_;
	DeviceId device_;
	InputDevice::DeviceState state_;
	int fd_;
	DialectTable<unsigned> buttonDialect_;
};



InputDevicePad::InputDevicePad(InputManager& manager, DeviceId device) :
	impl_(new InputDevicePadImpl(manager, device))
{
	GAINPUT_ASSERT(impl_);
	state_ = new InputState(manager.GetAllocator(), PadButtonCount + PadAxisCount);
	GAINPUT_ASSERT(state_);
	previousState_ = new InputState(manager.GetAllocator(), PadButtonCount + PadAxisCount);
	GAINPUT_ASSERT(previousState_);
}

InputDevicePad::~InputDevicePad()
{
	delete state_;
	delete previousState_;
	delete impl_;
}

void
InputDevicePad::Update(InputDeltaState* delta)
{
	*previousState_ = *state_;
	impl_->Update(*state_, *previousState_, delta);
}

InputDevice::DeviceState
InputDevicePad::GetState() const
{
	return impl_->GetState();
}

bool
InputDevicePad::GetAnyButtonDown(DeviceButtonId& outButtonId) const
{
	return false; // TODO
}

void
InputDevicePad::GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	strncpy(buffer, deviceButtonInfos[deviceButton].name, bufferLength);
}

ButtonType
InputDevicePad::GetButtonType(DeviceButtonId deviceButton) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	return deviceButtonInfos[deviceButton].type;
}

bool
InputDevicePad::Vibrate(float leftMotor, float rightMotor)
{
	return false; // TODO
}

}

#endif
