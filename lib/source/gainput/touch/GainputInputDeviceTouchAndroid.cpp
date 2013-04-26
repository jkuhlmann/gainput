
#include <gainput/gainput.h>

#if defined(GAINPUT_PLATFORM_ANDROID)

#include "GainputInputDeviceTouchAndroid.h"
#include "../GainputInputDeltaState.h"

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
	{ BT_BOOL, "touch_0_down" },
	{ BT_FLOAT, "touch_0_x" },
	{ BT_FLOAT, "touch_0_y" },
	{ BT_FLOAT, "touch_0_pressure" },
	{ BT_BOOL, "touch_1_down" },
	{ BT_FLOAT, "touch_1_x" },
	{ BT_FLOAT, "touch_1_y" },
	{ BT_FLOAT, "touch_1_pressure" },
	{ BT_BOOL, "touch_2_down" },
	{ BT_FLOAT, "touch_2_x" },
	{ BT_FLOAT, "touch_2_y" },
	{ BT_FLOAT, "touch_2_pressure" },
	{ BT_BOOL, "touch_3_down" },
	{ BT_FLOAT, "touch_3_x" },
	{ BT_FLOAT, "touch_3_y" },
	{ BT_FLOAT, "touch_3_pressure" },
	{ BT_BOOL, "touch_4_down" },
	{ BT_FLOAT, "touch_4_x" },
	{ BT_FLOAT, "touch_4_y" },
	{ BT_FLOAT, "touch_4_pressure" },
	{ BT_BOOL, "touch_5_down" },
	{ BT_FLOAT, "touch_5_x" },
	{ BT_FLOAT, "touch_5_y" },
	{ BT_FLOAT, "touch_5_pressure" },
	{ BT_BOOL, "touch_6_down" },
	{ BT_FLOAT, "touch_6_x" },
	{ BT_FLOAT, "touch_6_y" },
	{ BT_FLOAT, "touch_6_pressure" },
	{ BT_BOOL, "touch_7_down" },
	{ BT_FLOAT, "touch_7_x" },
	{ BT_FLOAT, "touch_7_y" },
	{ BT_FLOAT, "touch_7_pressure" }
};
}

const unsigned TouchPointCount = 8;
const unsigned TouchDataElems = 4;


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

	// Reset downs
	for (unsigned i = 0; i < TouchPointCount; ++i)
	{
		HandleBool(TOUCH_0_DOWN + i*TouchDataElems, false);
	}
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
		HandleBool(TOUCH_0_DOWN + i*TouchDataElems, true);
		HandleFloat(TOUCH_0_PRESSURE + i*TouchDataElems, AMotionEvent_getPressure(event, i));
#ifdef GAINPUT_DEBUG
		GAINPUT_LOG("Touch %i) x: %f, y: %f, w: %i, h: %i\n", i, x, y, w, h);
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


InputDeviceTouch::InputDeviceTouch(InputManager& manager, DeviceId device) :
	InputDevice(device)
{
	impl_ = manager.GetAllocator().New<InputDeviceTouchImpl>(manager, device);
	GAINPUT_ASSERT(impl_);
	state_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), TouchPointCount*TouchDataElems);
	GAINPUT_ASSERT(state_);
	previousState_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), TouchPointCount*TouchDataElems);
	GAINPUT_ASSERT(previousState_);
}

InputDeviceTouch::~InputDeviceTouch()
{
	impl_->GetManager().GetAllocator().Delete(state_);
	impl_->GetManager().GetAllocator().Delete(previousState_);
	impl_->GetManager().GetAllocator().Delete(impl_);
}

void
InputDeviceTouch::Update(InputDeltaState* delta)
{
	*previousState_ = *state_;
	impl_->Update(*state_, *previousState_, delta);
}

size_t
InputDeviceTouch::GetAnyButtonDown(DeviceButtonSpec* outButtons, size_t maxButtonCount) const
{
	GAINPUT_ASSERT(outButtons);
	GAINPUT_ASSERT(maxButtonCount > 0);
	return CheckAllButtonsDown(outButtons, maxButtonCount, TOUCH_0_DOWN, TOUCH_COUNT, impl_->GetDevice());
}

size_t
InputDeviceTouch::GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const
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
InputDeviceTouch::GetButtonType(DeviceButtonId deviceButton) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	return deviceButtonInfos[deviceButton].type;
}

DeviceButtonId
InputDeviceTouch::GetButtonByName(const char* name) const
{
	GAINPUT_ASSERT(name);
	for (unsigned i = 0; i < TouchPointCount*TouchDataElems; ++i)
	{
		if (strcmp(name, deviceButtonInfos[i].name) == 0)
		{
			return DeviceButtonId(i);
		}
	}
	return InvalidDeviceButtonId;
}

}

#endif

