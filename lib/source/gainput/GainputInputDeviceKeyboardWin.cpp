#include <gainput/gainput.h>


#if defined(GAINPUT_PLATFORM_WIN)
#include "GainputInputDeviceKeyboardWin.h"
#include "GainputInputDeltaState.h"
#include <windows.h>
#include <Windowsx.h>
#include <iostream>


namespace gainput
{

const unsigned KeyboardButtonCount = KEY_COUNT_;
const unsigned LParamExtendedKeymask= 1 << 24;


InputDeviceKeyboardImpl::InputDeviceKeyboardImpl(InputManager& manager, DeviceId device) :
	manager_(manager),
	device_(device),
	textInputEnabled(_true),
	state_(0),
	previousState_(0),
	delta_(0),
	dialect_(manager_.GetAllocator())
{
	dialect_[VK_ESCAPE] = KEY_ESCAPE;
	dialect_[VK_F1] = KEY_F1;
	dialect_[VK_F2] = KEY_F2;
	dialect_[VK_F3] = KEY_F3;
	dialect_[VK_F4] = KEY_F4;
	dialect_[VK_F5] = KEY_F5;
	dialect_[VK_F6] = KEY_F6;
	dialect_[VK_F7] = KEY_F7;
	dialect_[VK_F8] = KEY_F8;
	dialect_[VK_F9] = KEY_F9;
	dialect_[VK_F10] = KEY_F10;
	dialect_[VK_F11] = KEY_F11;
	dialect_[VK_F12] = KEY_F12;
	dialect_[VK_PRINT] = KEY_PRINT;
	dialect_[VK_SCROLL] = KEY_SCROLL_LOCK;
	dialect_[VK_PAUSE] = KEY_BREAK;

	dialect_[VK_SPACE] = KEY_SPACE;

	dialect_[VK_OEM_5] = KEY_APOSTROPHE;
	dialect_[VK_OEM_COMMA] = KEY_COMMA;

	dialect_['0'] = KEY_0;
	dialect_['1'] = KEY_1;
	dialect_['2'] = KEY_2;
	dialect_['3'] = KEY_3;
	dialect_['4'] = KEY_4;
	dialect_['5'] = KEY_5;
	dialect_['6'] = KEY_6;
	dialect_['7'] = KEY_7;
	dialect_['8'] = KEY_8;
	dialect_['9'] = KEY_9;

	dialect_['A'] = KEY_A;
	dialect_['B'] = KEY_B;
	dialect_['C'] = KEY_C;
	dialect_['D'] = KEY_D;
	dialect_['E'] = KEY_E;
	dialect_['F'] = KEY_F;
	dialect_['G'] = KEY_G;
	dialect_['H'] = KEY_H;
	dialect_['I'] = KEY_I;
	dialect_['J'] = KEY_J;
	dialect_['K'] = KEY_K;
	dialect_['L'] = KEY_L;
	dialect_['M'] = KEY_M;
	dialect_['N'] = KEY_N;
	dialect_['O'] = KEY_O;
	dialect_['P'] = KEY_P;
	dialect_['Q'] = KEY_Q;
	dialect_['R'] = KEY_R;
	dialect_['S'] = KEY_S;
	dialect_['T'] = KEY_T;
	dialect_['U'] = KEY_U;
	dialect_['V'] = KEY_V;
	dialect_['W'] = KEY_W;
	dialect_['X'] = KEY_X;
	dialect_['Y'] = KEY_Y;
	dialect_['Z'] = KEY_Z;

	dialect_[VK_LEFT] = KEY_LEFT;
	dialect_[VK_RIGHT] = KEY_RIGHT;
	dialect_[VK_UP] = KEY_UP;
	dialect_[VK_DOWN] = KEY_DOWN;
	dialect_[VK_INSERT] = KEY_INSERT;
	dialect_[VK_HOME] = KEY_HOME;
	dialect_[VK_DELETE] = KEY_DELETE;
	dialect_[VK_END] = KEY_END;
	dialect_[VK_PRIOR] = KEY_PAGE_UP;
	dialect_[VK_NEXT] = KEY_PAGE_DOWN;

	dialect_[VK_BACK] = KEY_BACK_SPACE;
	dialect_[VK_TAB] = KEY_TAB;
	dialect_[VK_RETURN] = KEY_RETURN;
	dialect_[VK_CAPITAL] = KEY_CAPS_LOCK;
	dialect_[VK_LSHIFT] = KEY_SHIFT_L;
	dialect_[VK_LCONTROL] = KEY_CTRL_L;
	dialect_[VK_LWIN] = KEY_SUPER_L;
	dialect_[VK_LMENU] = KEY_ALT_L;
	dialect_[VK_RMENU] = KEY_ALT_R;
	dialect_[VK_RWIN] = KEY_SUPER_R;
	dialect_[VK_APPS] = KEY_MENU;
	dialect_[VK_RCONTROL] = KEY_CTRL_R;
	dialect_[VK_RSHIFT] = KEY_SHIFT_R;
}

InputDeviceKeyboardImpl::~InputDeviceKeyboardImpl()
{
}

void
InputDeviceKeyboardImpl::Update(InputState& state, InputState& previousState, InputDeltaState* delta)
{
	state_ = &state;
	previousState_ = &previousState;
	delta_ = delta;
}

void
InputDeviceKeyboardImpl::HandleMessage(const MSG& msg)
{
	GAINPUT_ASSERT(state_);
	GAINPUT_ASSERT(previousState_);

	if (msg.message == WM_CHAR)
	{
		if (!textInputEnabled_)
		{
			return;
		}

		const int key = msg.wParam;
		if (key == 0x08 // backspace 
			|| key == 0x0A // linefeed 
			|| key == 0x1B // escape 
			|| key == 0x09 // tab 
			|| key == 0x0D // carriage return 
			|| key > 255)
		{
			return;
		}
		const char charKey = key;
		textBuffer_.Put(charKey);
		char buf[256];
		sprintf(buf, "Text: %c\n", charKey);
		OutputDebugStringA(buf);
		return;
	}

	bool pressed = false;
	unsigned winKey;
	switch (msg.message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		pressed = true;
		winKey = msg.wParam;
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		pressed = false;
		winKey = msg.wParam;
		break;
	default: // Non-keyboard message
		return;
	}

	if (winKey == VK_CONTROL)
	{
		winKey = (msg.lParam & LParamExtendedKeymask) ? VK_RCONTROL : VK_LCONTROL;
	}
	else if (winKey == VK_MENU)
	{
		winKey = (msg.lParam & LParamExtendedKeymask) ? VK_RMENU : VK_LMENU;
	}
	else if (winKey == VK_SHIFT)
	{
		if (pressed)
		{
			if (GetKeyState(VK_LSHIFT) & 0x8000)
			{
				winKey = VK_LSHIFT;
			}
			else if (GetKeyState(VK_RSHIFT) & 0x8000)
			{
				winKey = VK_RSHIFT;
			}
			else
			{
				OutputDebugStringA("WHAT HAPPEND\n");
			}
		}
		else
		{
			if (previousState->GetBool(KEY_SHIFT_L) && !(GetKeyState(VK_LSHIFT) & 0x8000))
			{
				winKey = VK_LSHIFT;
			} 
			else if (previousState->GetBool(KEY_SHIFT_R) && !(GetKeyState(VK_RSHIFT) & 0x8000))
			{
				winKey = VK_RSHIFT;
			}
			else
			{
				OutputDebugStringA("WHAT HAPPEND\n");
			}
		}
	}
	// TODO handle l/r alt properly

	char buf[256];
	sprintf(buf, "Keyboard: %d, %i\n", winKey, pressed);
	OutputDebugStringA(buf);

	if (dialect_.count(winKey))
	{
		const DeviceButtonId buttonId = dialect_[winKey];
		sprintf(buf, " --> Mapped to: %d\n", buttonId);
		OutputDebugStringA(buf);
		state_->Set(buttonId, pressed);

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




InputDeviceKeyboard::InputDeviceKeyboard(InputManager& manager, DeviceId device) :
	impl_(new InputDeviceKeyboardImpl(manager, device))
{
	GAINPUT_ASSERT(impl_);
	state_ = new InputState(manager.GetAllocator(), KeyboardButtonCount);
	GAINPUT_ASSERT(state_);
	previousState_ = new InputState(manager.GetAllocator(), KeyboardButtonCount);
	GAINPUT_ASSERT(previousState_);
}

InputDeviceKeyboard::~InputDeviceKeyboard()
{
	delete state_;
	delete previousState_;
	delete impl_;
}

void
InputDeviceKeyboard::Update(InputDeltaState* delta)
{
	*previousState_ = *state_;
	impl_->Update(*state_, *previousState_, delta);
}

bool
InputDeviceKeyboard::GetAnyButtonDown(DeviceButtonId& outButtonId) const
{
	return false;
}

size_t
InputDeviceKeyboard::GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	if (bufferLength > 1 && deviceButton >= KEY_SPACE && deviceButton <= KEY_GRAVE)
	{
		buffer[0] = deviceButton;
		buffer[1] = 0;
		return 2;
	}
	return 0;
}

ButtonType
InputDeviceKeyboard::GetButtonType(DeviceButtonId deviceButton) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	return BT_BOOL;
}

bool
InputDeviceKeyboard::IsTextInputEnabled() const
{
	return impl_->IsTextInputEnabled();
}

void
InputDeviceKeyboard::SetTextInputEnabled(bool enabled)
{
	impl_->SetTextInputEnabled(enabled);
}

char
InputDeviceKeyboard::GetNextCharacter()
{
	return impl_->GetNextCharacter();
}

}

#endif
