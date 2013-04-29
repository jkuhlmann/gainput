
#ifndef GAINPUTGESTURES_H_
#define GAINPUTGESTURES_H_

#ifdef GAINPUT_ENABLE_ALL_GESTURES
#define GAINPUT_ENABLE_DOUBLE_CLICK_GESTURE
#define GAINPUT_ENABLE_HOLD_GESTURE
#define GAINPUT_ENABLE_PINCH_GESTURE
#define GAINPUT_ENABLE_ROTATE_GESTURE
#define GAINPUT_ENABLE_TAP_GESTURE
#endif


namespace gainput
{

/// Common functionality for all input gestures.
class InputGesture : public InputDevice
{
public:
	/// Returns DT_GESTURE.
	DeviceType GetType() const { return DT_GESTURE; }
	DeviceState GetState() const { return DS_OK; }

protected:
	InputGesture(DeviceId device) : InputDevice(device) { }

};

}


#include <gainput/gestures/GainputDoubleClickGesture.h>
#include <gainput/gestures/GainputHoldGesture.h>
#include <gainput/gestures/GainputPinchGesture.h>
#include <gainput/gestures/GainputRotateGesture.h>
#include <gainput/gestures/GainputTapGesture.h>

#endif

