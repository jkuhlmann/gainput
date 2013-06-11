

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


/// Interface for anything that provides device inputs.
/**
 * An InputDevice can be anything from a physical device, like a mouse or keyboard, to the more abstract
 * concept of gestures that in turn themselves depend on other InputDevices. What they have in common is
 * that they provide a number of device buttons (identified by a DeviceButtonId) which can be queried for
 * their state.
 *
 * Note that you may not instantiate an InputDevice (or any derived implementation) directly. Instead you
 * have to call InputManager::CreateDevice() or InputManager::CreateAndGetDevice() with the device you want
 * to instantiate as the template parameter. That way the device will be properly registered with the
 * InputManager and continuously updated.
 *
 * Normally, you won't interact with an InputDevice directly, but instead use its device ID and its device
 * buttons' IDs to map the device buttons to user buttons (see InputMap).
 */
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
		DT_GESTURE,		///< A gesture input device, building on top of other input devices.
		DT_COUNT		///< The count of input device types.
	};

	/// State of an input device.
	enum DeviceState
	{
		DS_OK,			///< Everything is okay.
		DS_LOW_BATTERY,		///< The input device is low on battery.
		DS_UNAVAILABLE		///< The input device is currently not available.
	};

	/// Initializes the input device.
	/**
	 * Do not instantiate any input device directly. Call InputManager::CreateDevice() instead.
	 */
	InputDevice(InputManager& manager, DeviceId device);

	/// Empty virtual destructor.
	virtual ~InputDevice() { }

	/// Update this device, internally called by InputManager.
	/**
	 * \param delta The delta state to add changes to.
	 */
	virtual void Update(InputDeltaState* delta) = 0;

	/// Returns this device's ID.
	DeviceId GetDeviceId() const { return deviceId_; }

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

	/// Checks if any button on this device is down.
	/**
	 * \param[out] outButtons An array with maxButtonCount fields to receive the device buttons that are down.
	 * \param maxButtonCount The number of fields in outButtons.
	 * \return The number of device buttons written to outButtons.
	 */
	virtual size_t GetAnyButtonDown(DeviceButtonSpec* outButtons, size_t maxButtonCount) const { return 0; }

	/// Gets the name of the given button.
	/**
	 * \param deviceButton ID of the button.
	 * \param buffer A char-buffer to receive the button name.
	 * \param bufferLength Length of the buffer receiving the button name in bytes.
	 * \return The number of bytes written to buffer (includes the trailing \0).
	 */
	virtual size_t GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const { return 0; }
	/// Returns the type of the given button.
	virtual ButtonType GetButtonType(DeviceButtonId deviceButton) const = 0;

	/// Returns the button's ID if the name is of this device's buttons.
	/**
	 * \param name Name of the device button to look for.
	 * \return The device button ID.
	 */
	virtual DeviceButtonId GetButtonByName(const char* name) const { return InvalidDeviceButtonId; }

protected:
	/// The manager this device belongs to.
	InputManager& manager_;

	/// The ID of this device.
	DeviceId deviceId_;

	/// The current state of this device.
	InputState* state_;
	/// The previous state of this device.
	InputState* previousState_;

	size_t CheckAllButtonsDown(DeviceButtonSpec* outButtons, size_t maxButtonCount, unsigned start, unsigned end, DeviceId device) const;
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

