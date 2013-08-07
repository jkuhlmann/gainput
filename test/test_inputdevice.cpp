
#include "Catch/single_include/catch.hpp"

#include <gainput/gainput.h>

using namespace gainput;

TEST_CASE("InputDevice/keyboard", "")
{
	InputManager manager;

	InputDeviceKeyboard* device = manager.CreateAndGetDevice<InputDeviceKeyboard>();

	REQUIRE(device);
	REQUIRE(device->GetIndex() == 0);
	REQUIRE(device->GetDeviceId() != InvalidDeviceId);
	REQUIRE(device->GetType() == InputDevice::DT_KEYBOARD);
	REQUIRE(device->GetTypeName());
	REQUIRE(device->GetState() == InputDevice::DS_OK);
	REQUIRE(device->IsAvailable());

	REQUIRE(device->IsValidButtonId(KeyEscape));
	REQUIRE(device->IsValidButtonId(KeyReturn));
	REQUIRE(device->IsValidButtonId(Key1));
	REQUIRE(device->IsValidButtonId(KeyA));
	REQUIRE(device->IsValidButtonId(KeyZ));
	REQUIRE(device->IsValidButtonId(KeySpace));

	char buf[32];
	REQUIRE(device->GetButtonName(KeyF10, buf, 32) > 0);
	REQUIRE(device->GetButtonName(KeyRightParenthesis, buf, 32) > 0);
	REQUIRE(device->GetButtonName(KeyPeriod, buf, 32) > 0);
	REQUIRE(device->GetButtonName(KeyV, buf, 32) > 0);
	REQUIRE(device->GetButtonName(KeySpace, buf, 32) > 0);

	REQUIRE(device->GetButtonType(KeyEscape) == BT_BOOL);
	REQUIRE(device->GetButtonType(Key2) == BT_BOOL);
	REQUIRE(device->GetButtonType(KeyD) == BT_BOOL);
	REQUIRE(device->GetButtonType(KeyBackSpace) == BT_BOOL);

	REQUIRE(device->GetButtonByName("space") == KeySpace);
	REQUIRE(device->GetButtonByName("5") == Key5);

	REQUIRE(device->GetInputState());
	REQUIRE(device->GetPreviousInputState());

	REQUIRE(device->IsTextInputEnabled());
	device->SetTextInputEnabled(false);
	REQUIRE(!device->IsTextInputEnabled());
}

TEST_CASE("InputDevice/mouse", "")
{
	InputManager manager;

	InputDeviceMouse* device = manager.CreateAndGetDevice<InputDeviceMouse>();

	REQUIRE(device);
	REQUIRE(device->GetIndex() == 0);
	REQUIRE(device->GetDeviceId() != InvalidDeviceId);
	REQUIRE(device->GetType() == InputDevice::DT_MOUSE);
	REQUIRE(device->GetTypeName());
	REQUIRE(device->GetState() == InputDevice::DS_OK);
	REQUIRE(device->IsAvailable());

	REQUIRE(device->IsValidButtonId(MouseButtonLeft));
	REQUIRE(device->IsValidButtonId(MouseButtonMiddle));
	REQUIRE(device->IsValidButtonId(MouseButtonRight));
	REQUIRE(device->IsValidButtonId(MouseAxisX));
	REQUIRE(device->IsValidButtonId(MouseAxisY));

	char buf[32];
	REQUIRE(device->GetButtonName(MouseButtonLeft, buf, 32) > 0);
	REQUIRE(device->GetButtonName(MouseButtonRight, buf, 32) > 0);
	REQUIRE(device->GetButtonName(MouseButton14, buf, 32) > 0);
	REQUIRE(device->GetButtonName(MouseAxisX, buf, 32) > 0);
	REQUIRE(device->GetButtonName(MouseAxisY, buf, 32) > 0);

	REQUIRE(device->GetButtonType(MouseButtonLeft) == BT_BOOL);
	REQUIRE(device->GetButtonType(MouseButtonMiddle) == BT_BOOL);
	REQUIRE(device->GetButtonType(MouseButtonRight) == BT_BOOL);
	REQUIRE(device->GetButtonType(MouseAxisX) == BT_FLOAT);
	REQUIRE(device->GetButtonType(MouseAxisY) == BT_FLOAT);

	REQUIRE(device->GetButtonByName("mouse_left") == MouseButtonLeft);
	REQUIRE(device->GetButtonByName("mouse_right") == MouseButtonRight);
	REQUIRE(device->GetButtonByName("mouse_11") == MouseButton11);
	REQUIRE(device->GetButtonByName("mouse_x") == MouseAxisX);
	REQUIRE(device->GetButtonByName("mouse_y") == MouseAxisY);

	REQUIRE(device->GetInputState());
	REQUIRE(device->GetPreviousInputState());
}

