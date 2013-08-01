
#include <gainput/gainput.h>

#include "../samplefw/SampleFramework.h"


enum Button
{
	ButtonToggleListener,
	ButtonToggleMapListener,
	ButtonConfirm,
	ButtonMouseX,
};


class MyDeviceButtonListener : public gainput::InputListener
{
public:
	MyDeviceButtonListener(gainput::InputManager& manager) : manager(manager) { }

	void OnDeviceButtonBool(gainput::DeviceId deviceId, gainput::DeviceButtonId deviceButton, bool oldValue, bool newValue)
	{
		const gainput::InputDevice* device = manager.GetDevice(deviceId);
		char buttonName[64] = "";
		device->GetButtonName(deviceButton, buttonName, 64);
		SFW_LOG("Device %d (%s%d) bool button (%d/%s) changed: %d -> %d\n", deviceId, device->GetTypeName(), device->GetIndex(), deviceButton, buttonName, oldValue, newValue);
	}

	void OnDeviceButtonFloat(gainput::DeviceId deviceId, gainput::DeviceButtonId deviceButton, float oldValue, float newValue)
	{
		const gainput::InputDevice* device = manager.GetDevice(deviceId);
		char buttonName[64] = "";
		device->GetButtonName(deviceButton, buttonName, 64);
		SFW_LOG("Device %d (%s%d) float button (%d/%s) changed: %f -> %f\n", deviceId, device->GetTypeName(), device->GetIndex(), deviceButton, buttonName, oldValue, newValue);
	}

private:
	gainput::InputManager& manager;
};

class MyUserButtonListener : public gainput::MappedInputListener
{
public:
	void OnUserButtonBool(gainput::UserButtonId userButton, bool oldValue, bool newValue)
	{
		SFW_LOG("User bool button %d changed: %d -> %d\n", userButton, oldValue, newValue);
	}

	void OnUserButtonFloat(gainput::UserButtonId userButton, float oldValue, float newValue)
	{
		SFW_LOG("User float button %d changed: %f -> %f\n", userButton, oldValue, newValue);
	}
};


void SampleMain()
{
	SfwOpenWindow("Gainput: Listener sample");

	gainput::InputManager manager;

	const gainput::DeviceId keyboardId = manager.CreateDevice<gainput::InputDeviceKeyboard>();
	const gainput::DeviceId mouseId = manager.CreateDevice<gainput::InputDeviceMouse>();
	manager.CreateDevice<gainput::InputDevicePad>();
	manager.CreateDevice<gainput::InputDevicePad>();
	manager.CreateDevice<gainput::InputDeviceTouch>();

#if defined(GAINPUT_PLATFORM_LINUX) || defined(GAINPUT_PLATFORM_WIN)
	manager.SetDisplaySize(SfwGetWidth(), SfwGetHeight());
#endif

	SfwSetInputManager(&manager);

	gainput::InputMap map(manager, "testmap");

	map.MapBool(ButtonToggleListener, keyboardId, gainput::KeyF1);
	map.MapBool(ButtonToggleMapListener, keyboardId, gainput::KeyF2);
	map.MapBool(ButtonConfirm, mouseId, gainput::MOUSE_BUTTON_LEFT);
	map.MapFloat(ButtonMouseX, mouseId, gainput::MOUSE_AXIS_X);

	MyDeviceButtonListener myDeviceButtonListener(manager);
	MyUserButtonListener myUserButtonListener;
	gainput::ListenerId myDeviceButtonListenerId = manager.AddListener(&myDeviceButtonListener);
	gainput::ListenerId myUserButtonListenerId = map.AddListener(&myUserButtonListener);

	bool doExit = false;

	while (!SfwIsDone() && !doExit)
	{
		manager.Update();

#if defined(GAINPUT_PLATFORM_LINUX)
		XEvent event;
		while (XPending(SfwGetXDisplay()))
		{
			XNextEvent(SfwGetXDisplay(), &event);
			manager.HandleEvent(event);
			if (event.type == DestroyNotify || event.type == ClientMessage)
			{
				doExit = true;
			}
		}
#elif defined(GAINPUT_PLATFORM_WIN)
		MSG msg;
		while (PeekMessage(&msg, SfwGetHWnd(),  0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			manager.HandleMessage(msg);
		}
#endif

		SfwUpdate();

		if (map.GetBoolWasDown(ButtonToggleListener))
		{
			if (myDeviceButtonListenerId != gainput::ListenerId(-1))
			{
				manager.RemoveListener(myDeviceButtonListenerId);
				myDeviceButtonListenerId = gainput::ListenerId(-1);
				SFW_LOG("Device button listener disabled.\n");
			}
			else
			{
				myDeviceButtonListenerId = manager.AddListener(&myDeviceButtonListener);
				SFW_LOG("Device button listener enabled.\n");
			}
		}

		if (map.GetBoolWasDown(ButtonToggleMapListener))
		{
			if (myUserButtonListenerId != gainput::ListenerId(-1))
			{
				map.RemoveListener(myUserButtonListenerId);
				myUserButtonListenerId = gainput::ListenerId(-1);
				SFW_LOG("User button listener disabled.\n");
			}
			else
			{
				myUserButtonListenerId = map.AddListener(&myUserButtonListener);
				SFW_LOG("User button listener enabled.\n");
			}
		}
	}

	SfwCloseWindow();
}

