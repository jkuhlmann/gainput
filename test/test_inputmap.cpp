
#include "Catch/single_include/catch.hpp"

#include <gainput/gainput.h>

using namespace gainput;

enum TestButtons
{
	ButtonA,
	ButtonStart = ButtonA,
	ButtonB,
	ButtonC,
	ButtonD,
	ButtonE,
	ButtonF,
	ButtonG,
	ButtonCount
};

TEST_CASE("InputMap/create", "")
{
	InputManager manager;
	InputMap map(manager, "testmap");

	REQUIRE(&manager == &map.GetManager());
	REQUIRE(map.GetName());
	REQUIRE(strcmp(map.GetName(), "testmap") == 0);

	for (int b = ButtonStart; b < ButtonCount; ++b)
	{
		REQUIRE(!map.IsMapped(b));
	}

	InputMap map2(manager);
	REQUIRE(!map2.GetName());
}

TEST_CASE("InputMap/map_bool", "")
{
	InputManager manager;
	const DeviceId keyboardId = manager.CreateDevice<InputDeviceKeyboard>();
	const DeviceId mouseId = manager.CreateDevice<InputDeviceMouse>();
	InputMap map(manager, "testmap");

	REQUIRE(map.MapBool(ButtonA, keyboardId, KEY_A));
	REQUIRE(map.MapBool(ButtonA, keyboardId, KEY_B));
	REQUIRE(map.MapBool(ButtonA, keyboardId, KEY_ESCAPE));
	REQUIRE(map.MapBool(ButtonA, mouseId, MOUSE_BUTTON_LEFT));
	REQUIRE(map.IsMapped(ButtonA));

	REQUIRE(map.MapBool(ButtonB, keyboardId, KEY_F2));
	REQUIRE(map.MapBool(ButtonB, mouseId, MOUSE_BUTTON_LEFT));
	REQUIRE(map.IsMapped(ButtonB));

	map.Clear();
	for (int b = ButtonStart; b < ButtonCount; ++b)
	{
		REQUIRE(!map.IsMapped(b));
	}

	REQUIRE(map.MapBool(ButtonA, mouseId, MOUSE_BUTTON_RIGHT));
	REQUIRE(map.IsMapped(ButtonA));

	map.Unmap(ButtonA);
	REQUIRE(!map.IsMapped(ButtonA));

	DeviceButtonSpec mappings[32];
	REQUIRE(map.GetMappings(ButtonA, mappings, 32) == 0);

	REQUIRE(map.MapBool(ButtonA, mouseId, MOUSE_BUTTON_MIDDLE));
	REQUIRE(map.MapBool(ButtonA, keyboardId, KEY_D));
	REQUIRE(map.MapBool(ButtonD, keyboardId, KEY_B));

	REQUIRE(map.GetMappings(ButtonA, mappings, 32) == 2);
	REQUIRE(mappings[0].deviceId == mouseId);
	REQUIRE(mappings[0].buttonId == MOUSE_BUTTON_MIDDLE);
	REQUIRE(mappings[1].deviceId == keyboardId);
	REQUIRE(mappings[1].buttonId == KEY_D);

	char buf[32];
	REQUIRE(map.GetUserButtonName(ButtonA, buf, 32));

	REQUIRE(map.GetUserButtonId(mouseId, MOUSE_BUTTON_MIDDLE) == ButtonA);
	REQUIRE(map.GetUserButtonId(keyboardId, KEY_D) == ButtonA);
	REQUIRE(map.GetUserButtonId(keyboardId, KEY_B) == ButtonD);
}