TEST_CASE("InputDevice/pad", "")
{
	InputManager manager;

	InputDevicePad* device = manager.CreateAndGetDevice<InputDevicePad>();

	REQUIRE(device);
	REQUIRE(device->GetIndex() == 0);
	REQUIRE(device->GetDeviceId() != InvalidDeviceId);
	REQUIRE(device->GetType() == InputDevice::DT_PAD);
	REQUIRE(device->GetTypeName());

	REQUIRE(device->IsValidButtonId(PAD_BUTTON_LEFT_STICK_X));
	REQUIRE(device->IsValidButtonId(PAD_BUTTON_LEFT_STICK_Y));
	REQUIRE(device->IsValidButtonId(PAD_BUTTON_LEFT));
	REQUIRE(device->IsValidButtonId(PAD_BUTTON_RIGHT));
	REQUIRE(device->IsValidButtonId(PAD_BUTTON_UP));
	REQUIRE(device->IsValidButtonId(PAD_BUTTON_DOWN));
	REQUIRE(device->IsValidButtonId(PAD_BUTTON_A));
	REQUIRE(device->IsValidButtonId(PAD_BUTTON_B));
	REQUIRE(device->IsValidButtonId(PAD_BUTTON_X));
	REQUIRE(device->IsValidButtonId(PAD_BUTTON_Y));
	REQUIRE(device->IsValidButtonId(PAD_BUTTON_L1));
	REQUIRE(device->IsValidButtonId(PAD_BUTTON_R1));

	char buf[32];
	REQUIRE(device->GetButtonName(PAD_BUTTON_LEFT_STICK_X, buf, 32) > 0);
	REQUIRE(device->GetButtonName(PAD_BUTTON_B, buf, 32) > 0);
	REQUIRE(device->GetButtonName(PAD_BUTTON_AXIS_19, buf, 32) > 0);
	REQUIRE(device->GetButtonName(PAD_BUTTON_L2, buf, 32) > 0);
	REQUIRE(device->GetButtonName(PAD_BUTTON_GYROSCOPE_Y, buf, 32) > 0);

	REQUIRE(device->GetButtonType(PAD_BUTTON_LEFT) == BT_BOOL);
	REQUIRE(device->GetButtonType(PAD_BUTTON_DOWN) == BT_BOOL);
	REQUIRE(device->GetButtonType(PAD_BUTTON_A) == BT_BOOL);
	REQUIRE(device->GetButtonType(PAD_BUTTON_B) == BT_BOOL);
	REQUIRE(device->GetButtonType(PAD_BUTTON_L1) == BT_BOOL);
	REQUIRE(device->GetButtonType(PAD_BUTTON_LEFT_STICK_X) == BT_FLOAT);
	REQUIRE(device->GetButtonType(PAD_BUTTON_LEFT_STICK_Y) == BT_FLOAT);
	REQUIRE(device->GetButtonType(PAD_BUTTON_AXIS_15) == BT_FLOAT);

	REQUIRE(device->GetButtonByName("pad_left_stick_x") == PAD_BUTTON_LEFT_STICK_X);
	REQUIRE(device->GetButtonByName("pad_left_stick_y") == PAD_BUTTON_LEFT_STICK_Y);
	REQUIRE(device->GetButtonByName("pad_acceleration_x") == PAD_BUTTON_ACCELERATION_X);
	REQUIRE(device->GetButtonByName("pad_button_a") == PAD_BUTTON_A);
	REQUIRE(device->GetButtonByName("pad_button_y") == PAD_BUTTON_Y);

	REQUIRE(device->GetInputState());
	REQUIRE(device->GetPreviousInputState());
}

TEST_CASE("InputDevice/touch", "")
{
	InputManager manager;

	InputDeviceTouch* device = manager.CreateAndGetDevice<InputDeviceTouch>();

	REQUIRE(device);
	REQUIRE(device->GetIndex() == 0);
	REQUIRE(device->GetDeviceId() != InvalidDeviceId);
	REQUIRE(device->GetType() == InputDevice::DT_TOUCH);
	REQUIRE(device->GetTypeName());

	REQUIRE(device->IsValidButtonId(Touch0Down));
	REQUIRE(device->IsValidButtonId(Touch0X));
	REQUIRE(device->IsValidButtonId(Touch0Y));
	REQUIRE(device->IsValidButtonId(Touch1Down));
	REQUIRE(device->IsValidButtonId(Touch1X));
	REQUIRE(device->IsValidButtonId(Touch1Y));

	char buf[32];
	REQUIRE(device->GetButtonName(Touch0Down, buf, 32) > 0);
	REQUIRE(device->GetButtonName(Touch0X, buf, 32) > 0);
	REQUIRE(device->GetButtonName(Touch0Y, buf, 32) > 0);
	REQUIRE(device->GetButtonName(Touch2Down, buf, 32) > 0);

	REQUIRE(device->GetButtonType(Touch0Down) == BT_BOOL);
	REQUIRE(device->GetButtonType(Touch0X) == BT_FLOAT);
	REQUIRE(device->GetButtonType(Touch0Y) == BT_FLOAT);
	REQUIRE(device->GetButtonType(Touch3Down) == BT_BOOL);
	REQUIRE(device->GetButtonType(Touch3X) == BT_FLOAT);
	REQUIRE(device->GetButtonType(Touch3Y) == BT_FLOAT);

	REQUIRE(device->GetButtonByName("touch_0_down") == Touch0Down);
	REQUIRE(device->GetButtonByName("touch_0_x") == Touch0X);
	REQUIRE(device->GetButtonByName("touch_0_y") == Touch0Y);
	REQUIRE(device->GetButtonByName("touch_6_down") == Touch6Down);
	REQUIRE(device->GetButtonByName("touch_6_x") == Touch6X);
	REQUIRE(device->GetButtonByName("touch_6_y") == Touch6Y);

	REQUIRE(device->GetInputState());
	REQUIRE(device->GetPreviousInputState());
}

