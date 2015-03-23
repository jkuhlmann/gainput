
#ifndef GAINPUTINPUTDEVICETOUCHANDROID_H_
#define GAINPUTINPUTDEVICETOUCHANDROID_H_

#include <android/native_activity.h>

#include "GainputInputDeviceTouchImpl.h"
#include "GainputTouchInfo.h"

namespace gainput
{

class InputDeviceTouchImplAndroid : public InputDeviceTouchImpl
{
public:
	InputDeviceTouchImplAndroid(InputManager& manager, DeviceId device, InputState& state, InputState& previousState) :
		manager_(manager),
		device_(device),
		state_(&state),
		previousState_(&previousState),
		nextState_(manager.GetAllocator(), TouchPointCount*TouchDataElems),
		delta_(0)
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

	int32_t HandleInput(AInputEvent* event)
	{
		GAINPUT_ASSERT(state_);
		GAINPUT_ASSERT(previousState_);
		GAINPUT_ASSERT(event);

		if (AInputEvent_getType(event) != AINPUT_EVENT_TYPE_MOTION)
		{
			return 0;
		}

		for (unsigned i = 0; i < AMotionEvent_getPointerCount(event) && i < TouchPointCount; ++i)
		{
			GAINPUT_ASSERT(i < TouchPointCount);
			const float x = AMotionEvent_getX(event, i);
			const float y = AMotionEvent_getY(event, i);
			const int32_t w = manager_.GetDisplayWidth();
			const int32_t h = manager_.GetDisplayHeight();
			HandleFloat(Touch0X + i*TouchDataElems, x/float(w));
			HandleFloat(Touch0Y + i*TouchDataElems, y/float(h));
			const int motionAction = AMotionEvent_getAction(event);
			const bool down = (motionAction == AMOTION_EVENT_ACTION_DOWN || motionAction == AMOTION_EVENT_ACTION_MOVE);
			HandleBool(Touch0Down + i*TouchDataElems, down);
			HandleFloat(Touch0Pressure + i*TouchDataElems, AMotionEvent_getPressure(event, i));
#ifdef GAINPUT_DEBUG
			GAINPUT_LOG("Touch %i) x: %f, y: %f, w: %i, h: %i, action: %d\n", i, x, y, w, h, motionAction);
#endif
		}

		return 1;
	}

	void HandleInput(int id, int action, int x, int y)
	{
		GAINPUT_ASSERT(state_);
		GAINPUT_ASSERT(previousState_);
		GAINPUT_ASSERT(id < TouchPointCount);
		const int w = manager_.GetDisplayWidth();
		const int h = manager_.GetDisplayHeight();
		HandleFloat(Touch0X + id*TouchDataElems, float(x)/float(w));
		HandleFloat(Touch0Y + id*TouchDataElems, float(y)/float(h));
		const bool down = (action == 0 || action == 2);
		HandleBool(Touch0Down + id*TouchDataElems, down);
		HandleFloat(Touch0Pressure + id*TouchDataElems, down ? 1.0f : 0.0f);
#ifdef GAINPUT_DEBUG
		GAINPUT_LOG("Touch %i) x: %f, y: %f, w: %i, h: %i, action: %d\n", id, x, y, w, h, action);
#endif
	}

private:
	InputManager& manager_;
	DeviceId device_;
	InputState* state_;
	InputState* previousState_;
	InputState nextState_;
	InputDeltaState* delta_;

	void HandleBool(DeviceButtonId buttonId, bool value)
	{
		if (delta_)
		{
			const bool oldValue = nextState_.GetBool(buttonId);
			if (value != oldValue)
			{
				delta_->AddChange(device_, buttonId, oldValue, value);
			}
		}

		nextState_.Set(buttonId, value);
	}

	void HandleFloat(DeviceButtonId buttonId, float value)
	{
		if (value < -1.0f)
		{
			value = -1.0f;
		}

		if (delta_)
		{
			const float oldValue = nextState_.GetFloat(buttonId);
			if (value != oldValue)
			{
				delta_->AddChange(device_, buttonId, oldValue, value);
			}
		}

		nextState_.Set(buttonId, value);
	}
};

}

#endif

