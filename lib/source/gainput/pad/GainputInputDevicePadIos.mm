#include <gainput/gainput.h>

#ifdef GAINPUT_PLATFORM_IOS

#include "GainputInputDevicePadImpl.h"
#include "../GainputInputDeltaState.h"
#include "../GainputHelpers.h"
#include "../GainputLog.h"

#include "GainputInputDevicePadIos.h"

#import <GameController/GameController.h>

namespace gainput
{

InputDevicePadImplIos::InputDevicePadImplIos(InputManager& manager, InputDevice& device, unsigned index, InputState& state, InputState& previousState)
	: pausePressed_(false),
	manager_(manager),
	device_(device),
	index_(index),
	padFound_(false),
	state_(state),
	previousState_(previousState),
	deviceState_(InputDevice::DS_UNAVAILABLE),
	isExtended_(false),
	supportsMotion_(false)
{
}

InputDevicePadImplIos::~InputDevicePadImplIos()
{
}

void InputDevicePadImplIos::Update(InputDeltaState* delta)
{
	// Detect controllers
	GCController* controller = 0;
	NSArray* controllers = [GCController controllers];
	const std::size_t controllerCount = [controllers count];
	for (std::size_t i = 0; i < controllerCount; ++i)
	{
		NSInteger playerIndex = [controllers[i] playerIndex];
		if (!padFound_ && playerIndex == GCControllerPlayerIndexUnset)
		{
			controller = controllers[i];
			controller.playerIndex = index_ - 1;

			__block InputDevicePadImplIos* block_deviceImpl = this;
			controller.controllerPausedHandler = ^(GCController* controller)
			{
				block_deviceImpl->pausePressed_ = true;
			};

			break;
		}
		else if ([controllers[i] playerIndex] == index_ - 1)
		{
			controller = controllers[i];
			break;
		}
	}

	if (!controller)
	{
		deviceState_ = InputDevice::DS_UNAVAILABLE;
		isExtended_ = false;
		supportsMotion_ = false;
		return;
	}

	deviceState_ = InputDevice::DS_OK;

	if (GCExtendedGamepad* gamepad = [controller extendedGamepad])
	{
		isExtended_ = true;

		HandleButton(device_, state_, delta, PadButtonL1, gamepad.leftShoulder.pressed);
		HandleButton(device_, state_, delta, PadButtonR1, gamepad.rightShoulder.pressed);

		HandleButton(device_, state_, delta, PadButtonLeft, gamepad.dpad.left.pressed);
		HandleButton(device_, state_, delta, PadButtonRight, gamepad.dpad.right.pressed);
		HandleButton(device_, state_, delta, PadButtonUp, gamepad.dpad.up.pressed);
		HandleButton(device_, state_, delta, PadButtonDown, gamepad.dpad.down.pressed);

		HandleButton(device_, state_, delta, PadButtonA, gamepad.buttonA.pressed);
		HandleButton(device_, state_, delta, PadButtonB, gamepad.buttonB.pressed);
		HandleButton(device_, state_, delta, PadButtonX, gamepad.buttonX.pressed);
		HandleButton(device_, state_, delta, PadButtonY, gamepad.buttonY.pressed);

		HandleAxis(device_, state_, delta, PadButtonLeftStickX, gamepad.leftThumbstick.xAxis.value);
		HandleAxis(device_, state_, delta, PadButtonLeftStickY, gamepad.leftThumbstick.yAxis.value);

		HandleAxis(device_, state_, delta, PadButtonRightStickX, gamepad.rightThumbstick.xAxis.value);
		HandleAxis(device_, state_, delta, PadButtonRightStickY, gamepad.rightThumbstick.yAxis.value);

		HandleButton(device_, state_, delta, PadButtonL2, gamepad.leftTrigger.pressed);
		HandleAxis(device_, state_, delta, PadButtonAxis4, gamepad.leftTrigger.value);
		HandleButton(device_, state_, delta, PadButtonR2, gamepad.rightTrigger.pressed);
		HandleAxis(device_, state_, delta, PadButtonAxis5, gamepad.rightTrigger.value);
	}
	else if (GCGamepad* gamepad = [controller gamepad])
	{
		isExtended_ = false;

		HandleButton(device_, state_, delta, PadButtonL1, gamepad.leftShoulder.pressed);
		HandleButton(device_, state_, delta, PadButtonR1, gamepad.rightShoulder.pressed);

		HandleButton(device_, state_, delta, PadButtonLeft, gamepad.dpad.left.pressed);
		HandleButton(device_, state_, delta, PadButtonRight, gamepad.dpad.right.pressed);
		HandleButton(device_, state_, delta, PadButtonUp, gamepad.dpad.up.pressed);
		HandleButton(device_, state_, delta, PadButtonDown, gamepad.dpad.down.pressed);

		HandleButton(device_, state_, delta, PadButtonA, gamepad.buttonA.pressed);
		HandleButton(device_, state_, delta, PadButtonB, gamepad.buttonB.pressed);
		HandleButton(device_, state_, delta, PadButtonX, gamepad.buttonX.pressed);
		HandleButton(device_, state_, delta, PadButtonY, gamepad.buttonY.pressed);
	}

	if (GCMotion* motion = [controller motion])
	{
		supportsMotion_ = true;

		HandleAxis(device_, state_, delta, PadButtonAccelerationX, motion.userAcceleration.x);
		HandleAxis(device_, state_, delta, PadButtonAccelerationY, motion.userAcceleration.y);
		HandleAxis(device_, state_, delta, PadButtonAccelerationZ, motion.userAcceleration.z);

		HandleAxis(device_, state_, delta, PadButtonGravityX, motion.gravity.x);
		HandleAxis(device_, state_, delta, PadButtonGravityY, motion.gravity.y);
		HandleAxis(device_, state_, delta, PadButtonGravityZ, motion.gravity.z);

		const float gyroX = 2.0f * (motion.attitude.x * motion.attitude.z + motion.attitude.w * motion.attitude.y);
		const float gyroY = 2.0f * (motion.attitude.y * motion.attitude.z - motion.attitude.w * motion.attitude.x);
		const float gyroZ = 1.0f - 2.0f * (motion.attitude.x * motion.attitude.x + motion.attitude.y * motion.attitude.y);
		HandleAxis(device_, state_, delta, PadButtonGyroscopeX, gyroX);
		HandleAxis(device_, state_, delta, PadButtonGyroscopeY, gyroY);
		HandleAxis(device_, state_, delta, PadButtonGyroscopeZ, gyroZ);
	}

	HandleButton(device_, state_, delta, PadButtonHome, pausePressed_);
	pausePressed_ = false;
}

bool InputDevicePadImplIos::IsValidButton(DeviceButtonId deviceButton) const
{
	if (supportsMotion_ && deviceButton >= PadButtonAccelerationX && deviceButton <= PadButtonMagneticFieldZ)
	{
		return true;
	}

	if (isExtended_)
	{
		return (deviceButton >= PadButtonLeftStickX && deviceButton <= PadButtonAxis5)
			|| (deviceButton >= PadButtonLeft && deviceButton <= PadButtonR2)
			|| deviceButton == PadButtonHome;
	}

	return (deviceButton >= PadButtonLeft && deviceButton <= PadButtonR1)
		|| deviceButton == PadButtonHome;
}

}

#endif

