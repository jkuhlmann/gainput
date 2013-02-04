
#ifndef GAINPUTINPUTMANAGER_H_
#define GAINPUTINPUTMANAGER_H_


namespace gainput
{

/// Manages all input devices and misc stuff.
class GAINPUT_LIBEXPORT InputManager
{
public:
	/// Initializes the manager.
	/**
	 * Further initialization is typically necessary.
	 * \param allocator The memory allocator to be used for all allocations.
	 * \see SetXDisplay
	 * \see SetDisplaySize
	 */
	InputManager(Allocator& allocator = defaultAllocator);

#if defined(GAINPUT_PLATFORM_LINUX)
	/// [LINUX ONLY] Returns the XDisplay used to acquire keyboard and mouse inputs.
	Display* GetXDisplay() { return xDisplay_; }
	/// [LINUX ONLY] Sets the XDisplay used to acquire keyboard and mouse inputs.
	void SetXDisplay(Display* xDisplay, int width, int height) { xDisplay_ = xDisplay; displayWidth_ = width; displayHeight_ = height; }
#endif
#if defined(GAINPUT_PLATFORM_WIN) || defined(GAINPUT_PLATFORM_ANDROID)
	/// [WINDOWS/ANDROID ONLY] Sets the window resolution.
	void SetDisplaySize(int width, int height) { displayWidth_ = width; displayHeight_ = height; }
#endif
#if defined(GAINPUT_PLATFORM_WIN)
	/// [WINDOWS ONLY] Lets the InputManager handle the given Windows message.
	void HandleMessage(const MSG& msg);
#endif
#if defined(GAINPUT_PLATFORM_ANDROID)
	/// [ANDROID ONLY] Lets the InputManager handle the given input event.
	int32_t HandleInput(AInputEvent* event);
#endif

	/// Destructs the manager.
	~InputManager();

	/// Updates the input state, call this every frame.
	void Update();

	/// Returns the allocator to be used by for memory allocations.
	Allocator& GetAllocator() { return allocator_; }

	/// Creates an input device and registers it with the manager.
	/**
	 * \tparam T The input device class, muste be derived from InputDevice.
	 * \return The ID of the newly created input device.
	 */
	template<class T> DeviceId CreateDevice();
	/// Returns the input device with the given ID.
	/**
	 * \return The input device or 0 if it doesn't exist.
	 */
	InputDevice* GetDevice(DeviceId deviceId);
	/// Returns the input device with the given ID.
	/**
	 * \return The input device or 0 if it doesn't exist.
	 */
	const InputDevice* GetDevice(DeviceId deviceId) const;

	/// Registers a listener to be notified when a button state changes.
	/**
	 * If there are listeners registered, all input devices will have to record their state changes. This causes extra runtime costs.
	 */
	void AddListener(InputListener* listener);
	/// De-registers the given listener.
	void RemoveListener(InputListener* listener);

	/// Checks if any button on any device is down.
	/**
	 * \param[out] outDeviceId If a button is down, this is set to the device's ID.
	 * \param[out] outButtonId If a button is down, this is set to the device button's ID.
	 * \return true if any button is down, false otherwise.
	 */
	bool GetAnyButtonDown(DeviceId& outDeviceId, DeviceButtonId& outButtonId) const;

	/// Returns the number of devices with the given type.
	unsigned GetDeviceCountByType(InputDevice::DeviceType type) const;
	/// Returns the graphical display's width in pixels.
	int GetDisplayWidth() const { return displayWidth_; }
	/// Returns the graphical display's height in pixels.
	int GetDisplayHeight() const { return displayHeight_; }

private:
	Allocator& allocator_;

	typedef HashMap<DeviceId, InputDevice*> DeviceMap;
	DeviceMap devices_;
	unsigned nextDeviceId_;

	Array<InputListener*> listeners_;
	InputDeltaState* deltaState_;

#if defined(GAINPUT_PLATFORM_LINUX)
	/// [LINUX ONLY] The XDisplay used to acquire keyboard and mouse inputs.
	Display* xDisplay_;
#endif

	int displayWidth_;
	int displayHeight_;

	// Do not copy.
	InputManager(const InputManager &);
	InputManager& operator=(const InputManager &);

};


template<class T>
inline
DeviceId
InputManager::CreateDevice()
{
	T* device = allocator_.New<T>(*this, nextDeviceId_);
	devices_[nextDeviceId_] = device;
	return nextDeviceId_++;
}

inline
InputDevice*
InputManager::GetDevice(DeviceId deviceId)
{
	DeviceMap::iterator it = devices_.find(deviceId);
	if (it == devices_.end())
	{
		return 0;
	}
	return it->second;
}

inline
const InputDevice*
InputManager::GetDevice(DeviceId deviceId) const
{
	DeviceMap::const_iterator it = devices_.find(deviceId);
	if (it == devices_.end())
	{
		return 0;
	}
	return it->second;
}

}

#endif

