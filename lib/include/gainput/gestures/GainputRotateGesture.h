
#ifndef GAINPUTROTATEGESTURE_H_
#define GAINPUTROTATEGESTURE_H_

#ifdef GAINPUT_ENABLE_ROTATE_GESTURE

namespace gainput
{

/// Buttons provided by the RotateGesture.
enum RotateAction
{
	RotateTriggered,		///< The button that triggers when both pinch buttons are down.
	RotateAngle			///< The current pinch scale value if pinching is active.
};

/// A pinch gesture.
class GAINPUT_LIBEXPORT RotateGesture : public InputDevice
{
public:
	/// Initializes the gesture.
	RotateGesture(InputManager& manager, DeviceId device);
	/// Uninitializes the gesture.
	~RotateGesture();

	/// Sets up the gesture for operation with the given axes and button.
	/**
	 * \param downDevice ID of the input device containing the first touch button.
	 * \param downButton ID of the device button to be used as the first touch button.
	 * \param xAxisDevice ID of the input device containing the X coordinate of the first touch point.
	 * \param xAxis ID of the device button/axis to be used for the X coordinate of the first touch point.
	 * \param yAxisDevice ID of the input device containing the Y coordinate of the first touch point.
	 * \param yAxis ID of the device button/axis to be used for the Y coordinate of the first touch point.
	 * \param down2Device ID of the input device containing the second touch button.
	 * \param downButton2 ID of the device button to be used as the second touch button.
	 * \param xAxis2Device ID of the input device containing the X coordinate of the second touch point.
	 * \param xAxis2 ID of the device button/axis to be used for the X coordinate of the second touch point.
	 * \param yAxis2Device ID of the input device containing the Y coordinate of the second touch point.
	 * \param yAxis2 ID of the device button/axis to be used for the Y coordinate of the second touch point.
	 */
	void Initialize(DeviceId downDevice, DeviceButtonId downButton, 
			DeviceId xAxisDevice, DeviceButtonId xAxis, 
			DeviceId yAxisDevice, DeviceButtonId yAxis, 
			DeviceId down2Device, DeviceButtonId downButton2, 
			DeviceId xAxis2Device, DeviceButtonId xAxis2, 
			DeviceId yAxis2Device, DeviceButtonId yAxis2);

	void Update(InputDeltaState* delta);

	/// Returns DT_GESTURE.
	DeviceType GetType() const { return DT_GESTURE; }
	DeviceState GetState() const { return DS_OK; }
	bool IsValidButtonId(DeviceButtonId deviceButton) const { return deviceButton == RotateTriggered || deviceButton == RotateAngle; }

	ButtonType GetButtonType(DeviceButtonId deviceButton) const { GAINPUT_ASSERT(IsValidButtonId(deviceButton)); return deviceButton == RotateTriggered ? BT_BOOL : BT_FLOAT; }

private:
	InputManager& manager_;
	DeviceButtonSpec downButton_;
	DeviceButtonSpec xAxis_;
	DeviceButtonSpec yAxis_;
	DeviceButtonSpec downButton2_;
	DeviceButtonSpec xAxis2_;
	DeviceButtonSpec yAxis2_;

	bool rotating_;
	float initialAngle_;

};

}

#endif

#endif

