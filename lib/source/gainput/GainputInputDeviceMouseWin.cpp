
#include <gainput/gainput.h>

#if defined(GAINPUT_PLATFORM_WIN)

#include "GainputInputDeviceMouseWin.h"
#include "GainputInputDeltaState.h"
#include <windows.h>
#include <Windowsx.h>
#include <iostream>


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
		{ BT_BOOL, "mouse_left" },
		{ BT_BOOL, "mouse_middle" },
		{ BT_BOOL, "mouse_right" },
		{ BT_BOOL, "mouse_3" },
		{ BT_BOOL, "mouse_4" },
		{ BT_BOOL, "mouse_5" },
		{ BT_BOOL, "mouse_6" },
		{ BT_BOOL, "mouse_7" },
		{ BT_BOOL, "mouse_8" },
		{ BT_BOOL, "mouse_9" },
		{ BT_BOOL, "mouse_10" },
		{ BT_BOOL, "mouse_11" },
		{ BT_BOOL, "mouse_12" },
		{ BT_BOOL, "mouse_13" },
		{ BT_BOOL, "mouse_14" },
		{ BT_BOOL, "mouse_15" },
		{ BT_BOOL, "mouse_16" },
		{ BT_BOOL, "mouse_17" },
		{ BT_BOOL, "mouse_18" },
		{ BT_BOOL, "mouse_19" },
		{ BT_BOOL, "mouse_20" },
		{ BT_FLOAT, "mouse_x" },
		{ BT_FLOAT, "mouse_y" }
};
}

const unsigned MouseButtonCount = 20;
const unsigned MouseAxisCount = 2;


InputDeviceMouseImpl::InputDeviceMouseImpl(InputManager& manager, DeviceId device) :
	manager_(manager),
	device_(device),
	state_(0),
	previousState_(0),
	delta_(0)
{
}

InputDeviceMouseImpl::~InputDeviceMouseImpl()
{
}

void
InputDeviceMouseImpl::Update(InputState& state, InputState& previousState, InputDeltaState* delta)
{
	state_ = &state;
	previousState_ = &previousState;
	delta_ = delta;

	// Reset mouse wheel buttons
	state_.Set(MOUSE_BUTTON_3, false);
	state_.Set(MOUSE_BUTTON_4, false);
	// TODO respect delta
}

void
InputDeviceMouseImpl::HandleMessage(const MSG& msg)
{
	GAINPUT_ASSERT(state_);
	GAINPUT_ASSERT(previousState_);

	DeviceButtonId buttonId;
	bool pressed;
	bool moveMessage = false;
	int ax = -1;
	int ay = -1;
	switch (msg.message)
	{
	case WM_LBUTTONDOWN:
		buttonId = MOUSE_BUTTON_LEFT;
		pressed = true;
		break;
	case WM_LBUTTONUP:
		buttonId = MOUSE_BUTTON_LEFT;
		pressed = false;
		break;
	case WM_RBUTTONDOWN:
		buttonId = MOUSE_BUTTON_RIGHT;
		pressed = true;
		break;
	case WM_RBUTTONUP:
		buttonId = MOUSE_BUTTON_RIGHT;
		pressed = false;
		break;
	case WM_MBUTTONDOWN:
		buttonId = MOUSE_BUTTON_MIDDLE;
		pressed = true;
		break;
	case WM_MBUTTONUP:
		buttonId = MOUSE_BUTTON_MIDDLE;
		pressed = false;
		break;
	case WM_XBUTTONDOWN:
		buttonId = MOUSE_BUTTON_4 + GET_XBUTTON_WPARAM(msg.wParam);
		pressed = true;
		break;
	case WM_XBUTTONUP:
		buttonId = MOUSE_BUTTON_4 + GET_XBUTTON_WPARAM(msg.wParam);
		pressed = false;
		break;
	case WM_MOUSEMOVE:
		moveMessage = true;
		ax = GET_X_LPARAM(msg.lParam);
		ay = GET_Y_LPARAM(msg.lParam);
		break;
	case WM_MOUSEWHEEL:
		{
			int wheel = GET_WHEEL_DELTA_WPARAM(msg.wParam);
			if (wheel < 0)
			{
				buttonId = MOUSE_BUTTON_4;
				pressed = true;
			}
			else if (wheel > 0)
			{
				buttonId = MOUSE_BUTTON_3;
				pressed = true;
			}
			break;
		}
	default: // Non-mouse message
		return;
	}

	if (moveMessage)
	{
		float x = float(ax)/float(manager_.GetDisplayWidth());
		float y = float(ay)/float(manager_.GetDisplayHeight());
		state_->Set(MOUSE_AXIS_X, x);
		state_->Set(MOUSE_AXIS_Y, y);

#ifdef GAINPUT_DEBUG
		char buf[256];
		sprintf(buf, "Mouse: %f, %f\n", x, y);
		OutputDebugStringA(buf);
#endif

		if (delta_)
		{
			const float oldX = previousState_->GetFloat(MOUSE_AXIS_X);
			const float oldY = previousState_->GetFloat(MOUSE_AXIS_Y);
			if (oldX != x)
			{
				delta_->AddChange(device_, MOUSE_AXIS_X, oldX, x);
			}
			if (oldY != y)
			{
				delta_->AddChange(device_, MOUSE_AXIS_Y, oldY, y);
			}
		}
	}
	else
	{
		state_->Set(buttonId, pressed);

#ifdef GAINPUT_DEBUG
		char buf[256];
		sprintf(buf, "Button: %i\n", buttonId);
		OutputDebugStringA(buf);
#endif

		if (delta)
		{
			const bool oldValue = previousState_->GetBool(buttonId);
			if (oldValue != pressed)
			{
				delta_->AddChange(device_, buttonId, oldValue, pressed);
			}
		}
	}
}




InputDeviceMouse::InputDeviceMouse(InputManager& manager, DeviceId device) :
	impl_(new InputDeviceMouseImpl(manager, device))
{
	GAINPUT_ASSERT(impl_);
	state_ = new InputState(manager.GetAllocator(), MouseButtonCount + MouseAxisCount);
	GAINPUT_ASSERT(state_);
	previousState_ = new InputState(manager.GetAllocator(), MouseButtonCount + MouseAxisCount);
	GAINPUT_ASSERT(previousState_);
}

InputDeviceMouse::~InputDeviceMouse()
{
	delete state_;
	delete previousState_;
	delete impl_;
}

void
InputDeviceMouse::Update(InputDeltaState* delta)
{
	*previousState_ = *state_;
	impl_->Update(*state_, *previousState_, delta);
}

bool
InputDeviceMouse::GetAnyButtonDown(DeviceButtonId& outButtonId) const
{
	for (unsigned i = MOUSE_BUTTON_0; i < MOUSE_BUTTON_MAX; ++i)
	{
		DeviceButtonId id(i);
		if (GetButtonType(id) == BT_BOOL
				&& GetBool(id))
		{
			outButtonId = id;
			return true;
		}
	}
	return false;
}

void
InputDeviceMouse::GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	strncpy(buffer, deviceButtonInfos[deviceButton].name, bufferLength);
}

ButtonType
InputDeviceMouse::GetButtonType(DeviceButtonId deviceButton) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	return deviceButtonInfos[deviceButton].type;
}

}

#endif

