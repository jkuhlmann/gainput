
#ifndef GAINPUTINPUTMANAGER_H_
#define GAINPUTINPUTMANAGER_H_


namespace gainput
{

/// Manages all input devices and some other helpful stuff.
/**
 * This manager takes care of all device-related things. Normally, you should only need one that contains
 * all your input devices.
 *
 * After instantiating an InputManager, you have to call SetDisplaySize(). You should also create some 
 * input devices using the template function CreateDevice() which returns the device ID that is needed 
 * to do anything further with the device (for example, see InputMap).
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
	 * \see SetDisplaySize
	 */
	InputManager(Allocator& allocator = GetDefaultAllocator());

	/// Sets the window resolution.
	/**
	 * Informs the InputManager and its devices of the size of the window that is used for
	 * receiving inputs. For example, the size is used to to normalize mouse inputs.
	 */
	void SetDisplaySize(int width, int height) { displayWidth_ = width; displayHeight_ = height; }

#if defined(GAINPUT_PLATFORM_LINUX)
	/// [LINUX ONLY] Lets the InputManager handle the given X event.
	void HandleEvent(XEvent& event);
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

	/// Returns the allocator to be used for memory allocations.
	Allocator& GetAllocator() const { return allocator_; }

	/// Returns a monotonic time in milliseconds.
	uint64_t GetTime() const;

	/// Creates an input device and registers it with the manager.
	/**
	 * \tparam T The input device class, muste be derived from InputDevice.
	 * \param variant Requests the specified device variant. Note that this is only
	 * a request. Another implementation variant of the device may silently be instantiated
	 * instead. To determine what variant was instantiated, call InputDevice::GetVariant().
	 * \return The ID of the newly created input device.
	 */
	template<class T> DeviceId CreateDevice(InputDevice::DeviceVariant variant = InputDevice::DV_STANDARD);
	/// Creates an input device, registers it with the manager and returns it.
	/**
	 * \tparam T The input device class, muste be derived from InputDevice.
	 * \param variant Requests the specified device variant. Note that this is only
	 * a request. Another implementation variant of the device may silently be instantiated
	 * instead. To determine what variant was instantiated, call InputDevice::GetVariant().
	 * \return The newly created input device.
	 */
	template<class T> T* CreateAndGetDevice(InputDevice::DeviceVariant variant = InputDevice::DV_STANDARD);
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
	/// Returns the ID of the device with the given type and index.
	/**
	 * \param type The device type. Should come from InputDevice::GetType().
	 * \param index The index of the device. Should come from InputDevice::GetIndex().
	 * \return The device's ID or InvalidDeviceId if no matching device exists.
	 */
	DeviceId FindDeviceId(InputDevice::DeviceType type, unsigned index) const;

	typedef HashMap<DeviceId, InputDevice*> DeviceMap;
	/// Iterator over all registered devices.
	typedef DeviceMap::iterator iterator;
	/// Const iterator over all registered devices.
	typedef DeviceMap::const_iterator const_iterator;

	/// Returns the begin iterator over all registered devices.
	iterator begin() { return devices_.begin(); }
	/// Returns the end iterator over all registered devices.
	iterator end() { return devices_.end(); }
	/// Returns the begin iterator over all registered devices.
	const_iterator begin() const { return devices_.begin(); }
	/// Returns the end iterator over all registered devices.
	const_iterator end() const { return devices_.end(); }

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

	/// Registers a modifier that will be called after devices have been updated.
	ModifierId AddDeviceStateModifier(DeviceStateModifier* modifier);
	/// De-registers the given modifier.
	void RemoveDeviceStateModifier(ModifierId modifierId);

	/// [IN dev BUILDS ONLY] Connect to a remote host to send device state changes to.
	void ConnectForStateSync(const char* ip, unsigned port);
	/// [IN dev BUILDS ONLY] Initiate sending of device state changes to the given device.
	void StartDeviceStateSync(DeviceId deviceId);

private:
	Allocator& allocator_;

	DeviceMap devices_;
	unsigned nextDeviceId_;

	HashMap<ListenerId, InputListener*> listeners_;
	unsigned nextListenerId_;

	HashMap<ModifierId, DeviceStateModifier*> modifiers_;
	unsigned nextModifierId_;

	InputDeltaState* deltaState_;

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
InputManager::CreateDevice(InputDevice::DeviceVariant variant)
{
	T* device = allocator_.New<T>(*this, nextDeviceId_, variant);
	devices_[nextDeviceId_] = device;
	DeviceCreated(device);
	return nextDeviceId_++;
}

template<class T>
inline
T*
InputManager::CreateAndGetDevice(InputDevice::DeviceVariant variant)
{
	T* device = allocator_.New<T>(*this, nextDeviceId_, variant);
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


/// Interface for modifiers that change device input states after they have been updated.
class DeviceStateModifier
{
public:
	/// Called after non-dependent devices have been updated.
	/**
	 * This function is called by InputManager::Update() after InputDevice::Update() has been
	 * called on all registered devices that have InputDevice::IsLateUpdate() return \c false.
	 *
	 * \param delta All device state changes should be registered with this delta state, may be 0.
	 */
	virtual void Update(InputDeltaState* delta) = 0;
};

}

#endif

