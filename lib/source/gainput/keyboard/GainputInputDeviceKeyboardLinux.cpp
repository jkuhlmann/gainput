
#include <gainput/gainput.h>


#if defined(GAINPUT_PLATFORM_LINUX)

#include <X11/Xutil.h>

#include "../GainputInputDeltaState.h"
#include "GainputKeyboardKeyNames.h"


namespace gainput
{

const unsigned KeyboardButtonCount = KEY_COUNT_;

class InputDeviceKeyboardImpl
{
public:
	InputDeviceKeyboardImpl(InputManager& manager, DeviceId device) :
		manager_(manager),
		device_(device),
		textInputEnabled_(true),
		dialect_(manager_.GetAllocator()),
		keyNames_(manager_.GetAllocator())
	{
		GetKeyboardKeyNames(keyNames_);

		// Cf. <X11/keysymdef.h>
		dialect_[XK_Escape] = KEY_ESCAPE;
		dialect_[XK_F1] = KEY_F1;
		dialect_[XK_F2] = KEY_F2;
		dialect_[XK_F3] = KEY_F3;
		dialect_[XK_F4] = KEY_F4;
		dialect_[XK_F5] = KEY_F5;
		dialect_[XK_F6] = KEY_F6;
		dialect_[XK_F7] = KEY_F7;
		dialect_[XK_F8] = KEY_F8;
		dialect_[XK_F9] = KEY_F9;
		dialect_[XK_F10] = KEY_F10;
		dialect_[XK_F11] = KEY_F11;
		dialect_[XK_F12] = KEY_F12;
		dialect_[XK_Print] = KEY_PRINT;
		dialect_[XK_Scroll_Lock] = KEY_SCROLL_LOCK;
		dialect_[XK_Pause] = KEY_BREAK;

		dialect_[XK_space] = KEY_SPACE;

		dialect_[XK_apostrophe] = KEY_APOSTROPHE;
		dialect_[XK_comma] = KEY_COMMA;
		dialect_[XK_minus] = KEY_MINUS;
		dialect_[XK_period] = KEY_PERIOD;
		dialect_[XK_slash] = KEY_SLASH;

		dialect_[XK_0] = KEY_0;
		dialect_[XK_1] = KEY_1;
		dialect_[XK_2] = KEY_2;
		dialect_[XK_3] = KEY_3;
		dialect_[XK_4] = KEY_4;
		dialect_[XK_5] = KEY_5;
		dialect_[XK_6] = KEY_6;
		dialect_[XK_7] = KEY_7;
		dialect_[XK_8] = KEY_8;
		dialect_[XK_9] = KEY_9;

		dialect_[XK_semicolon] = KEY_SEMICOLON;
		dialect_[XK_less] = KEY_LESS;
		dialect_[XK_equal] = KEY_EQUAL;

		dialect_[XK_a] = KEY_A;
		dialect_[XK_b] = KEY_B;
		dialect_[XK_c] = KEY_C;
		dialect_[XK_d] = KEY_D;
		dialect_[XK_e] = KEY_E;
		dialect_[XK_f] = KEY_F;
		dialect_[XK_g] = KEY_G;
		dialect_[XK_h] = KEY_H;
		dialect_[XK_i] = KEY_I;
		dialect_[XK_j] = KEY_J;
		dialect_[XK_k] = KEY_K;
		dialect_[XK_l] = KEY_L;
		dialect_[XK_m] = KEY_M;
		dialect_[XK_n] = KEY_N;
		dialect_[XK_o] = KEY_O;
		dialect_[XK_p] = KEY_P;
		dialect_[XK_q] = KEY_Q;
		dialect_[XK_r] = KEY_R;
		dialect_[XK_s] = KEY_S;
		dialect_[XK_t] = KEY_T;
		dialect_[XK_u] = KEY_U;
		dialect_[XK_v] = KEY_V;
		dialect_[XK_w] = KEY_W;
		dialect_[XK_x] = KEY_X;
		dialect_[XK_y] = KEY_Y;
		dialect_[XK_z] = KEY_Z;

		dialect_[XK_bracketleft] = KEY_BRACKET_LEFT;
		dialect_[XK_backslash] = KEY_BACKSLASH;
		dialect_[XK_bracketright] = KEY_BRACKET_RIGHT;

		dialect_[XK_grave] = KEY_GRAVE;

		dialect_[XK_Left] = KEY_LEFT;
		dialect_[XK_Right] = KEY_RIGHT;
		dialect_[XK_Up] = KEY_UP;
		dialect_[XK_Down] = KEY_DOWN;
		dialect_[XK_Insert] = KEY_INSERT;
		dialect_[XK_Home] = KEY_HOME;
		dialect_[XK_Delete] = KEY_DELETE;
		dialect_[XK_End] = KEY_END;
		dialect_[XK_Page_Up] = KEY_PAGE_UP;
		dialect_[XK_Page_Down] = KEY_PAGE_DOWN;

		dialect_[XK_Num_Lock] = KEY_NUM_LOCK;
		dialect_[XK_KP_Divide] = KEY_KP_DIVIDE;
		dialect_[XK_KP_Multiply] = KEY_KP_MULTIPLY;
		dialect_[XK_KP_Subtract] = KEY_KP_SUBTRACT;
		dialect_[XK_KP_Add] = KEY_KP_ADD;
		dialect_[XK_KP_Enter] = KEY_KP_ENTER;
		dialect_[XK_KP_Insert] = KEY_KP_INSERT;
		dialect_[XK_KP_End] = KEY_KP_END;
		dialect_[XK_KP_Down] = KEY_KP_DOWN;
		dialect_[XK_KP_Page_Down] = KEY_KP_PAGE_DOWN;
		dialect_[XK_KP_Left] = KEY_KP_LEFT;
		dialect_[XK_KP_Begin] = KEY_KP_BEGIN;
		dialect_[XK_KP_Right] = KEY_KP_RIGHT;
		dialect_[XK_KP_Home] = KEY_KP_HOME;
		dialect_[XK_KP_Up] = KEY_KP_UP;
		dialect_[XK_KP_Page_Up] = KEY_KP_PAGE_UP;
		dialect_[XK_KP_Delete] = KEY_KP_DELETE;

		dialect_[XK_BackSpace] = KEY_BACK_SPACE;
		dialect_[XK_Tab] = KEY_TAB;
		dialect_[XK_Return] = KEY_RETURN;
		dialect_[XK_Caps_Lock] = KEY_CAPS_LOCK;
		dialect_[XK_Shift_L] = KEY_SHIFT_L;
		dialect_[XK_Control_L] = KEY_CTRL_L;
		dialect_[XK_Super_L] = KEY_SUPER_L;
		dialect_[XK_Alt_L] = KEY_ALT_L;
		dialect_[XK_Alt_R] = KEY_ALT_R;
		dialect_[XK_Super_R] = KEY_SUPER_R;
		dialect_[XK_Menu] = KEY_MENU;
		dialect_[XK_Control_R] = KEY_CTRL_R;
		dialect_[XK_Shift_R] = KEY_SHIFT_R;

		dialect_[XK_dead_circumflex] = KEY_CIRCUMFLEX;
		dialect_[XK_ssharp] = KEY_SSHARP;
		dialect_[XK_dead_acute] = KEY_ACUTE;
		dialect_[XK_ISO_Level3_Shift] = KEY_ALT_GR;
		dialect_[XK_plus] = KEY_PLUS;
		dialect_[XK_numbersign] = KEY_NUMBERSIGN;
		dialect_[XK_udiaeresis] = KEY_UDIAERESIS;
		dialect_[XK_adiaeresis] = KEY_ADIAERESIS;
		dialect_[XK_odiaeresis] = KEY_ODIAERESIS;
		dialect_[XK_section] = KEY_SECTION;
		dialect_[XK_aring] = KEY_ARING;
		dialect_[XK_dead_diaeresis] = KEY_DIAERESIS;
		dialect_[XK_twosuperior] = KEY_TWOSUPERIOR;
		dialect_[XK_parenright] = KEY_RIGHT_PARENTHESIS;
		dialect_[XK_dollar] = KEY_DOLLAR;
		dialect_[XK_ugrave] = KEY_UGRAVE;
		dialect_[XK_asterisk] = KEY_ASTERISK;
		dialect_[XK_colon] = KEY_COLON;
		dialect_[XK_exclam] = KEY_EXCLAM;
	}

