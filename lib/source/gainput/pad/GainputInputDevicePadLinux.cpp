
#include <gainput/gainput.h>


#if defined(GAINPUT_PLATFORM_LINUX)

#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>

#include "../GainputInputDeltaState.h"
#include "GainputPadInfo.h"


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
	InputDevicePadImpl(InputManager& manager, DeviceId device) :
		manager_(manager),
		device_(device),
		state_(InputDevice::DS_UNAVAILABLE),
		buttonDialect_(manager_.GetAllocator())
	{
		unsigned padIndex = manager_.GetDeviceCountByType(InputDevice::DT_PAD);
		GAINPUT_ASSERT(padIndex < MaxPadCount);

		fd_ = open(PadDeviceIds[padIndex], O_RDONLY | O_NONBLOCK);
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

		char name[128];
		if (ioctl(fd_, JSIOCGNAME(sizeof(name)), name) < 0)
		{
			strncpy(name, "Unknown", sizeof(name));
		}
#ifdef GAINPUT_DEBUG
		GAINPUT_LOG("Name: %s\n", name);
#endif

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

		while ( (c = read(fd_, &event, sizeof(js_event))) == sizeof(js_event) )
		{
			event.type &= ~JS_EVENT_INIT;
			if (event.type == JS_EVENT_AXIS)
			{
				GAINPUT_ASSERT(event.number >= PAD_BUTTON_LEFT_STICK_X);
				GAINPUT_ASSERT(event.number < PAD_BUTTON_AXIS_COUNT);
				DeviceButtonId buttonId = event.number;
				const float value = float(event.value)/MaxAxisValue;
				state.Set(buttonId, value);

#ifdef GAINPUT_DEBUG
				GAINPUT_LOG("Pad axis: %i, %f\n", buttonId, value);
#endif

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

#ifdef GAINPUT_DEBUG
					GAINPUT_LOG("Pad button: %i, %d\n", buttonId, value);
#endif

					if (delta)
					{
						const bool oldValue = previousState.GetBool(buttonId);
						if (oldValue != value)
						{
							delta->AddChange(device_, buttonId, oldValue, value);
						}
					}
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

		return false;
	}

private:
	InputManager& manager_;
	DeviceId device_;
	InputDevice::DeviceState state_;
	int fd_;
	HashMap<unsigned, DeviceButtonId> buttonDialect_;
};



InputDevicePad::InputDevicePad(InputManager& manager, DeviceId device)
{
	impl_ = manager.GetAllocator().New<InputDevicePadImpl>(manager, device);
	GAINPUT_ASSERT(impl_);
	state_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), PadButtonCount + PadAxisCount);
	GAINPUT_ASSERT(state_);
	previousState_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), PadButtonCount + PadAxisCount);
	GAINPUT_ASSERT(previousState_);
}

InputDevicePad::~InputDevicePad()
{
	impl_->GetManager().GetAllocator().Delete(state_);
	impl_->GetManager().GetAllocator().Delete(previousState_);
	impl_->GetManager().GetAllocator().Delete(impl_);
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
InputDevicePad::IsValidButtonId(DeviceButtonId deviceButton) const
{
	return impl_->IsValidButton(deviceButton);
}

size_t
InputDevicePad::GetAnyButtonDown(DeviceButtonSpec* outButtons, size_t maxButtonCount) const
{
	GAINPUT_ASSERT(outButtons);
	GAINPUT_ASSERT(maxButtonCount > 0);
	return CheckAllButtonsDown(outButtons, maxButtonCount, PAD_BUTTON_LEFT_STICK_X, PAD_BUTTON_MAX, impl_->GetDevice());
}

size_t
InputDevicePad::GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	GAINPUT_ASSERT(buffer);
	GAINPUT_ASSERT(bufferLength > 0);
	strncpy(buffer, deviceButtonInfos[deviceButton].name, bufferLength);
	buffer[bufferLength-1] = 0;
	const size_t nameLen = strlen(deviceButtonInfos[deviceButton].name);
	return nameLen >= bufferLength ? bufferLength : nameLen+1;
}

ButtonType
InputDevicePad::GetButtonType(DeviceButtonId deviceButton) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	return deviceButtonInfos[deviceButton].type;
}

DeviceButtonId
InputDevicePad::GetButtonByName(const char* name) const
{
	GAINPUT_ASSERT(name);
	for (unsigned i = 0; i < PadButtonCount + PadAxisCount; ++i)
	{
		if (strcmp(name, deviceButtonInfos[i].name) == 0)
		{
			return DeviceButtonId(i);
		}
	}
	return InvalidDeviceButtonId;
}

bool
InputDevicePad::Vibrate(float leftMotor, float rightMotor)
{
	return false; // TODO
}

}

#endif
