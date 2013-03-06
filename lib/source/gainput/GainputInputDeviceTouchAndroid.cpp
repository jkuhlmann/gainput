
#include <gainput/gainput.h>

#if defined(GAINPUT_PLATFORM_ANDROID)

#include "GainputInputDeviceTouchAndroid.h"
#include "GainputInputDeltaState.h"

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

InputDeviceTouchImpl::~InputDeviceTouchImpl()
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
		state_->Set(i*TouchDataElems, false);
	}
}

int32_t
InputDeviceTouchImpl::HandleInput(AInputEvent* event)
{
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
		state_->Set(TOUCH_0_X + i*TouchDataElems, x/float(w));
		state_->Set(TOUCH_0_Y + i*TouchDataElems, y/float(h));
		state_->Set(TOUCH_0_DOWN + i*TouchDataElems, true);
		state_->Set(TOUCH_0_PRESSURE + i*TouchDataElems, AMotionEvent_getPressure(event, i));
#ifdef GAINPUT_DEBUG
		GAINPUT_LOG("%i) x: %f, y: %f, w: %i, h: %i\n", i, x, y, w, h);
#endif
	}

	return 1;
}


InputDeviceTouch::InputDeviceTouch(InputManager& manager, DeviceId device) :
	impl_(new InputDeviceTouchImpl(manager, device))
{
	GAINPUT_ASSERT(impl_);
	state_ = new InputState(manager.GetAllocator(), TouchPointCount*TouchDataElems);
	GAINPUT_ASSERT(state_);
	previousState_ = new InputState(manager.GetAllocator(), TouchPointCount*TouchDataElems);
	GAINPUT_ASSERT(previousState_);
}

InputDeviceTouch::~InputDeviceTouch()
{
	delete state_;
	delete previousState_;
	delete impl_;
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
	return CheckAllButtonsDown(outButtons, maxButtonCount, TOUCH_0_DOWN, TOUCH_COUNT, impl_->GetDevice());
}

size_t
InputDeviceTouch::GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
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

}

#endif

