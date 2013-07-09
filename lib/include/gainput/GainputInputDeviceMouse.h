
#ifndef GAINPUTINPUTDEVICEMOUSE_H_
#define GAINPUTINPUTDEVICEMOUSE_H_

namespace gainput
{

/// All valid device buttons for InputDeviceMouse.
enum MouseButton
{
	MOUSE_BUTTON_0 = 0,
	MOUSE_BUTTON_LEFT = MOUSE_BUTTON_0,
	MOUSE_BUTTON_1,
	MOUSE_BUTTON_MIDDLE = MOUSE_BUTTON_1,
	MOUSE_BUTTON_2,
	MOUSE_BUTTON_RIGHT = MOUSE_BUTTON_2,
	MOUSE_BUTTON_3,
	MOUSE_BUTTON_WHEEL_UP = MOUSE_BUTTON_3,
	MOUSE_BUTTON_4,
	MOUSE_BUTTON_WHEEL_DOWN = MOUSE_BUTTON_4,
	MOUSE_BUTTON_5,
	MOUSE_BUTTON_6,
	MOUSE_BUTTON_7,
	MOUSE_BUTTON_8,
	MOUSE_BUTTON_9,
	MOUSE_BUTTON_10,
	MOUSE_BUTTON_11,
	MOUSE_BUTTON_12,
	MOUSE_BUTTON_13,
	MOUSE_BUTTON_14,
	MOUSE_BUTTON_15,
	MOUSE_BUTTON_16,
	MOUSE_BUTTON_17,
	MOUSE_BUTTON_18,
	MOUSE_BUTTON_19,
	MOUSE_BUTTON_20,
	MOUSE_BUTTON_MAX = MOUSE_BUTTON_20,
	MOUSE_AXIS_X,
	MOUSE_AXIS_Y,
	MOUSE_BUTTON_COUNT
};



class InputDeviceMouseImpl;

/// A mouse input device.
/**
 * This input device provides support for standard mouse devices. The valid device buttons are defined
 * in the ::MouseButton enum.
 *
 * This device is implemented on Linux and Windows.
 */
class GAINPUT_LIBEXPORT InputDeviceMouse : public InputDevice
{
public:
	/// Initializes the device.
	/**
	 * Instantiate the device using InputManager::CreateDevice().
	 *
	 * \param manager The input manager this device is managed by.
	 * \param device The ID of this device.
	 */
	InputDeviceMouse(InputManager& manager, DeviceId device);
	/// Shuts down the device.
	~InputDeviceMouse();

	/// Returns DT_MOUSE.
	DeviceType GetType() const { return DT_MOUSE; }
	const char* GetTypeName() const { return "mouse"; }
	bool IsValidButtonId(DeviceButtonId deviceButton) const { return deviceButton >= MOUSE_BUTTON_0 && deviceButton < MOUSE_BUTTON_COUNT; }

	size_t GetAnyButtonDown(DeviceButtonSpec* outButtons, size_t maxButtonCount) const;

	size_t GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const;
	ButtonType GetButtonType(DeviceButtonId deviceButton) const;
	DeviceButtonId GetButtonByName(const char* name) const;

	/// Returns the platform-specific implementation of this device (internal use only).
	InputDeviceMouseImpl* GetPimpl() { return impl_; }

protected:
	void InternalUpdate(InputDeltaState* delta);

	DeviceState InternalGetState() const { return DS_OK; }

private:
	InputDeviceMouseImpl* impl_;

};

}

#endif