	void Update(InputState& state, InputState& previousState, InputDeltaState* delta)
	{
		const long eventMask = KeyPressMask | KeyReleaseMask;
		XEvent event;
		while (XCheckMaskEvent(manager_.GetXDisplay(), eventMask, &event))
		{
			switch (event.type)
			{
			case KeyPress:
			case KeyRelease:
				{
					XKeyEvent& keyEvent = event.xkey;
					KeySym keySym = XKeycodeToKeysym(manager_.GetXDisplay(), keyEvent.keycode, 0);
					const bool pressed = event.type == KeyPress;

					if (dialect_.count(keySym))
					{
						const DeviceButtonId buttonId = dialect_[keySym];
						state.Set(buttonId, pressed);
#ifdef GAINPUT_DEBUG
						GAINPUT_LOG("buttonId: %d\n", buttonId);
#endif

						if (delta)
						{
							const bool oldValue = previousState.GetBool(buttonId);
							if (oldValue != pressed)
							{
								delta->AddChange(device_, buttonId, oldValue, pressed);
							}
						}
					}
#ifdef GAINPUT_DEBUG
					else
					{
						char* str = XKeysymToString(keySym);
						GAINPUT_LOG("Unmapped key >> keycode: %d, keysym: %d, str: %s\n", keyEvent.keycode, int(keySym), str);
					}
#endif



					if (textInputEnabled_)
					{
						char buf[32];
						int len = XLookupString(&keyEvent, buf, 32, 0, 0);
						if (len == 1)
						{
							textBuffer_.Put(buf[0]);
						}
					}
					break;
				}
			}
		}
	}

