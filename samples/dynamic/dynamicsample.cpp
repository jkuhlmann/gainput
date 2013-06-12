
#include <gainput/gainput.h>

#include "../samplefw/SampleFramework.h"


enum Button
{
	ButtonReset,
	ButtonOther,
	ButtonTest
};


void SampleMain()
{
	SfwOpenWindow("Gainput: Dynamic sample");

	gainput::InputManager manager;

	const gainput::DeviceId mouseId = manager.CreateDevice<gainput::InputDeviceMouse>();
	const gainput::DeviceId touchId = manager.CreateDevice<gainput::InputDeviceTouch>();
	const gainput::DeviceId keyboardId = manager.CreateDevice<gainput::InputDeviceKeyboard>();
	const gainput::DeviceId padId = manager.CreateDevice<gainput::InputDevicePad>();

#if defined(GAINPUT_PLATFORM_LINUX)
	manager.SetXDisplay(SfwGetXDisplay(), SfwGetWidth(), SfwGetHeight());
#elif defined(GAINPUT_PLATFORM_WIN)
	manager.SetDisplaySize(SfwGetWidth(), SfwGetHeight());
#endif

	SfwSetInputManager(&manager);

	gainput::InputMap map(manager);
	map.MapBool(ButtonReset, keyboardId, gainput::KEY_ESCAPE);
	map.MapBool(ButtonOther, keyboardId, gainput::KEY_A);

	gainput::DeviceButtonSpec anyButton[32];
	bool mapped = false;

	SFW_LOG("No button mapped, please press any button.\n");
	SFW_LOG("Press ESC to reset.\n");


	while (!SfwIsDone())
	{
		manager.Update();

#if defined(GAINPUT_PLATFORM_WIN)
		MSG msg;
		while (PeekMessage(&msg, SfwGetHWnd(),  0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			manager.HandleMessage(msg);
		}
#endif

		SfwUpdate();

		if (map.GetBoolWasDown(ButtonReset))
		{
			SFW_LOG("Mapping reset. Press any button.\n");
			mapped = false;
			map.Unmap(ButtonTest);
		}

		if (!mapped)
		{
			const size_t anyCount = manager.GetAnyButtonDown(anyButton, 32);
			for (size_t i = 0; i < anyCount; ++i)
			{
				// Filter the returned buttons as needed.
				const gainput::InputDevice* device = manager.GetDevice(anyButton[i].deviceId);
				if (device->GetButtonType(anyButton[i].buttonId) == gainput::BT_BOOL
					&& map.GetUserButtonId(anyButton[i].deviceId, anyButton[i].buttonId) == gainput::InvalidDeviceButtonId)
				{
					SFW_LOG("Mapping to: %d:%d\n", anyButton[i].deviceId, anyButton[i].buttonId);
					map.MapBool(ButtonTest, anyButton[i].deviceId, anyButton[i].buttonId);
					mapped = true;
					break;
				}
			}
		}
		else
		{
			if (map.GetBoolWasDown(ButtonTest))
			{
				SFW_LOG("Button was down!\n");
			}
		}
	}

	SfwCloseWindow();
}


