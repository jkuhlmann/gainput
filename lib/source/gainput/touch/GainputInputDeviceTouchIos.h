
#ifndef GAINPUTINPUTDEVICETOUCHANDROID_H_
#define GAINPUTINPUTDEVICETOUCHANDROID_H_

#include "GainputInputDeviceTouchImpl.h"
#include "GainputTouchInfo.h"

#include "../GainputInputDeltaState.h"
#include "../GainputHelpers.h"

namespace gainput
{

class InputDeviceTouchImplIos : public InputDeviceTouchImpl
{
public:
	InputDeviceTouchImplIos(InputManager& manager, InputDevice& device, InputState& state, InputState& previousState) :
		manager_(manager),
		device_(device),
		state_(&state),
		previousState_(&previousState),
		nextState_(manager.GetAllocator(), TouchPointCount*TouchDataElems),
		delta_(0),
		touches_(manager.GetAllocator())
	{
	}

	InputDevice::DeviceVariant GetVariant() const
	{
		return InputDevice::DV_STANDARD;
	}

	void Update(InputDeltaState* delta)
	{
		delta_ = delta;
		*state_ = nextState_;
	}

	InputDevice::DeviceState GetState() const { return InputDevice::DS_OK; }

	void HandleTouch(void* id, float x, float y)
	{
		GAINPUT_ASSERT(state_);
		GAINPUT_ASSERT(previousState_);

		int touchIdx = -1;
		for (unsigned i = 0; i < touches_.size(); ++i)
		{
			if (touches_[i] == static_cast<void*>(id))
			{
				touchIdx = i;
				break;
			}
		}

		if (touchIdx == -1)
		{
			for (unsigned i = 0; i < touches_.size(); ++i)
			{
				if (touches_[i] == 0)
				{
					touches_[i] = static_cast<void*>(id);
					touchIdx = i;
					break;
				}
			}
		}

		if (touchIdx == -1)
		{
			touchIdx = static_cast<unsigned>(touches_.size());
			touches_.push_back(static_cast<void*>(id));
		}

		HandleBool(gainput::Touch0Down + touchIdx*4, true);
		HandleFloat(gainput::Touch0X + touchIdx*4, x);
		HandleFloat(gainput::Touch0Y + touchIdx*4, y);
		HandleFloat(gainput::Touch0Pressure + touchIdx*4, 1.0f);
	}

	void HandleTouchEnd(void* id, float x, float y)
	{
		GAINPUT_ASSERT(state_);
		GAINPUT_ASSERT(previousState_);

		int touchIdx = -1;
		for (unsigned i = 0; i < touches_.size(); ++i)
		{
			if (touches_[i] == static_cast<void*>(id))
			{
				touchIdx = i;
				break;
			}
		}

		GAINPUT_ASSERT(touchIdx != -1);
		if (touchIdx == -1)
		{
			return;
		}

		touches_[touchIdx] = 0;

		HandleBool(gainput::Touch0Down + touchIdx*4, false);
		HandleFloat(gainput::Touch0X + touchIdx*4, x);
		HandleFloat(gainput::Touch0Y + touchIdx*4, y);
		HandleFloat(gainput::Touch0Pressure + touchIdx*4, 0.0f);
	}

private:
	InputManager& manager_;
	InputDevice& device_;
	InputState* state_;
	InputState* previousState_;
	InputState nextState_;
	InputDeltaState* delta_;

	typedef gainput::Array< void* > TouchList;
	TouchList touches_;

	void HandleBool(DeviceButtonId buttonId, bool value)
	{
		HandleButton(device_, nextState_, delta_, buttonId, value);
	}

	void HandleFloat(DeviceButtonId buttonId, float value)
	{
		HandleAxis(device_, nextState_, delta_, buttonId, value);
	}
};

}

#endif

