
#include <gainput/gainput.h>

#if defined(GAINPUT_PLATFORM_WIN)

#include "GainputInputDeviceMouseWin.h"
#include "../GainputInputDeltaState.h"

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
	state_->Set(MOUSE_BUTTON_3, false);
	state_->Set(MOUSE_BUTTON_4, false);
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




InputDeviceMouse::InputDeviceMouse(InputManager& manager, DeviceId device)
{
	impl_ = manager.GetAllocator().New<InputDeviceMouseImpl>(manager, device);
	GAINPUT_ASSERT(impl_);
	state_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), MouseButtonCount + MouseAxisCount);
	GAINPUT_ASSERT(state_);
	previousState_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), MouseButtonCount + MouseAxisCount);
	GAINPUT_ASSERT(previousState_);
}

InputDeviceMouse::~InputDeviceMouse()
{
	impl_->GetManager().GetAllocator().Delete(state_);
	impl_->GetManager().GetAllocator().Delete(previousState_);
	impl_->GetManager().GetAllocator().Delete(impl_);
}

void
InputDeviceMouse::Update(InputDeltaState* delta)
{
	*previousState_ = *state_;
	impl_->Update(*state_, *previousState_, delta);
}

size_t
InputDeviceMouse::GetAnyButtonDown(DeviceButtonSpec* outButtons, size_t maxButtonCount) const
{
	return CheckAllButtonsDown(outButtons, maxButtonCount, MOUSE_BUTTON_0, MOUSE_BUTTON_MAX, impl_->GetDevice());
}

size_t
InputDeviceMouse::GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	strncpy(buffer, deviceButtonInfos[deviceButton].name, bufferLength);
	buffer[bufferLength-1] = 0;
	const size_t nameLen = strlen(deviceButtonInfos[deviceButton].name);
	return nameLen >= bufferLength ? bufferLength : nameLen+1;
}

ButtonType
InputDeviceMouse::GetButtonType(DeviceButtonId deviceButton) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	return deviceButtonInfos[deviceButton].type;
}

DeviceButtonId
InputDeviceMouse::GetButtonByName(const char* name) const
{
	for (unsigned i = 0; i < MouseButtonCount + MouseAxisCount; ++i)
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

