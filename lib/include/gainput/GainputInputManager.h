
#ifndef GAINPUTINPUTMANAGER_H_
#define GAINPUTINPUTMANAGER_H_


namespace gainput
{

/// Manages all input devices and some other helpful stuff.
/**
 * This manager takes care of all device-related things. Normally, you should only need one that contains
 * all your input devices.
 *
 * After instantiating an InputManager, you probably have to call some platform-specific function 
 * (see SetXDisplay(), SetDisplaySize()). You should also create some input devices using the template
 * function CreateDevice() which returns the device ID that is needed to do anything further with the
 * device (for example, see InputMap).
 *
 * The manager has to be updated every frame by calling Update(). Depending on the platform,
 * you may have to call another function as part of your message handling code (see HandleMessage(), HandleInput()).
 *
 * Note that destruction of an InputManager invalidates all input maps based on it and all devices created
 * through it.
 */
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
	InputManager(Allocator& allocator = GetDefaultAllocator());

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
	Allocator& GetAllocator() const { return allocator_; }

	/// Returns a monotonic time in milliseconds.
	uint64_t GetTime() const;

	/// Creates an input device and registers it with the manager.
	/**
	 * \tparam T The input device class, muste be derived from InputDevice.
	 * \return The ID of the newly created input device.
	 */
	template<class T> DeviceId CreateDevice();
	/// Creates an input device, registers it with the manager and returns it.
	/**
	 * \tparam T The input device class, muste be derived from InputDevice.
	 * \return The newly created input device.
	 */
	template<class T> T* CreateAndGetDevice();
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
	/// Returns the ID of the device with the given type and index.
	/**
	 * \param typeName The name of the device type. Should come from InputDevice::GetTypeName().
	 * \param index The index of the device. Should come from InputDevice::GetIndex().
	 * \return The device's ID or InvalidDeviceId if no matching device exists.
	 */
	DeviceId FindDeviceId(const char* typeName, unsigned index) const;

	/// Registers a listener to be notified when a button state changes.
	/**
	 * If there are listeners registered, all input devices will have to record their state changes. This incurs extra runtime costs.
	 */
	ListenerId AddListener(InputListener* listener);
	/// De-registers the given listener.
	void RemoveListener(ListenerId listenerId);

	/// Checks if any button on any device is down.
	/**
	 * \param[out] outButtons An array with maxButtonCount fields to receive the device buttons that are down.
	 * \param maxButtonCount The number of fields in outButtons.
	 * \return The number of device buttons written to outButtons.
	 */
	size_t GetAnyButtonDown(DeviceButtonSpec* outButtons, size_t maxButtonCount) const;

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

	HashMap<ListenerId, InputListener*> listeners_;
	unsigned nextListenerId_;

	InputDeltaState* deltaState_;

#if defined(GAINPUT_PLATFORM_LINUX)
	/// [LINUX ONLY] The XDisplay used to acquire keyboard and mouse inputs.
	Display* xDisplay_;
#endif

	int displayWidth_;
	int displayHeight_;

	void DeviceCreated(InputDevice* device);

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
	DeviceCreated(device);
	return nextDeviceId_++;
}

template<class T>
inline
T*
InputManager::CreateAndGetDevice()
{
	T* device = allocator_.New<T>(*this, nextDeviceId_);
	devices_[nextDeviceId_] = device;
	++nextDeviceId_;
	DeviceCreated(device);
	return device;
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

