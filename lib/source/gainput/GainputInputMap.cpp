
#include <gainput/gainput.h>

#include <iostream>


namespace gainput
{

class MappedInput
{
public:
	enum MappedType
	{
		MT_DEVICE_BUTTON,
		MT_GESTURE_BUTTON
	};

	MappedType mappedType;

	DeviceId device;
	DeviceButtonId deviceButton;

	float rangeMin;
	float rangeMax;
};

typedef Array<MappedInput> MappedInputList;

class UserButton
{
public:
	enum UserButtonPolicy
	{
		UBP_FIRST_DOWN,
		UBP_MAX,
		UBP_MIN,
		UBP_AVERAGE
	};

	UserButtonId userButton;
	MappedInputList inputs;
	UserButtonPolicy policy;

	UserButton(Allocator& allocator) :
		inputs(allocator)
	{ }
};


InputMap::InputMap(const InputManager& manager, Allocator& allocator) :
	allocator_(allocator),
	manager_(manager),
	userButtons_(allocator_),
	nextUserButtonId_(0),
	gestures_(allocator_),
	listeners_(allocator_)
{

}

InputMap::~InputMap()
{
	Clear();
}

void
InputMap::Clear()
{
	for (UserButtonMap::iterator it = userButtons_.begin();
			it != userButtons_.end();
			++it)
	{
		allocator_.Delete(it->second);
	}
	userButtons_.clear();
	nextUserButtonId_ = 0;
}

bool
InputMap::MapBool(UserButtonId userButton, DeviceId device, DeviceButtonId deviceButton)
{
	UserButton* ub = GetUserButton(userButton);

	if (!ub)
	{
		ub = allocator_.New<UserButton>(allocator_);
		GAINPUT_ASSERT(ub);
		ub->userButton = nextUserButtonId_++;
		ub->policy = UserButton::UBP_FIRST_DOWN;
		userButtons_[userButton] = ub;
	}

	MappedInput mi;
	mi.mappedType = MappedInput::MT_DEVICE_BUTTON;
	mi.device = device;
	mi.deviceButton = deviceButton;
	ub->inputs.push_back(mi);

	return true;
}

bool
InputMap::IsMapped(UserButtonId userButton) const
{
	return GetUserButton(userButton) != 0;
}

bool
InputMap::GetBool(UserButtonId userButton) const
{
	const UserButton* ub = GetUserButton(userButton);
	GAINPUT_ASSERT(ub);
	for (MappedInputList::const_iterator it = ub->inputs.begin();
			it != ub->inputs.end();
			++it)
	{
		const MappedInput& mi= *it;
		const InputDevice* device = manager_.GetDevice(mi.device);
		GAINPUT_ASSERT(device);
		if (device->GetBool(mi.deviceButton))
		{
			return true;
		}
	}
	return false;
}

bool
InputMap::GetBoolIsNew(UserButtonId userButton) const
{
	const UserButton* ub = GetUserButton(userButton);
	GAINPUT_ASSERT(ub);
	for (MappedInputList::const_iterator it = ub->inputs.begin();
			it != ub->inputs.end();
			++it)
	{
		const MappedInput& mi= *it;
		const InputDevice* device = manager_.GetDevice(mi.device);
		GAINPUT_ASSERT(device);
		if (device->GetBool(mi.deviceButton)
				&& !device->GetBoolPrevious(mi.deviceButton))
		{
			return true;
		}
	}
	return false;
}

bool
InputMap::GetBoolPrevious(UserButtonId userButton) const
{
	const UserButton* ub = GetUserButton(userButton);
	GAINPUT_ASSERT(ub);
	for (MappedInputList::const_iterator it = ub->inputs.begin();
			it != ub->inputs.end();
			++it)
	{
		const MappedInput& mi= *it;
		const InputDevice* device = manager_.GetDevice(mi.device);
		GAINPUT_ASSERT(device);
		if (device->GetBoolPrevious(mi.deviceButton))
		{
			return true;
		}
	}
	return false;
}

bool
InputMap::GetBoolWasDown(UserButtonId userButton) const
{
	const UserButton* ub = GetUserButton(userButton);
	GAINPUT_ASSERT(ub);
	for (MappedInputList::const_iterator it = ub->inputs.begin();
			it != ub->inputs.end();
			++it)
	{
		const MappedInput& mi= *it;
		const InputDevice* device = manager_.GetDevice(mi.device);
		GAINPUT_ASSERT(device);
		if (!device->GetBool(mi.deviceButton)
				&& device->GetBoolPrevious(mi.deviceButton))
		{
			return true;
		}
	}
	return false;
}

float
InputMap::GetFloat(UserButtonId userButton) const
{
	float value = 0.0f;
	int downCount = 0;
	const UserButton* ub = GetUserButton(userButton);
	GAINPUT_ASSERT(ub);
	for (MappedInputList::const_iterator it = ub->inputs.begin();
			it != ub->inputs.end();
			++it)
	{
		const MappedInput& mi= *it;
		const InputDevice* device = manager_.GetDevice(mi.device);
		GAINPUT_ASSERT(device);

		bool down = false;
		float deviceValue = 0.0f;
		if (device->GetButtonType(mi.deviceButton) == BT_BOOL)
		{
			deviceValue = device->GetBool(mi.deviceButton)
					? mi.rangeMax
					: mi.rangeMin;
			down = device->GetBool(mi.deviceButton);
		}
		else if (device->GetFloat(mi.deviceButton) != 0.0f)
		{
			GAINPUT_ASSERT(device->GetButtonType(mi.deviceButton) == BT_FLOAT);
			deviceValue = mi.rangeMin + device->GetFloat(mi.deviceButton)*mi.rangeMax;
			down = true;
		}

		if (down)
		{
			++downCount;
			if (ub->policy == UserButton::UBP_FIRST_DOWN)
			{
				value = deviceValue;
				break;
			}
			else if (ub->policy == UserButton::UBP_MAX)
			{
				value = std::max(value, deviceValue);
			}
			else if (ub->policy == UserButton::UBP_MIN)
			{
				if (downCount == 1)
				{
					value = deviceValue;
				}
				else
				{
					value = std::min(value, deviceValue);
				}
			}
			else if (ub->policy == UserButton::UBP_AVERAGE)
			{
				value += deviceValue;
			}
		}
	}

	if (ub->policy == UserButton::UBP_AVERAGE && downCount)
	{
		value /= float(downCount);
	}

	return value;
}

UserButton*
InputMap::GetUserButton(UserButtonId userButton)
{
	UserButtonMap::iterator it = userButtons_.find(userButton);
	if (it == userButtons_.end())
	{
		return 0;
	}
	return it->second;
}

const UserButton*
InputMap::GetUserButton(UserButtonId userButton) const
{
	UserButtonMap::const_iterator it = userButtons_.find(userButton);
	if (it == userButtons_.end())
	{
		return 0;
	}
	return it->second;
}

}

