
#include <gainput/gainput.h>

#include "../samplefw/SampleFramework.h"


enum Button
{
	ButtonConfirm,
	ButtonConfirmDouble,
	ButtonHoldGesture,
	ButtonTapGesture,
	ButtonPinching,
	ButtonPinchScale,
	ButtonRotating,
	ButtonRotateAngle,
};


void SampleMain()
{
	SfwOpenWindow("Gainput: Gesture sample");

	gainput::InputManager manager;

	const gainput::DeviceId mouseId = manager.CreateDevice<gainput::InputDeviceMouse>();
	const gainput::DeviceId touchId = manager.CreateDevice<gainput::InputDeviceTouch>();

#if defined(GAINPUT_PLATFORM_LINUX)
	manager.SetXDisplay(SfwGetXDisplay(), SfwGetWidth(), SfwGetHeight());
#elif defined(GAINPUT_PLATFORM_WIN)
	manager.SetDisplaySize(SfwGetWidth(), SfwGetHeight());
#endif

	SfwSetInputManager(&manager);

	gainput::InputMap map(manager, "testmap");

	map.MapBool(ButtonConfirm, mouseId, gainput::MOUSE_BUTTON_LEFT);

	gainput::DoubleClickGesture* dcg = manager.CreateAndGetDevice<gainput::DoubleClickGesture>();
	GAINPUT_ASSERT(dcg);
	dcg->Initialize(mouseId, gainput::MOUSE_BUTTON_LEFT,
			mouseId, gainput::MOUSE_AXIS_X, 0.01f,
			mouseId, gainput::MOUSE_AXIS_Y, 0.01f,
			500);
	map.MapBool(ButtonConfirmDouble, dcg->GetDeviceId(), gainput::DoubleClickTriggered);

	gainput::HoldGesture* hg = manager.CreateAndGetDevice<gainput::HoldGesture>();
	GAINPUT_ASSERT(hg);
	hg->Initialize(touchId, gainput::TOUCH_0_DOWN,
			touchId, gainput::TOUCH_0_X, 0.1f,
			touchId, gainput::TOUCH_0_Y, 0.1f,
			true,
			800);
	map.MapBool(ButtonHoldGesture, hg->GetDeviceId(), gainput::HoldTriggered);

	gainput::TapGesture* tg = manager.CreateAndGetDevice<gainput::TapGesture>();
	GAINPUT_ASSERT(tg);
	tg->Initialize(touchId, gainput::TOUCH_0_DOWN,
			500);
	map.MapBool(ButtonTapGesture, tg->GetDeviceId(), gainput::TapTriggered);
	
	gainput::PinchGesture* pg = manager.CreateAndGetDevice<gainput::PinchGesture>();
	GAINPUT_ASSERT(pg);
	pg->Initialize(touchId, gainput::TOUCH_0_DOWN,
			touchId, gainput::TOUCH_0_X,
			touchId, gainput::TOUCH_0_Y,
			touchId, gainput::TOUCH_1_DOWN,
			touchId, gainput::TOUCH_1_X,
			touchId, gainput::TOUCH_1_Y);
	map.MapBool(ButtonPinching, pg->GetDeviceId(), gainput::PinchTriggered);
	map.MapFloat(ButtonPinchScale, pg->GetDeviceId(), gainput::PinchScale);

	gainput::RotateGesture* rg = manager.CreateAndGetDevice<gainput::RotateGesture>();
	GAINPUT_ASSERT(rg);
	rg->Initialize(touchId, gainput::TOUCH_0_DOWN,
			touchId, gainput::TOUCH_0_X,
			touchId, gainput::TOUCH_0_Y,
			touchId, gainput::TOUCH_1_DOWN,
			touchId, gainput::TOUCH_1_X,
			touchId, gainput::TOUCH_1_Y);
	map.MapBool(ButtonRotating, rg->GetDeviceId(), gainput::RotateTriggered);
	map.MapFloat(ButtonRotateAngle, rg->GetDeviceId(), gainput::RotateAngle);

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

		if (map.GetBoolWasDown(ButtonConfirm))
		{
			SFW_LOG("Confirmed!\n");
		}

		if (map.GetBoolWasDown(ButtonConfirmDouble))
		{
			SFW_LOG("Confirmed doubly!\n");
		}

		if (map.GetBool(ButtonHoldGesture))
		{
			SFW_LOG("Hold triggered!\n");
		}

		if (map.GetBoolWasDown(ButtonTapGesture))
		{
			SFW_LOG("Tapped!\n");
		}

		if (map.GetBool(ButtonPinching))
		{
			SFW_LOG("Pinching: %f\n", map.GetFloat(ButtonPinchScale));
		}

		if (map.GetBool(ButtonRotating))
		{
			SFW_LOG("Rotation angle: %f\n", map.GetFloat(ButtonRotateAngle));
		}
	}

	SfwCloseWindow();
}


