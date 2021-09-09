
#ifndef GAINPUTINPUTDEVICEMOUSEWIN_H_
#define GAINPUTINPUTDEVICEMOUSEWIN_H_

#include "GainputInputDeviceMouseImpl.h"
#include <gainput/GainputHelpers.h>

#include "../GainputWindows.h"

namespace gainput
{

class InputDeviceMouseImplWin : public InputDeviceMouseImpl
{
public:
	InputDeviceMouseImplWin(InputManager& manager, InputDevice& device, InputState& state, InputState& previousState) :
		manager_(manager),
		device_(device),
		state_(&state),
		previousState_(&previousState),
		nextState_(manager.GetAllocator(), MouseButtonCount + MouseAxisCount),
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

		// Reset mouse wheel buttons
		auto resetWheel = [&](const DeviceButtonId buttonId) {
			const bool oldValue = previousState_->GetBool(buttonId);
			if (oldValue)
			{
				const bool pressed = false;
				HandleButton(device_, nextState_, delta_, buttonId, pressed);
			}
		};
		resetWheel(MouseButton3);
		resetWheel(MouseButton4);

		*state_ = nextState_;
	}

	void HandleMessage(const MSG& msg)
	{
		GAINPUT_ASSERT(state_);
		GAINPUT_ASSERT(previousState_);

		DeviceButtonId buttonId;
		bool pressed = false;
		bool moveMessage = false;
		int ax = -1;
		int ay = -1;
		switch (msg.message)
		{
		case WM_LBUTTONDOWN:
			buttonId = MouseButtonLeft;
			pressed = true;
			break;
		case WM_LBUTTONUP:
			buttonId = MouseButtonLeft;
			pressed = false;
			break;
		case WM_RBUTTONDOWN:
			buttonId = MouseButtonRight;
			pressed = true;
			break;
		case WM_RBUTTONUP:
			buttonId = MouseButtonRight;
			pressed = false;
			break;
		case WM_MBUTTONDOWN:
			buttonId = MouseButtonMiddle;
			pressed = true;
			break;
		case WM_MBUTTONUP:
			buttonId = MouseButtonMiddle;
			pressed = false;
			break;
		case WM_XBUTTONDOWN:
			buttonId = MouseButton4 + GET_XBUTTON_WPARAM(msg.wParam);
			pressed = true;
			break;
		case WM_XBUTTONUP:
			buttonId = MouseButton4 + GET_XBUTTON_WPARAM(msg.wParam);
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
					buttonId = MouseButton4;
					pressed = true;
				}
				else if (wheel > 0)
				{
					buttonId = MouseButton3;
					pressed = true;
				}
				else
				{
					return;
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
			HandleAxis(device_, nextState_, delta_, MouseAxisX, x);
			HandleAxis(device_, nextState_, delta_, MouseAxisY, y);
		}
		else
		{
			HandleButton(device_, nextState_, delta_, buttonId, pressed);
		}
	}

private:
	InputManager& manager_;
	InputDevice& device_;
	InputState* state_;
	InputState* previousState_;
	InputState nextState_;
	InputDeltaState* delta_;
};

}

#endif

