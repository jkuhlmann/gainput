
#ifndef GAINPUTHOLDGESTURE_H_
#define GAINPUTHOLDGESTURE_H_

#ifdef GAINPUT_ENABLE_HOLD_GESTURE

namespace gainput
{

/// Buttons provided by the HoldGesture.
enum HoldAction
{
	HoldTriggered		///< The button that triggers after holding for the given time.
};

/// A hold-to-trigger gesture.
class GAINPUT_LIBEXPORT HoldGesture : public InputDevice
{
public:
	/// Initializes the gesture.
	HoldGesture(InputManager& manager, DeviceId device);
	/// Uninitializes the gesture.
	~HoldGesture();

	/// Sets up the gesture for operation without position checking.
	/**
	 * \param actionButtonDevice ID of the input device containing the action button.
	 * \param actionButton ID of the device button to be used as the action button.
	 * \param oneShot Specifies if the gesture triggers only once after the given time or if it triggers indefinitely.
	 * \param timeSpan Time in milliseconds the user needs to hold in order to trigger the gesture.
	 */
	void Initialize(DeviceId actionButtonDevice, DeviceButtonId actionButton, bool oneShot = true, uint64_t timeSpan = 800);
	/// Sets up the gesture for operation with position checking, i.e. the user may not move the mouse too far while holding.
	/**
	 * \param actionButtonDevice ID of the input device containing the action button.
	 * \param actionButton ID of the device button to be used as the action button.
	 * \param xAxisDevice ID of the input device containing the X coordinate of the pointer.
	 * \param xAxis ID of the device button/axis to be used for the X coordinate of the pointer.
	 * \param xTolerance The amount the pointer may travel in the X coordinate to still be valid.
	 * \param yAxisDevice ID of the input device containing the Y coordinate of the pointer.
	 * \param yAxis ID of the device button/axis to be used for the Y coordinate of the pointer.
	 * \param yTolerance The amount the pointer may travel in the Y coordinate to still be valid.
	 * \param oneShot Specifies if the gesture triggers only once after the given time or if it triggers indefinitely.
	 * \param timeSpan Time in milliseconds the user needs to hold in order to trigger the gesture.
	 */
	void Initialize(DeviceId actionButtonDevice, DeviceButtonId actionButton, 
			DeviceId xAxisDevice, DeviceButtonId xAxis, float xTolerance, 
			DeviceId yAxisDevice, DeviceButtonId yAxis, float yTolerance, 
			bool oneShot = true, 
			uint64_t timeSpan = 800);

	void Update(InputDeltaState* delta);

	/// Returns DT_GESTURE.
	DeviceType GetType() const { return DT_GESTURE; }
	DeviceState GetState() const { return DS_OK; }
	bool IsValidButtonId(DeviceButtonId deviceButton) const { return deviceButton == HoldTriggered; }

	ButtonType GetButtonType(DeviceButtonId deviceButton) const { GAINPUT_ASSERT(IsValidButtonId(deviceButton)); return BT_BOOL; }

private:
	InputManager& manager_;
	DeviceButtonSpec actionButton_;
	DeviceButtonSpec xAxis_;
	float xTolerance_;
	DeviceButtonSpec yAxis_;
	float yTolerance_;

	bool oneShot_;
	bool oneShotReset_;
	uint64_t timeSpan_;
	uint64_t firstDownTime_;

	float firstDownX_;
	float firstDownY_;

};

}

#endif

#endif

