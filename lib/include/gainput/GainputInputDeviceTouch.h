
#ifndef GAINPUTINPUTDEVICETOUCH_H_
#define GAINPUTINPUTDEVICETOUCH_H_

namespace gainput
{

/// All valid touches for a touch device.
enum TouchButton
{
	TOUCH_0_DOWN,
	TOUCH_0_X,
	TOUCH_0_Y,
	TOUCH_0_PRESSURE,
	TOUCH_COUNT
};



class InputDeviceTouchImpl;

/// A touch input device.
class GAINPUT_LIBEXPORT InputDeviceTouch : public InputDevice
{
public:
	/// Initializes the device.
	/**
	 * \param manager The input manager this device is managed by.
	 * \param device The ID of this device.
	 */
	InputDeviceTouch(InputManager& manager, DeviceId device);
	/// Shuts down the device.
	~InputDeviceTouch();

	void Update(InputDeltaState* delta);

	/// Returns DT_TOUCH.
	DeviceType GetType() const { return DT_TOUCH; }
	DeviceState GetState() const { return DS_OK; }
	bool IsValidButtonId(DeviceButtonId deviceButton) const { return deviceButton >= TOUCH_0_DOWN && deviceButton < TOUCH_COUNT; }

	size_t GetAnyButtonDown(DeviceButtonSpec* outButtons, size_t maxButtonCount) const;

	size_t GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const;
	ButtonType GetButtonType(DeviceButtonId deviceButton) const;

	/// Returns the platform-specific implementation of this device.
	InputDeviceTouchImpl* GetPimpl() { return impl_; }

private:
	InputDeviceTouchImpl* impl_;

};

}

#endif

