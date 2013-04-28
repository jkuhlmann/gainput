
#ifndef GAINPUTINPUTMAP_H_
#define GAINPUTINPUTMAP_H_

namespace gainput
{

class UserButton;

/// Maps user buttons to device buttons.
/**
 * This is the interface that should be used to get input. You can have several maps that are used
 * simultaneously or use different ones depending on game state. The user button IDs have to be unique per input map.
 *
 * InputMap uses the provided InputManager to get devices inputs and process them into user-mapped inputs. After creating 
 * an InputMap, you should map some device buttons to user buttons (using MapBool() or MapFloat()). User buttons are identified 
 * by an ID provided by you. In order to ensure their uniqueness, it's a good idea to define an enum containing all your user buttons
 * for any given InputMap. It's of course possible to map multiple different device button to one user button.
 *
 * After a user button has been mapped, you can query its state by calling one of the several GetBool* and GetFloat* functions. The
 * result will depend on the mapped device button(s) and the policy (set using SetUserButtonPolicy()).
 */
class GAINPUT_LIBEXPORT InputMap
{
public:
	/// Initializes the map.
	/**
	 * \param manager The input manager used to get device inputs.
	 * \param allocator The allocator to be used for all memory allocations.
	 */
	InputMap(const InputManager& manager, Allocator& allocator = GetDefaultAllocator());
	/// Unitializes the map.
	~InputMap();

	/// Clears all mapped inputs.
	void Clear();
	
	/// Returns the input manager this input map uses.
	const InputManager& GetManager() const { return manager_; }

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
	/// Removes all mappings for the given user button.
	void Unmap(UserButtonId userButton);
	/// Returns if the given user button has any mappings.
	bool IsMapped(UserButtonId userButton) const;
	
	/// Gets all device buttons mapped to the given user button.
	/**
	 * \param userButton The user button ID of the button to return all mappings for.
	 * \param[out] outButtons An array with maxButtonCount fields to receive the device buttons that are mapped.
	 * \param maxButtonCount The number of fields in outButtons.
	 * \return The number of device buttons written to outButtons.
	 */
	size_t GetMappings(UserButtonId userButton, DeviceButtonSpec* outButtons, size_t maxButtonCount) const;
	
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

	/// Gets the name of the device button mapped to the given user button.
	/**
	 * \param userButton ID of the user button.
	 * \param buffer A char-buffer to receive the button name.
	 * \param bufferLength Length of the buffer receiving the button name in bytes.
	 * \return The number of bytes written to buffer (includes the trailing \0).
	 */
	size_t GetUserButtonName(UserButtonId userButton, char* buffer, size_t bufferLength) const;

private:
	Allocator& allocator_;
	const InputManager& manager_;

	typedef HashMap<UserButtonId, UserButton*> UserButtonMap;
	UserButtonMap userButtons_;
	UserButtonId nextUserButtonId_;

	Array<InputListener*> listeners_;

	float GetFloatState(UserButtonId userButton, bool previous) const;

	UserButton* GetUserButton(UserButtonId userButton);
	const UserButton* GetUserButton(UserButtonId userButton) const;

	// Do not copy.
	InputMap(const InputMap &);
	InputMap& operator=(const InputMap &);

};

}

#endif

