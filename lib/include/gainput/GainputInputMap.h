
#ifndef GAINPUTINPUTMAP_H_
#define GAINPUTINPUTMAP_H_

namespace gainput
{

class UserButton;

/// Maps user buttons to device buttons.
/**
 * This is the interface that should be used to get input. You can have several maps that you use depending on game state.
 * The user button IDs have to be unique per input map.
 */
class GAINPUT_LIBEXPORT InputMap
{
public:
	/// Initializes the map.
	/**
	 * \param manager The input manager used to get device inputs.
	 * \param allocator The allocator to be used for all memory allocations.
	 */
	InputMap(const InputManager& manager, Allocator& allocator = defaultAllocator);
	/// Unitializes the map.
	~InputMap();

	/// Clears all mapped inputs.
	void Clear();
	
	/// Returns the input manager this input map uses.
	const InputManager& GetManager() const { return manager_; }

	GestureId AddGesture(InputGesture* gesture);
	InputGesture* GetGesture(GestureId gesture);
	void RemoveGesture(GestureId gesture);

	void AddListener(InputListener* listener);
	void RemoveListener(InputListener* listener);

	/// Maps a bool-type button.
	/**
	 * \param userButton The user ID for this mapping.
	 * \param device The device's ID of the device button to be mapped.
	 * \param deviceButton The ID of the device button to be mapped.
	 * \return true if the mapping was created.
	 */
	bool MapBool(UserButtonId userButton, DeviceId device, DeviceButtonId deviceButton);
	/// Maps a float-type button, possibly to a custom range.
	/**
	 * \param userButton The user ID for this mapping.
	 * \param device The device's ID of the device button to be mapped.
	 * \param deviceButton The ID of the device button to be mapped.
	 * \param min The minimum value of the mapped button.
	 * \param max The maximum value of the mapped button.
	 * \return true if the mapping was created.
	 */
	bool MapFloat(UserButtonId userButton, DeviceId device, DeviceButtonId deviceButton,
			float min = 0.0f, float max = 1.0f);
	bool MapGesture(UserButtonId userButton, GestureId gesture, DeviceButtonId gestureButton);
	/// Removes all mappings for the given user button.
	void Unmap(UserButtonId userButton);
	/// Returns if the given user button has any mappings.
	bool IsMapped(UserButtonId userButton) const;
	
	/// Policy for how multiple device buttons are summarized in one user button.
	enum UserButtonPolicy
	{
		UBP_FIRST_DOWN,		///< The first device buttons that is down (or not 0.0f) determines the result.
		UBP_MAX,		///< The maximum of all device button states is the result.
		UBP_MIN,		///< The minimum of all device button states is the result.
		UBP_AVERAGE		///< The average of all device button states is the result.
	};
	/// Sets how a user button handles inputs from multiple device buttons.
	/**
	 * \return true if the policy was set, false otherwise (i.e. the user button doesn't exist).
	 */
	bool SetUserButtonPolicy(UserButtonId userButton, UserButtonPolicy policy);

	/// Returns the bool state of a user button.
	bool GetBool(UserButtonId userButton) const;
	/// Returns if the user button is newly down.
	bool GetBoolIsNew(UserButtonId userButton) const;
	/// Returns the bool state of a user button from the previous frame.
	bool GetBoolPrevious(UserButtonId userButton) const;
	/// Returns if the user button has been released.
	bool GetBoolWasDown(UserButtonId userButton) const;

	/// Returns the float state of a user button.
	float GetFloat(UserButtonId userButton) const;
	/// Returns the float state of a user button from the previous frame.
	float GetFloatPrevious(UserButtonId userButton) const;
	/// Returns the delta between the previous and the current frame of the float state of the given user button.
	float GetFloatDelta(UserButtonId userButton) const;

private:
	Allocator& allocator_;
	const InputManager& manager_;

	typedef HashMap<UserButtonId, UserButton*> UserButtonMap;
	UserButtonMap userButtons_;
	UserButtonId nextUserButtonId_;

	typedef HashMap<GestureId, InputGesture*> GestureMap;
	GestureMap gestures_;
	Array<InputListener*> listeners_;

	UserButton* GetUserButton(UserButtonId userButton);
	const UserButton* GetUserButton(UserButtonId userButton) const;

	// Do not copy.
	InputMap(const InputMap &);
	InputMap& operator=(const InputMap &);

};

}

#endif

