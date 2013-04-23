
#ifndef GAINPUTDOUBLECLICKGESTURE_H_
#define GAINPUTDOUBLECLICKGESTURE_H_

#ifdef GAINPUT_ENABLE_DOUBLE_CLICK_GESTURE

namespace gainput
{

enum DoubleClickAction
{
	DoubleClickTriggered
};

/// A double-click gesture.
class GAINPUT_LIBEXPORT DoubleClickGesture : public InputDevice
{
public:
	/// Initializes the gesture.
	DoubleClickGesture(InputManager& manager, DeviceId device);
	/// Uninitializes the gesture.
	~DoubleClickGesture();

	/// Sets up the gesture for operation without position checking.
	/**
	 * \param actionButtonDevice ID of the input device containing the action button.
	 * \param actionButton ID of the device button to be used as the action button.
	 * \param timeSpan Allowed time between clicks in milliseconds.
	 */
	void Initialize(DeviceId actionButtonDevice, DeviceButtonId actionButton, uint64_t timeSpan = 300);
	/// Sets up the gesture for operation with position checking, i.e. the user may not move the mouse too far between clicks.
	/**
	 * \param actionButtonDevice ID of the input device containing the action button.
	 * \param actionButton ID of the device button to be used as the action button.
	 * \param xAxisDevice ID of the input device containing the X coordinate of the pointer.
	 * \param xAxis ID of the device button/axis to be used for the X coordinate of the pointer.
	 * \param xTolerance The amount the pointer may travel in the X coordinate to still be valid.
	 * \param yAxisDevice ID of the input device containing the Y coordinate of the pointer.
	 * \param yAxis ID of the device button/axis to be used for the Y coordinate of the pointer.
	 * \param yTolerance The amount the pointer may travel in the Y coordinate to still be valid.
	 * \param timeSpan Allowed time between clicks in milliseconds.
	 */
	void Initialize(DeviceId actionButtonDevice, DeviceButtonId actionButton, 
			DeviceId xAxisDevice, DeviceButtonId xAxis, float xTolerance, 
			DeviceId yAxisDevice, DeviceButtonId yAxis, float yTolerance, 
			uint64_t timeSpan = 300);

	/// Sets the number of clicks to trigger an action.
	/**
	 * \param count The number of clicks that will trigger this gesture; the default is 2, i.e. double-click.
	 */
	void SetClicksTargetCount(unsigned count) { clicksTargetCount_ = count; }

	void Update(InputDeltaState* delta);

	/// Returns DT_GESTURE.
	DeviceType GetType() const { return DT_GESTURE; }
	DeviceState GetState() const { return DS_OK; }
	bool IsValidButtonId(DeviceButtonId deviceButton) const { return deviceButton == DoubleClickTriggered; }

	ButtonType GetButtonType(DeviceButtonId deviceButton) const { GAINPUT_ASSERT(IsValidButtonId(deviceButton)); return BT_BOOL; }

private:
	InputManager& manager_;
	DeviceButtonSpec actionButton_;
	DeviceButtonSpec xAxis_;
	float xTolerance_;
	DeviceButtonSpec yAxis_;
	float yTolerance_;

	uint64_t timeSpan_;
	uint64_t firstClickTime_;

	float firstClickX_;
	float firstClickY_;

	unsigned clicksRegistered_;
	unsigned clicksTargetCount_;

};

}

#endif

#endif

