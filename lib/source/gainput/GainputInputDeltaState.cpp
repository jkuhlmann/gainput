
#include <gainput/gainput.h>
#include "GainputInputDeltaState.h"


namespace gainput
{

InputDeltaState::InputDeltaState(Allocator& allocator) :
	changes_(allocator)
{
}

void
InputDeltaState::AddChange(DeviceId device, DeviceButtonId deviceButton, bool oldValue, bool newValue)
{
	Change change;
	change.device = device;
	change.deviceButton = deviceButton;
	change.type = BT_BOOL;
	change.oldValue.b = oldValue;
	change.newValue.b = newValue;
	changes_.push_back(change);
}

void
InputDeltaState::AddChange(DeviceId device, DeviceButtonId deviceButton, float oldValue, float newValue)
{
	Change change;
	change.device = device;
	change.deviceButton = deviceButton;
	change.type = BT_FLOAT;
	change.oldValue.f = oldValue;
	change.newValue.f = newValue;
	changes_.push_back(change);
}

void
InputDeltaState::Clear()
{
	changes_.clear();
}

void
InputDeltaState::NotifyListeners(HashMap<ListenerId, InputListener*>& listeners) const
{
	for (Array<Change>::const_iterator it = changes_.begin();
			it != changes_.end();
			++it)
	{
		const Change& change = *it;
		for (HashMap<ListenerId, InputListener*>::iterator it2 = listeners.begin();
				it2 != listeners.end();
				++it2)
		{
			if (change.type == BT_BOOL)
			{
				it2->second->OnDeviceButtonBool(change.device, change.deviceButton, change.oldValue.b, change.newValue.b);
			}
			else if (change.type == BT_FLOAT)
			{
				it2->second->OnDeviceButtonFloat(change.device, change.deviceButton, change.oldValue.f, change.newValue.f);
			}
		}
	}
}

}