TEST_CASE("InputMap/map_float", "")
{
	InputManager manager;
	const DeviceId keyboardId = manager.CreateDevice<InputDeviceKeyboard>();
	const DeviceId mouseId = manager.CreateDevice<InputDeviceMouse>();
	const DeviceId padId = manager.CreateDevice<InputDevicePad>();
	InputMap map(manager, "testmap");

	REQUIRE(map.MapFloat(ButtonA, keyboardId, KEY_A));
	REQUIRE(map.MapFloat(ButtonA, keyboardId, KEY_B));
	REQUIRE(map.MapFloat(ButtonA, keyboardId, KEY_ESCAPE));
	REQUIRE(map.MapFloat(ButtonA, mouseId, MOUSE_BUTTON_LEFT));
	REQUIRE(map.MapFloat(ButtonA, mouseId, MOUSE_AXIS_Y));
	REQUIRE(map.MapFloat(ButtonA, padId, PAD_BUTTON_LEFT_STICK_X));
	REQUIRE(map.IsMapped(ButtonA));

	REQUIRE(map.MapFloat(ButtonB, keyboardId, KEY_F2));
	REQUIRE(map.MapFloat(ButtonB, mouseId, MOUSE_AXIS_X));
	REQUIRE(map.IsMapped(ButtonB));

	map.Clear();
	for (int b = ButtonStart; b < ButtonCount; ++b)
	{
		REQUIRE(!map.IsMapped(b));
	}

	REQUIRE(map.MapFloat(ButtonA, mouseId, MOUSE_AXIS_X));
	REQUIRE(map.IsMapped(ButtonA));

	map.Unmap(ButtonA);
	REQUIRE(!map.IsMapped(ButtonA));

	DeviceButtonSpec mappings[32];
	REQUIRE(map.GetMappings(ButtonA, mappings, 32) == 0);

	REQUIRE(map.MapFloat(ButtonA, mouseId, MOUSE_AXIS_X));
	REQUIRE(map.MapFloat(ButtonA, keyboardId, KEY_F5));
	REQUIRE(map.MapFloat(ButtonD, padId, PAD_BUTTON_RIGHT_STICK_Y));

	REQUIRE(map.GetMappings(ButtonA, mappings, 32) == 2);
	REQUIRE(mappings[0].deviceId == mouseId);
	REQUIRE(mappings[0].buttonId == MOUSE_AXIS_X);
	REQUIRE(mappings[1].deviceId == keyboardId);
	REQUIRE(mappings[1].buttonId == KEY_F5);

	char buf[32];
	REQUIRE(map.GetUserButtonName(ButtonA, buf, 32));

	REQUIRE(map.GetUserButtonId(mouseId, MOUSE_AXIS_X) == ButtonA);
	REQUIRE(map.GetUserButtonId(keyboardId, KEY_F5) == ButtonA);
	REQUIRE(map.GetUserButtonId(padId, PAD_BUTTON_RIGHT_STICK_Y) == ButtonD);
}

TEST_CASE("InputMap/SetDeadZone_SetUserButtonPolicy", "")
{
	InputManager manager;
	const DeviceId keyboardId = manager.CreateDevice<InputDeviceKeyboard>();
	const DeviceId mouseId = manager.CreateDevice<InputDeviceMouse>();
	const DeviceId padId = manager.CreateDevice<InputDevicePad>();
	InputMap map(manager, "testmap");

	for (int b = ButtonStart; b < ButtonCount; ++b)
	{
		REQUIRE(!map.SetDeadZone(b, 0.1f));
		REQUIRE(!map.SetUserButtonPolicy(b, InputMap::UBP_AVERAGE));
	}

	REQUIRE(map.MapFloat(ButtonA, mouseId, MOUSE_AXIS_X));

	REQUIRE(map.SetDeadZone(ButtonA, 0.01f));
	REQUIRE(map.SetDeadZone(ButtonA, 0.0f));
	REQUIRE(map.SetDeadZone(ButtonA, 1.01f));
	REQUIRE(!map.SetDeadZone(ButtonF, 1.01f));

	REQUIRE(map.SetUserButtonPolicy(ButtonA, InputMap::UBP_AVERAGE));
	REQUIRE(map.SetUserButtonPolicy(ButtonA, InputMap::UBP_MAX));
	REQUIRE(map.SetUserButtonPolicy(ButtonA, InputMap::UBP_MIN));
	REQUIRE(map.SetUserButtonPolicy(ButtonA, InputMap::UBP_FIRST_DOWN));

	map.Clear();

	for (int b = ButtonStart; b < ButtonCount; ++b)
	{
		REQUIRE(!map.SetDeadZone(b, 0.1f));
		REQUIRE(!map.SetUserButtonPolicy(b, InputMap::UBP_AVERAGE));
	}
}


