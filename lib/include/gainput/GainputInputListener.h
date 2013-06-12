
#ifndef GAINPUTINPUTLISTENER_H_
#define GAINPUTINPUTLISTENER_H_

namespace gainput
{

/// Listener interface that allows to receive notifications when device button states change.
class GAINPUT_LIBEXPORT InputListener
{
public:
	/// Called when a bool-type button state changes.
	/**
	 * \param device The input device's ID the state change occurred on.
	 * \param deviceButton The ID of the device button that changed.
	 * \param oldValue Previous state of the button.
	 * \param newValue New state of the button.
	 */
	virtual void OnDeviceButtonBool(DeviceId device, DeviceButtonId deviceButton, bool oldValue, bool newValue) { }
	/// Called when a float-type button state changes.
	/**
	 * \param device The input device's ID the state change occurred on.
	 * \param deviceButton The ID of the device button that changed.
	 * \param oldValue Previous state of the button.
	 * \param newValue New state of the button.
	 */
	virtual void OnDeviceButtonFloat(DeviceId device, DeviceButtonId deviceButton, float oldValue, float newValue) { }
};


/// Listener interface that allows to receive notifications when user button states change.
class GAINPUT_LIBEXPORT MappedInputListener
{
public:
	/// Called when a bool-type button state changes.
	/**
	 * \param userButton The user button's ID.
	 * \param oldValue Previous state of the button.
	 * \param newValue New state of the button.
	 */
	virtual void OnUserButtonBool(UserButtonId userButton, bool oldValue, bool newValue) { }
	/// Called when a float-type button state changes.
	/**
	 * \param userButton The user button's ID.
	 * \param oldValue Previous state of the button.
	 * \param newValue New state of the button.
	 */
	virtual void OnUserButtonFloat(UserButtonId userButton, float oldValue, float newValue) { }

};

}

#endif

