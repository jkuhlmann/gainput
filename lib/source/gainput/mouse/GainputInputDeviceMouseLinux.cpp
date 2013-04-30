
#include <gainput/gainput.h>


#if defined(GAINPUT_PLATFORM_LINUX)

#include "../GainputInputDeltaState.h"
#include "GainputMouseInfo.h"

namespace gainput
{

class InputDeviceMouseImpl
{
public:
	InputDeviceMouseImpl(InputManager& manager, DeviceId device) :
		manager_(manager),
		device_(device)
	{
		const size_t size = sizeof(bool)*MouseButtonCount;
		isWheel_ = static_cast<bool*>(manager_.GetAllocator().Allocate(size));
		GAINPUT_ASSERT(isWheel_);
		memset(isWheel_, 0, size);
	}

	~InputDeviceMouseImpl()
	{
		manager_.GetAllocator().Deallocate(isWheel_);
	}

	void Update(InputState& state, InputState& previousState, InputDeltaState* delta)
	{
		// Reset mouse wheel buttons
		for (unsigned i = 0; i < MouseButtonCount; ++i)
		{
			const DeviceButtonId buttonId = i;
			const bool oldValue = previousState.GetBool(buttonId);
			if (isWheel_[i] && oldValue)
			{
				const bool pressed = false;
				state.Set(buttonId, pressed);
#ifdef GAINPUT_DEBUG
				GAINPUT_LOG("Button: %i, %d\n", buttonId, pressed);
#endif
				if (delta)
				{
					if (oldValue != pressed)
					{
						delta->AddChange(device_, buttonId, oldValue, pressed);
					}
				}
			}
		}

		bool pressedThisFrame[MouseButtonCount];
		memset(pressedThisFrame, 0, sizeof(bool) * MouseButtonCount);

		const long eventMask = ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
		XEvent event;
		while (XCheckMaskEvent(manager_.GetXDisplay(), eventMask, &event))
		{
			switch (event.type)
			{
			case MotionNotify:
				{
					const XMotionEvent& motionEvent = event.xmotion;
					const float x = float(motionEvent.x)/float(manager_.GetDisplayWidth());
					const float y = float(motionEvent.y)/float(manager_.GetDisplayHeight());
					state.Set(MOUSE_AXIS_X, x);
					state.Set(MOUSE_AXIS_Y, y);

					if (delta)
					{
						const float oldX = previousState.GetFloat(MOUSE_AXIS_X);
						const float oldY = previousState.GetFloat(MOUSE_AXIS_Y);
						if (oldX != x)
						{
							delta->AddChange(device_, MOUSE_AXIS_X, oldX, x);
						}
						if (oldY != y)
						{
							delta->AddChange(device_, MOUSE_AXIS_Y, oldY, y);
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
					GAINPUT_ASSERT(buttonId <= MOUSE_BUTTON_MAX);
					const bool pressed = event.type == ButtonPress;

					if (!pressed && pressedThisFrame[buttonId])
					{
						// This is a mouse wheel button. Ignore release now, reset next frame.
						isWheel_[buttonId] = true;
					}
					else if (buttonEvent.button < MouseButtonCount)
					{
						state.Set(buttonId, pressed);

#ifdef GAINPUT_DEBUG
						GAINPUT_LOG("Button: %i, %d\n", buttonId, pressed);
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

					if (pressed)
					{
						pressedThisFrame[buttonId] = true;
					}
					break;
				}
			}
		}
	}

	InputManager& GetManager() const { return manager_; }
	DeviceId GetDevice() const { return device_; }

private:
	InputManager& manager_;
	DeviceId device_;
	bool* isWheel_;
};

}

#include "GainputMouseCommon.h"

#endif

