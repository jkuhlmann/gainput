
#ifndef GAINPUTINPUTDEVICETOUCH_H_
#define GAINPUTINPUTDEVICETOUCH_H_

namespace gainput
{

/// All valid device inputs for InputDeviceTouch.
enum TouchButton
{
	TOUCH_0_DOWN,
	TOUCH_0_X,
	TOUCH_0_Y,
	TOUCH_0_PRESSURE,
	TOUCH_1_DOWN,
	TOUCH_1_X,
	TOUCH_1_Y,
	TOUCH_1_PRESSURE,
	TOUCH_2_DOWN,
	TOUCH_2_X,
	TOUCH_2_Y,
	TOUCH_2_PRESSURE,
	TOUCH_3_DOWN,
	TOUCH_3_X,
	TOUCH_3_Y,
	TOUCH_3_PRESSURE,
	TOUCH_4_DOWN,
	TOUCH_4_X,
	TOUCH_4_Y,
	TOUCH_4_PRESSURE,
	TOUCH_5_DOWN,
	TOUCH_5_X,
	TOUCH_5_Y,
	TOUCH_5_PRESSURE,
	TOUCH_6_DOWN,
	TOUCH_6_X,
	TOUCH_6_Y,
	TOUCH_6_PRESSURE,
	TOUCH_7_DOWN,
	TOUCH_7_X,
	TOUCH_7_Y,
	TOUCH_7_PRESSURE,
	TOUCH_COUNT
};



class InputDeviceTouchImpl;

/// A touch input device.
/**
 * This input device provides support for touch screen/surface devices. The valid device buttons are defined
 * in the ::TouchButton enum. For each touch point, there is a boolean input (TOUCH_*_DOWN) showing if the
 * touch point is active, two float inputs (TOUCH_*_X, TOUCH_*_Y) showing the touch point's position, and a
 * float input (TOUCH_*_PRESSURE) showing the touch's pressure.
 *
 * Not all touch points must be numbered consecutively, i.e. point #2 may be down even though #0 and #1 are not.
 *
 * The number of simultaneously possible touch points is implementaion-dependent.
 *
 * This device is implemented on Android NDK.
 */
class GAINPUT_LIBEXPORT InputDeviceTouch : public InputDevice
{
public:
	/// Initializes the device.
	/**
	 * Instantiate the device using InputManager::CreateDevice().
	 *
	 * \param manager The input manager this device is managed by.
	 * \param device The ID of this device.
	 */
	InputDeviceTouch(InputManager& manager, DeviceId device);
	/// Shuts down the device.
	~InputDeviceTouch();

	/// Returns DT_TOUCH.
	DeviceType GetType() const { return DT_TOUCH; }
	const char* GetTypeName() const { return "touch"; }
	bool IsValidButtonId(DeviceButtonId deviceButton) const { return deviceButton >= TOUCH_0_DOWN && deviceButton < TOUCH_COUNT; }

	size_t GetAnyButtonDown(DeviceButtonSpec* outButtons, size_t maxButtonCount) const;

	size_t GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const;
	ButtonType GetButtonType(DeviceButtonId deviceButton) const;
	DeviceButtonId GetButtonByName(const char* name) const;

	/// Returns the platform-specific implementation of this device.
	InputDeviceTouchImpl* GetPimpl() { return impl_; }

protected:
	void InternalUpdate(InputDeltaState* delta);

	DeviceState InternalGetState() const;

private:
	InputDeviceTouchImpl* impl_;

};

}

#endif

