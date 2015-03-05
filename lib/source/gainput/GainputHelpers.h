
#ifndef GAINPUTHELPERS_H_
#define GAINPUTHELPERS_H_

#include "GainputLog.h"

namespace gainput
{

	inline void HandleButton(DeviceId device, InputState& state, InputState& previousState, InputDeltaState* delta, DeviceButtonId buttonId, bool value)
	{
#ifdef GAINPUT_DEBUG
		if (value != state.GetBool(buttonId))
		{
			GAINPUT_LOG("Button changed: %d, %i\n", buttonId, value);
		}
#endif

		if (delta)
		{
			const bool oldValue = state.GetBool(buttonId);
			if (value != oldValue)
			{
				delta->AddChange(device, buttonId, oldValue, value);
			}
		}

		state.Set(buttonId, value);
	}

	inline void HandleAxis(DeviceId device, InputState& state, InputState& previousState, InputDeltaState* delta, DeviceButtonId buttonId, float value)
	{
#ifdef GAINPUT_DEBUG
		if (value != state.GetFloat(buttonId))
		{
			GAINPUT_LOG("Axis changed: %d, %f\n", buttonId, value);
		}
#endif

		if (delta)
		{
			const float oldValue = state.GetFloat(buttonId);
			if (value != oldValue)
			{
				delta->AddChange(device, buttonId, oldValue, value);
			}
		}

		state.Set(buttonId, value);
	}

}

#endif