	InputManager& GetManager() const { return manager_; }
	DeviceId GetDevice() const { return device_; }

	size_t GetKeyName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const
	{
		HashMap<Key, const char*>::const_iterator it = keyNames_.find(Key(deviceButton));
		if (it == keyNames_.end())
		{
			return 0;
		}
		strncpy(buffer, it->second, bufferLength);
		buffer[bufferLength-1] = 0;
		const size_t nameLen = strlen(it->second);
		return nameLen >= bufferLength ? bufferLength : nameLen+1;
	}

	bool IsTextInputEnabled() const { return textInputEnabled_; }
	void SetTextInputEnabled(bool enabled) { textInputEnabled_ = enabled; }

	char GetNextCharacter()
	{
		if (!textBuffer_.CanGet())
		{
			return 0;
		}
		return textBuffer_.Get();
	}

private:
	InputManager& manager_;
	DeviceId device_;
	bool textInputEnabled_;
	RingBuffer<GAINPUT_TEXT_INPUT_QUEUE_LENGTH, char> textBuffer_;
	HashMap<unsigned, DeviceButtonId> dialect_;
	HashMap<Key, const char*> keyNames_;
};



InputDeviceKeyboard::InputDeviceKeyboard(InputManager& manager, DeviceId device)
{
	impl_ = manager.GetAllocator().New<InputDeviceKeyboardImpl>(manager, device);
	GAINPUT_ASSERT(impl_);
	state_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), KeyboardButtonCount);
	GAINPUT_ASSERT(state_);
	previousState_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), KeyboardButtonCount);
	GAINPUT_ASSERT(previousState_);
}

InputDeviceKeyboard::~InputDeviceKeyboard()
{
	impl_->GetManager().GetAllocator().Delete(state_);
	impl_->GetManager().GetAllocator().Delete(previousState_);
	impl_->GetManager().GetAllocator().Delete(impl_);
}

void
InputDeviceKeyboard::Update(InputDeltaState* delta)
{
	*previousState_ = *state_;
	impl_->Update(*state_, *previousState_, delta);
}

size_t
InputDeviceKeyboard::GetAnyButtonDown(DeviceButtonSpec* outButtons, size_t maxButtonCount) const
{
	return CheckAllButtonsDown(outButtons, maxButtonCount, 0, KeyboardButtonCount, impl_->GetDevice());
}

size_t
InputDeviceKeyboard::GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	return impl_->GetKeyName(deviceButton, buffer, bufferLength);
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
