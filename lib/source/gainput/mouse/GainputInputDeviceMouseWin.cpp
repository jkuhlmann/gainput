
#include <gainput/gainput.h>

#if defined(GAINPUT_PLATFORM_WIN)

#include "GainputInputDeviceMouseWin.h"
#include "../GainputInputDeltaState.h"
#include "GainputMouseInfo.h"

#include <windows.h>
#include <Windowsx.h>


namespace gainput
{


InputDeviceMouseImpl::InputDeviceMouseImpl(InputManager& manager, DeviceId device) :
	manager_(manager),
	device_(device),
	state_(0),
	previousState_(0),
	delta_(0)
{
}

void
InputDeviceMouseImpl::Update(InputState& state, InputState& previousState, InputDeltaState* delta)
{
	state_ = &state;
	previousState_ = &previousState;
	delta_ = delta;

	// Reset mouse wheel buttons
	state_->Set(MOUSE_BUTTON_3, false);
	state_->Set(MOUSE_BUTTON_4, false);
	
	if (delta)
	{
		bool oldValue = previousState.GetBool(MOUSE_BUTTON_3);
		if (oldValue)
		{
			delta->AddChange(device_, MOUSE_BUTTON_3, oldValue, false);
		}
		oldValue = previousState.GetBool(MOUSE_BUTTON_4);
		if (oldValue)
		{
			delta->AddChange(device_, MOUSE_BUTTON_4, oldValue, false);
		}
	}
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
		GAINPUT_LOG("Mouse: %f, %f\n", x, y);
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
		GAINPUT_LOG("Button: %i\n", buttonId);
#endif

		if (delta_)
		{
			const bool oldValue = previousState_->GetBool(buttonId);
			if (oldValue != pressed)
			{
				delta_->AddChange(device_, buttonId, oldValue, pressed);
			}
		}
	}
}

}

#include "GainputMouseCommon.h"

#endif

