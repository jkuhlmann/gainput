
#include <gainput/gainput.h>

#if defined(GAINPUT_PLATFORM_WIN) && defined (GAINPUT_ENABLE_RAW_INPUT)

#include "GainputInputDeviceMouseWinRaw.h"
#include "../GainputInputDeltaState.h"
#include "../GainputHelpers.h"
#include "GainputMouseInfo.h"

#include <windows.h>
#include <Windowsx.h>

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

namespace gainput
{


InputDeviceMouseImpl::InputDeviceMouseImpl(InputManager& manager, DeviceId device) :
	manager_(manager),
	device_(device),
	state_(0),
	previousState_(0),
	nextState_(manager.GetAllocator(), MouseButtonCount + MouseAxisCount),
	delta_(0),
	buttonsToReset_(manager.GetAllocator())
{
	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = 0;//RIDEV_NOLEGACY;
	Rid[0].hwndTarget = 0;
	BOOL deviceRegistered = RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
	GAINPUT_ASSERT(deviceRegistered);
}

void
InputDeviceMouseImpl::Update(InputState& state, InputState& previousState, InputDeltaState* delta)
{
	state_ = &state;
	previousState_ = &previousState;
	delta_ = delta;

	for (Array<DeviceButtonId>::const_iterator it = buttonsToReset_.begin();
			it != buttonsToReset_.end();
			++it)
	{

		HandleButton(device_, nextState_, nextState_, delta, *it, false);
	}
	buttonsToReset_.clear();

	*state_ = nextState_;
}

void
InputDeviceMouseImpl::HandleMessage(const MSG& msg)
{
	GAINPUT_ASSERT(state_);
	GAINPUT_ASSERT(previousState_);

	if (msg.message != WM_INPUT)
	{
		return;
	}

	DeviceButtonId buttonId;
	UINT dwSize = 40;
	static BYTE lpb[40];
    
	GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
    
	RAWINPUT* raw = (RAWINPUT*)lpb;
    
	if (raw->header.dwType == RIM_TYPEMOUSE) 
	{
		if (raw->data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
		{
			const float prevX = previousState_->GetFloat(MouseAxisX);
			HandleAxis(device_, nextState_, *previousState_, delta_, MouseAxisX, prevX + float(raw->data.mouse.lLastX));
			const float prevY = previousState_->GetFloat(MouseAxisY);
			HandleAxis(device_, nextState_, *previousState_, delta_, MouseAxisY, prevY + float(raw->data.mouse.lLastY));
		}
		else if (raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
		{
			HandleAxis(device_, nextState_, *previousState_, delta_, MouseAxisX, float( raw->data.mouse.lLastX));
			HandleAxis(device_, nextState_, *previousState_, delta_, MouseAxisY, float( raw->data.mouse.lLastY));
		}
		
		if (raw->data.mouse.usButtonFlags == RI_MOUSE_WHEEL)
		{
			if (SHORT(raw->data.mouse.usButtonData) < 0)
			{
				HandleButton(device_, nextState_, *previousState_, delta_, MouseButtonWheelDown, true);
				buttonsToReset_.push_back(MouseButtonWheelDown);
			}
			else if (SHORT(raw->data.mouse.usButtonData) > 0)
			{
				HandleButton(device_, nextState_, *previousState_, delta_, MouseButtonWheelUp, true);
				buttonsToReset_.push_back(MouseButtonWheelUp);
			}
		}
		else
		{
			if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN)
			{
				HandleButton(device_, nextState_, *previousState_, delta_, MouseButton0, true);
			}
			if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP)
			{
				HandleButton(device_, nextState_, *previousState_, delta_, MouseButton0, false);
			}

			if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN)
			{
				HandleButton(device_, nextState_, *previousState_, delta_, MouseButton1, true);
			}
			if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP)
			{
				HandleButton(device_, nextState_, *previousState_, delta_, MouseButton1, false);
			}

			if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN)
			{
				HandleButton(device_, nextState_, *previousState_, delta_, MouseButton2, true);
			}
			if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP)
			{
				HandleButton(device_, nextState_, *previousState_, delta_, MouseButton2, false);
			}

			if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN)
			{
				HandleButton(device_, nextState_, *previousState_, delta_, MouseButton5, true);
			}
			if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP)
			{
				HandleButton(device_, nextState_, *previousState_, delta_, MouseButton5, false);
			}

			if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN)
			{
				HandleButton(device_, nextState_, *previousState_, delta_, MouseButton6, true);
			}
			if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP)
			{
				HandleButton(device_, nextState_, *previousState_, delta_, MouseButton6, false);
			}
		}
	} 
}

}

#include "GainputMouseCommon.h"

#endif

