

#ifndef GAINPUTINPUTDEVICE_H_
#define GAINPUTINPUTDEVICE_H_

namespace gainput
{


/// Type of an input device button.
enum ButtonType
{
	BT_BOOL,	///< A boolean value button, either down (true) or up (false).
	BT_FLOAT,	///< A floating-point value button, between -1.0f and 1.0f or 0.0f and 1.0f.
	BT_COUNT	///< The number of different button types.
};


/// Interface for all input devices.
class GAINPUT_LIBEXPORT InputDevice
{
public:
	/// Type of an input device.
	enum DeviceType
	{
		DT_MOUSE,		///< A mouse/cursor input device featuring one pointer.
		DT_KEYBOARD,		///< A keyboard input device.
		DT_PAD,			///< A joypad/gamepad input device.
		DT_TOUCH,		///< A touch-sensitive input device supporting multiple simultaneous pointers.
		DT_REMOTE,		///< A generic networked input device.
		DT_COUNT		///< The count of input device types.
	};

	/// State of an input device.
	enum DeviceState
	{
		DS_OK,			///< Everything is okay.
		DS_LOW_BATTERY,		///< The input device is low on battery.
		DS_UNAVAILABLE		///< The input device is currently not available.
	};


	/// Empty virtual destructor.
	virtual ~InputDevice() { }

	/// Update this device.
	/**
	 * \param delta The delta state to add changes to.
	 */
	virtual void Update(InputDeltaState* delta) = 0;

	/// Returns the device type.
	virtual DeviceType GetType() const = 0;
	/// Returns the device state.
	virtual DeviceState GetState() const = 0;
	/// Returns if this device is available.
	virtual bool IsAvailable() const { return GetState() == DS_OK || GetState() == DS_LOW_BATTERY; }
	/// Returns if the given button is valid for this device.
	virtual bool IsValidButtonId(DeviceButtonId deviceButton) const = 0;

	/// Returns the current state of the given button.
	virtual bool GetBool(DeviceButtonId deviceButton) const;
	/// Returns the previous state of the given button.
	virtual bool GetBoolPrevious(DeviceButtonId deviceButton) const;
	/// Returns the current state of the given button.
	virtual float GetFloat(DeviceButtonId deviceButton) const;
	/// Returns the previous state of the given button.
	virtual float GetFloatPrevious(DeviceButtonId deviceButton) const;

	/// Returns true if any button is down.
	/**
	 * \param[out] outButtonId The ID of the first button found that is down.
	 * \return true if any button is down.
	 */
	virtual bool GetAnyButtonDown(DeviceButtonId& outButtonId) const = 0;

	/// Gets the name of the given button.
	/**
	 * \param deviceButton ID of the button.
	 * \param buffer A char-buffer to receive the button name.
	 * \param bufferLength Length of the buffer receiving the button name in bytes.
	 * \return The number of bytes written to buffer (includes the trailing \0).
	 */
	virtual size_t GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const = 0;
	/// Returns the type of the given button.
	virtual ButtonType GetButtonType(DeviceButtonId deviceButton) const = 0;

protected:
	/// The current state of this device.
	InputState* state_;
	/// The previous state of this device.
	InputState* previousState_;
};


inline
bool
InputDevice::GetBool(DeviceButtonId deviceButton) const
{
	if (!IsAvailable())
	{
		return false;
	}
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	GAINPUT_ASSERT(state_);
	return state_->GetBool(deviceButton);
}

inline
bool
InputDevice::GetBoolPrevious(DeviceButtonId deviceButton) const
{
	if (!IsAvailable())
	{
		return false;
	}
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	GAINPUT_ASSERT(previousState_);
	return previousState_->GetBool(deviceButton);
}

inline
float
InputDevice::GetFloat(DeviceButtonId deviceButton) const
{
	if (!IsAvailable())
	{
		return 0.0f;
	}
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	GAINPUT_ASSERT(state_);
	return state_->GetFloat(deviceButton);
}

inline
float
InputDevice::GetFloatPrevious(DeviceButtonId deviceButton) const
{
	if (!IsAvailable())
	{
		return 0.0f;
	}
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	GAINPUT_ASSERT(previousState_);
	return previousState_->GetFloat(deviceButton);
}

}

#endif

