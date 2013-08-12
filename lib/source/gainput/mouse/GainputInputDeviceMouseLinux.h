
#ifndef GAINPUTINPUTDEVICEMOUSELINUX_H_
#define GAINPUTINPUTDEVICEMOUSELINUX_H_

#include "GainputInputDeviceMouseImpl.h"

namespace gainput
{

class InputDeviceMouseImplLinux : public InputDeviceMouseImpl
{
public:
	InputDeviceMouseImplLinux(InputManager& manager, DeviceId device) :
		manager_(manager),
		device_(device),
		state_(0),
		previousState_(0),
		nextState_(manager.GetAllocator(), MouseButtonCount + MouseAxisCount),
		delta_(0)
	{
		const size_t size = sizeof(bool)*MouseButtonCount;
		isWheel_ = static_cast<bool*>(manager_.GetAllocator().Allocate(size));
		GAINPUT_ASSERT(isWheel_);
		memset(isWheel_, 0, size);
		pressedThisFrame_ = static_cast<bool*>(manager_.GetAllocator().Allocate(size));
		GAINPUT_ASSERT(pressedThisFrame_);
	}

	~InputDeviceMouseImplLinux()
	{
		manager_.GetAllocator().Deallocate(isWheel_);
		manager_.GetAllocator().Deallocate(pressedThisFrame_);
	}

	InputDevice::DeviceVariant GetVariant() const
	{
		return InputDevice::DV_STANDARD;
	}

	void Update(InputState& state, InputState& previousState, InputDeltaState* delta)
	{
		state_ = &state;
		previousState_ = &previousState;
		delta_ = delta;

		// Reset mouse wheel buttons
		for (unsigned i = 0; i < MouseButtonCount; ++i)
		{
			const DeviceButtonId buttonId = i;
			const bool oldValue = previousState.GetBool(buttonId);
			if (isWheel_[i] && oldValue)
			{
				const bool pressed = false;
				nextState_.Set(buttonId, pressed);
#ifdef GAINPUT_DEBUG
				GAINPUT_LOG("Button: %i, %d\n", buttonId, pressed);
#endif
				if (delta_)
				{
					if (oldValue != pressed)
					{
						delta_->AddChange(device_, buttonId, oldValue, pressed);
					}
				}
			}
		}

		*state_ = nextState_;

		memset(pressedThisFrame_, 0, sizeof(bool) * MouseButtonCount);
	}

	void HandleEvent(XEvent& event)
	{
		switch (event.type)
		{
		case MotionNotify:
			{
				const XMotionEvent& motionEvent = event.xmotion;
				const float x = float(motionEvent.x)/float(manager_.GetDisplayWidth());
				const float y = float(motionEvent.y)/float(manager_.GetDisplayHeight());
				nextState_.Set(MouseAxisX, x);
				nextState_.Set(MouseAxisY, y);

				if (delta_)
				{
					const float oldX = previousState_->GetFloat(MouseAxisX);
					const float oldY = previousState_->GetFloat(MouseAxisY);
					if (oldX != x)
					{
						delta_->AddChange(device_, MouseAxisX, oldX, x);
					}
					if (oldY != y)
					{
						delta_->AddChange(device_, MouseAxisY, oldY, y);
					}
				}
				break;
			}
		case ButtonPress:
		case ButtonRelease:
			{
				const XButtonEvent& buttonEvent = event.xbutton;
				GAINPUT_ASSERT(buttonEvent.button > 0);
				const DeviceButtonId buttonId = buttonEvent.button-1;
				GAINPUT_ASSERT(buttonId <= MouseButtonMax);
				const bool pressed = event.type == ButtonPress;

				if (!pressed && pressedThisFrame_[buttonId])
				{
					// This is a mouse wheel button. Ignore release now, reset next frame.
					isWheel_[buttonId] = true;
				}
				else if (buttonEvent.button < MouseButtonCount)
				{
					nextState_.Set(buttonId, pressed);

#ifdef GAINPUT_DEBUG
					GAINPUT_LOG("Button: %i, %d\n", buttonId, pressed);
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

				if (pressed)
				{
					pressedThisFrame_[buttonId] = true;
				}
				break;
			}
		}
	}

private:
	InputManager& manager_;
	DeviceId device_;
	bool* isWheel_;
	bool* pressedThisFrame_;
	InputState* state_;
	InputState* previousState_;
	InputState nextState_;
	InputDeltaState* delta_;
};

}

#endif

