#include <gainput/gainput.h>

#if defined(GAINPUT_PLATFORM_ANDROID)
#include "GainputInputDeviceKeyboardAndroid.h"
#include "GainputKeyboardKeyNames.h"
#include "../GainputInputDeltaState.h"


namespace gainput
{

const unsigned KeyboardButtonCount = KEY_COUNT_;


InputDeviceKeyboardImpl::InputDeviceKeyboardImpl(InputManager& manager, DeviceId device) :
	manager_(manager),
	device_(device),
	textInputEnabled_(true),
	dialect_(manager_.GetAllocator()),
	keyNames_(manager_.GetAllocator()),
	state_(0),
	previousState_(0),
	delta_(0)
{
	GetKeyboardKeyNames(keyNames_);

	dialect_[AKEYCODE_SPACE] = KEY_SPACE;

	dialect_[AKEYCODE_APOSTROPHE] = KEY_APOSTROPHE;
	dialect_[AKEYCODE_COMMA] = KEY_COMMA;

	dialect_[AKEYCODE_0] = KEY_0;
	dialect_[AKEYCODE_1] = KEY_1;
	dialect_[AKEYCODE_2] = KEY_2;
	dialect_[AKEYCODE_3] = KEY_3;
	dialect_[AKEYCODE_4] = KEY_4;
	dialect_[AKEYCODE_5] = KEY_5;
	dialect_[AKEYCODE_6] = KEY_6;
	dialect_[AKEYCODE_7] = KEY_7;
	dialect_[AKEYCODE_8] = KEY_8;
	dialect_[AKEYCODE_9] = KEY_9;

	dialect_[AKEYCODE_A] = KEY_A;
	dialect_[AKEYCODE_B] = KEY_B;
	dialect_[AKEYCODE_C] = KEY_C;
	dialect_[AKEYCODE_D] = KEY_D;
	dialect_[AKEYCODE_E] = KEY_E;
	dialect_[AKEYCODE_F] = KEY_F;
	dialect_[AKEYCODE_G] = KEY_G;
	dialect_[AKEYCODE_H] = KEY_H;
	dialect_[AKEYCODE_I] = KEY_I;
	dialect_[AKEYCODE_J] = KEY_J;
	dialect_[AKEYCODE_K] = KEY_K;
	dialect_[AKEYCODE_L] = KEY_L;
	dialect_[AKEYCODE_M] = KEY_M;
	dialect_[AKEYCODE_N] = KEY_N;
	dialect_[AKEYCODE_O] = KEY_O;
	dialect_[AKEYCODE_P] = KEY_P;
	dialect_[AKEYCODE_Q] = KEY_Q;
	dialect_[AKEYCODE_R] = KEY_R;
	dialect_[AKEYCODE_S] = KEY_S;
	dialect_[AKEYCODE_T] = KEY_T;
	dialect_[AKEYCODE_U] = KEY_U;
	dialect_[AKEYCODE_V] = KEY_V;
	dialect_[AKEYCODE_W] = KEY_W;
	dialect_[AKEYCODE_X] = KEY_X;
	dialect_[AKEYCODE_Y] = KEY_Y;
	dialect_[AKEYCODE_Z] = KEY_Z;

	dialect_[AKEYCODE_DPAD_LEFT] = KEY_LEFT;
	dialect_[AKEYCODE_DPAD_RIGHT] = KEY_RIGHT;
	dialect_[AKEYCODE_DPAD_UP] = KEY_UP;
	dialect_[AKEYCODE_DPAD_DOWN] = KEY_DOWN;
	dialect_[AKEYCODE_HOME] = KEY_HOME;
	dialect_[AKEYCODE_DEL] = KEY_DELETE;
	dialect_[AKEYCODE_PAGE_UP] = KEY_PAGE_UP;
	dialect_[AKEYCODE_PAGE_DOWN] = KEY_PAGE_DOWN;

	dialect_[AKEYCODE_TAB] = KEY_TAB;
	dialect_[AKEYCODE_ENTER] = KEY_RETURN;
	dialect_[AKEYCODE_SHIFT_LEFT] = KEY_SHIFT_L;
	dialect_[AKEYCODE_ALT_LEFT] = KEY_ALT_L;
	dialect_[AKEYCODE_ALT_RIGHT] = KEY_ALT_R;
	dialect_[AKEYCODE_MENU] = KEY_MENU;
	dialect_[AKEYCODE_SHIFT_RIGHT] = KEY_SHIFT_R;

	dialect_[AKEYCODE_BACKSLASH] = KEY_BACKSLASH;
	dialect_[AKEYCODE_LEFT_BRACKET] = KEY_BRACKET_LEFT;
	dialect_[AKEYCODE_RIGHT_BRACKET] = KEY_BRACKET_RIGHT;
	dialect_[AKEYCODE_NUM] = KEY_NUM_LOCK;
	dialect_[AKEYCODE_GRAVE] = KEY_GRAVE;

	dialect_[AKEYCODE_BACK] = KEY_BACK;
	dialect_[AKEYCODE_SOFT_LEFT] = KEY_SOFT_LEFT;
	dialect_[AKEYCODE_SOFT_RIGHT] = KEY_SOFT_RIGHT;
	dialect_[AKEYCODE_CALL] = KEY_CALL;
	dialect_[AKEYCODE_ENDCALL] = KEY_ENDCALL;
	dialect_[AKEYCODE_STAR] = KEY_STAR;
	dialect_[AKEYCODE_POUND] = KEY_POUND;
	dialect_[AKEYCODE_DPAD_CENTER] = KEY_DPAD_CENTER;
	dialect_[AKEYCODE_VOLUME_UP] = KEY_VOLUME_UP;
	dialect_[AKEYCODE_VOLUME_DOWN] = KEY_VOLUME_DOWN;
	dialect_[AKEYCODE_POWER] = KEY_POWER;
	dialect_[AKEYCODE_CAMERA] = KEY_CAMERA;
	dialect_[AKEYCODE_CLEAR] = KEY_CLEAR;
	dialect_[AKEYCODE_PERIOD] = KEY_PERIOD;
	dialect_[AKEYCODE_SYM] = KEY_SYM;
	dialect_[AKEYCODE_EXPLORER] = KEY_EXPLORER;
	dialect_[AKEYCODE_ENVELOPE] = KEY_ENVELOPE;
	dialect_[AKEYCODE_MINUS] = KEY_MINUS;
	dialect_[AKEYCODE_EQUALS] = KEY_EQUALS;
	dialect_[AKEYCODE_SEMICOLON] = KEY_SEMICOLON;
	dialect_[AKEYCODE_SLASH] = KEY_SLASH;
	dialect_[AKEYCODE_AT] = KEY_AT;
	dialect_[AKEYCODE_HEADSETHOOK] = KEY_HEADSETHOOK;
	dialect_[AKEYCODE_FOCUS] = KEY_FOCUS;
	dialect_[AKEYCODE_PLUS] = KEY_PLUS;
	dialect_[AKEYCODE_NOTIFICATION] = KEY_NOTIFICATION;
	dialect_[AKEYCODE_SEARCH] = KEY_SEARCH;
	dialect_[AKEYCODE_MEDIA_PLAY_PAUSE] = KEY_MEDIA_PLAY_PAUSE;
	dialect_[AKEYCODE_MEDIA_STOP] = KEY_MEDIA_STOP;
	dialect_[AKEYCODE_MEDIA_NEXT] = KEY_MEDIA_NEXT;
	dialect_[AKEYCODE_MEDIA_PREVIOUS] = KEY_MEDIA_PREVIOUS;
	dialect_[AKEYCODE_MEDIA_REWIND] = KEY_MEDIA_REWIND;
	dialect_[AKEYCODE_MEDIA_FAST_FORWARD] = KEY_MEDIA_FAST_FORWARD;
	dialect_[AKEYCODE_MUTE] = KEY_MUTE;
	dialect_[AKEYCODE_PICTSYMBOLS] = KEY_PICTSYMBOLS;
	dialect_[AKEYCODE_SWITCH_CHARSET] = KEY_SWITCH_CHARSET;
}

void
InputDeviceKeyboardImpl::Update(InputState& state, InputState& previousState, InputDeltaState* delta)
{
	state_ = &state;
	previousState_ = &previousState;
	delta_ = delta;
}

int32_t
InputDeviceKeyboardImpl::HandleInput(AInputEvent* event)
{
	GAINPUT_ASSERT(event);
	GAINPUT_ASSERT(state_);

	if (AInputEvent_getType(event) != AINPUT_EVENT_TYPE_KEY)
	{
		return 0;
	}

	const bool pressed = AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN;
	const int32_t keyCode = AKeyEvent_getKeyCode(event);
	if (dialect_.count(keyCode))
	{
		const DeviceButtonId buttonId = dialect_[keyCode];

#ifdef GAINPUT_DEBUG
		GAINPUT_LOG("%d --> %d: %d\n", keyCode, buttonId, pressed);
#endif
		state_->Set(buttonId, pressed);

		if (delta_)
		{
			GAINPUT_ASSERT(previousState_);
			const bool oldValue = previousState_->GetBool(buttonId);
			if (oldValue != pressed)
			{
				delta_->AddChange(device_, buttonId, oldValue, pressed);
			}
		}

		return 1;
	}

	return 0;
}



size_t
InputDeviceKeyboardImpl::GetKeyName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const
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

DeviceButtonId
InputDeviceKeyboardImpl::GetButtonByName(const char* name) const
{
	for (HashMap<Key, const char*>::const_iterator it = keyNames_.begin();
			it != keyNames_.end();
			++it)
	{
		if (strcmp(name, it->second) == 0)
		{
			return it->first;
		}
	}
	return InvalidDeviceButtonId;
}



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
	GAINPUT_ASSERT(outButtons);
	GAINPUT_ASSERT(maxButtonCount > 0);
	return CheckAllButtonsDown(outButtons, maxButtonCount, 0, KeyboardButtonCount, impl_->GetDevice());
}

size_t
InputDeviceKeyboard::GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	GAINPUT_ASSERT(buffer);
	GAINPUT_ASSERT(bufferLength > 0);
	return impl_->GetKeyName(deviceButton, buffer, bufferLength);
}

ButtonType
InputDeviceKeyboard::GetButtonType(DeviceButtonId deviceButton) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	return BT_BOOL;
}

DeviceButtonId
InputDeviceKeyboard::GetButtonByName(const char* name) const
{
	GAINPUT_ASSERT(name);
	return impl_->GetButtonByName(name);
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
