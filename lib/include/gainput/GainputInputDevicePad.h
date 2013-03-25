
#ifndef GAINPUTINPUTDEVICEPAD_H_
#define GAINPUTINPUTDEVICEPAD_H_

namespace gainput
{

/// The maximum number of pads supported.
enum { MaxPadCount = 10 };

/// All valid device buttons for the pad device.
enum PadButton
{
	PAD_BUTTON_LEFT_STICK_X,
	PAD_BUTTON_LEFT_STICK_Y,
	PAD_BUTTON_RIGHT_STICK_X,
	PAD_BUTTON_RIGHT_STICK_Y,
	PAD_BUTTON_AXIS_4, // L2/Left trigger
	PAD_BUTTON_AXIS_5, // R2/Right trigger
	PAD_BUTTON_AXIS_6,
	PAD_BUTTON_AXIS_7,
	PAD_BUTTON_AXIS_8,
	PAD_BUTTON_AXIS_9,
	PAD_BUTTON_AXIS_10,
	PAD_BUTTON_AXIS_11,
	PAD_BUTTON_AXIS_12,
	PAD_BUTTON_AXIS_13,
	PAD_BUTTON_AXIS_14,
	PAD_BUTTON_AXIS_15,
	PAD_BUTTON_AXIS_16,
	PAD_BUTTON_AXIS_17,
	PAD_BUTTON_AXIS_18,
	PAD_BUTTON_AXIS_19,
	PAD_BUTTON_AXIS_20,
	PAD_BUTTON_AXIS_21,
	PAD_BUTTON_AXIS_22,
	PAD_BUTTON_AXIS_23,
	PAD_BUTTON_AXIS_24,
	PAD_BUTTON_AXIS_25,
	PAD_BUTTON_AXIS_26,
	PAD_BUTTON_AXIS_27,
	PAD_BUTTON_AXIS_28,
	PAD_BUTTON_AXIS_29,
	PAD_BUTTON_AXIS_30,
	PAD_BUTTON_AXIS_31,
	PAD_BUTTON_ACCELERATION_X,
	PAD_BUTTON_ACCELERATION_Y,
	PAD_BUTTON_ACCELERATION_Z,
	PAD_BUTTON_GYROSCOPE_X,
	PAD_BUTTON_GYROSCOPE_Y,
	PAD_BUTTON_GYROSCOPE_Z,
	PAD_BUTTON_MAGNETICFIELD_X,
	PAD_BUTTON_MAGNETICFIELD_Y,
	PAD_BUTTON_MAGNETICFIELD_Z,
	PAD_BUTTON_START,
	PAD_BUTTON_AXIS_COUNT = PAD_BUTTON_START,
	PAD_BUTTON_SELECT,
	PAD_BUTTON_LEFT,
	PAD_BUTTON_RIGHT,
	PAD_BUTTON_UP,
	PAD_BUTTON_DOWN,
	PAD_BUTTON_A, // Cross
	PAD_BUTTON_B, // Circle
	PAD_BUTTON_X, // Square
	PAD_BUTTON_Y, // Triangle
	PAD_BUTTON_L1,
	PAD_BUTTON_R1,
	PAD_BUTTON_L2,
	PAD_BUTTON_R2,
	PAD_BUTTON_L3, // Left thumb
	PAD_BUTTON_R3, // Right thumb
	PAD_BUTTON_HOME, // PS button
	PAD_BUTTON_17,
	PAD_BUTTON_18,
	PAD_BUTTON_19,
	PAD_BUTTON_20,
	PAD_BUTTON_21,
	PAD_BUTTON_22,
	PAD_BUTTON_23,
	PAD_BUTTON_24,
	PAD_BUTTON_25,
	PAD_BUTTON_26,
	PAD_BUTTON_27,
	PAD_BUTTON_28,
	PAD_BUTTON_29,
	PAD_BUTTON_30,
	PAD_BUTTON_31,
	PAD_BUTTON_MAX,
	PAD_BUTTON_COUNT = PAD_BUTTON_MAX - PAD_BUTTON_AXIS_COUNT
};

class InputDevicePadImpl;

/// A pad input device.
class GAINPUT_LIBEXPORT InputDevicePad : public InputDevice
{
public:
	/// The operating system device IDs for all possible pads.
	static const char* PadDeviceIds[MaxPadCount];
	// TODO SetPadDeviceId(padIndex, const char* id);

	/// Initializes the device.
	/**
	 * \param manager The input manager this device is managed by.
	 * \param device The ID of this device.
	 */
	InputDevicePad(InputManager& manager, DeviceId device);
	/// Shuts down the device.
	~InputDevicePad();

	void Update(InputDeltaState* delta);

	/// Returns DT_PAD.
	DeviceType GetType() const { return DT_PAD; }
	DeviceState GetState() const;
	bool IsValidButtonId(DeviceButtonId deviceButton) const { return deviceButton >= PAD_BUTTON_LEFT_STICK_X && deviceButton < PAD_BUTTON_MAX; }

	size_t GetAnyButtonDown(DeviceButtonSpec* outButtons, size_t maxButtonCount) const;

	size_t GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const;
	ButtonType GetButtonType(DeviceButtonId deviceButton) const;
	DeviceButtonId GetButtonByName(const char* name) const;

	/// Enables the rumble feature of the pad.
	/**
	 * \param leftMotor Speed of the left motor, between 0.0 and 1.0.
	 * \param rightMotor Speed of the right motor, between 0.0 and 1.0.
	 * \return true if rumble was enabled successfully, false otherwise.
	 */
	bool Vibrate(float leftMotor, float rightMotor);

private:
	InputDevicePadImpl* impl_;

};

}

#endif

