
#include <gainput/gainput.h>

#if defined(GAINPUT_PLATFORM_ANDROID)

#include "GainputInputDeviceTouchAndroid.h"
#include "../GainputInputDeltaState.h"
#include "GainputTouchInfo.h"

namespace gainput
{

InputDeviceTouchImpl::InputDeviceTouchImpl(InputManager& manager, DeviceId device) :
	manager_(manager),
	device_(device),
	state_(0),
	previousState_(0),
	delta_(0)
{
}

void
InputDeviceTouchImpl::Update(InputState& state, InputState& previousState, InputDeltaState* delta)
{
	state_ = &state;
	previousState_ = &previousState;
	delta_ = delta;
}

int32_t
InputDeviceTouchImpl::HandleInput(AInputEvent* event)
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
		HandleFloat(TOUCH_0_X + i*TouchDataElems, x/float(w));
		HandleFloat(TOUCH_0_Y + i*TouchDataElems, y/float(h));
		const int motionAction = AMotionEvent_getAction(event);
		const bool down = (motionAction == AMOTION_EVENT_ACTION_DOWN || motionAction == AMOTION_EVENT_ACTION_MOVE);
		HandleBool(TOUCH_0_DOWN + i*TouchDataElems, down);
		HandleFloat(TOUCH_0_PRESSURE + i*TouchDataElems, AMotionEvent_getPressure(event, i));
#ifdef GAINPUT_DEBUG
		GAINPUT_LOG("Touch %i) x: %f, y: %f, w: %i, h: %i, action: %d\n", i, x, y, w, h, motionAction);
#endif
	}

	return 1;
}

void
InputDeviceTouchImpl::HandleBool(DeviceButtonId buttonId, bool value)
{
	state_->Set(buttonId, value);

	if (delta_)
	{
		const bool oldValue = previousState_->GetBool(buttonId);
		if (value != oldValue)
		{
			delta_->AddChange(device_, buttonId, oldValue, value);
		}
	}
}

void
InputDeviceTouchImpl::HandleFloat(DeviceButtonId buttonId, float value)
{
	if (value < -1.0f) // Because theoretical min value is -32768
	{
		value = -1.0f;
	}

	state_->Set(buttonId, value);

	if (delta_)
	{
		const float oldValue = previousState_->GetFloat(buttonId);
		if (value != oldValue)
		{
			delta_->AddChange(device_, buttonId, oldValue, value);
		}
	}
}

}

#include "GainputTouchCommon.h"

#endif

