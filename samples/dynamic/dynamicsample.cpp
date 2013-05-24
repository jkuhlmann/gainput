
#include <gainput/gainput.h>

#include "../samplefw/SampleFramework.h"


enum Button
{
	ButtonReset,
	ButtonTest
};


void SampleMain()
{
	SfwOpenWindow("Gainput: Dynamic sample");

	gainput::InputManager manager;

#if defined(GAINPUT_PLATFORM_LINUX) || defined(GAINPUT_PLATFORM_WIN)
	const gainput::DeviceId mouseId = manager.CreateDevice<gainput::InputDeviceMouse>();
#else
	const gainput::DeviceId touchId = manager.CreateDevice<gainput::InputDeviceTouch>();
#endif
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

	gainput::DeviceButtonSpec anyButton[32];
	bool mapped = false;

	GAINPUT_LOG("No button mapped, please press any button.\n");
	GAINPUT_LOG("Press ESC to reset.\n");


	while (!SfwIsDone())
	{
		manager.Update();

#if defined(GAINPUT_PLATFORM_WIN)
		MSG msg;
		while (PeekMessage(&msg, hWnd,  0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			manager.HandleMessage(msg);
		}
#endif

		SfwUpdate();

		if (map.GetBoolWasDown(ButtonReset))
		{
			GAINPUT_LOG("Mapping reset. Press any button.\n");
			mapped = false;
			map.Unmap(ButtonTest);
		}

		if (!mapped)
		{
			const size_t anyCount = manager.GetAnyButtonDown(anyButton, 32);
			if (anyCount > 0)
			{
				GAINPUT_LOG("Mapping to: %d:%d\n", anyButton[0].deviceId, anyButton[0].buttonId);
				map.MapBool(ButtonTest, anyButton[0].deviceId, anyButton[0].buttonId);
				mapped = true;
			}
		}
		else
		{
			if (map.GetBoolWasDown(ButtonTest))
			{
				GAINPUT_LOG("Button was down!\n");
			}
		}
	}

	SfwCloseWindow();
}


