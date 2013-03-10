
#include <gainput/gainput.h>


#if defined(GAINPUT_PLATFORM_ANDROID)

#include "../GainputInputDeltaState.h"
#include "GainputPadInfo.h"


namespace gainput
{


class InputDevicePadImpl
{
public:
	InputDevicePadImpl(InputManager& manager, DeviceId device) :
		manager_(manager),
		device_(device),
		state_(InputDevice::DS_UNAVAILABLE),
		buttonDialect_(manager_.GetAllocator())
	{
		ALooper* looper = ALooper_forThread();
		if (!looper)
			looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
		sensorManager_ = ASensorManager_getInstance();
		accelerometerSensor_ = ASensorManager_getDefaultSensor(sensorManager_, ASENSOR_TYPE_ACCELEROMETER);
		sensorEventQueue_ = ASensorManager_createEventQueue(sensorManager_, looper, ALOOPER_POLL_CALLBACK, NULL, NULL);
		ASensorEventQueue_setEventRate(sensorEventQueue_, accelerometerSensor_, ASensor_getMinDelay(accelerometerSensor_));
		ASensorEventQueue_enableSensor(sensorEventQueue_, accelerometerSensor_);
		state_ = InputDevice::DS_OK;
	}

	~InputDevicePadImpl()
	{
	}

	void Update(InputState& state, InputState& previousState, InputDeltaState* delta)
	{
		ASensorEvent event;

		while (ASensorEventQueue_getEvents(sensorEventQueue_, &event, 1) > 0)
		{
			HandleFloat(state, previousState, delta, PAD_BUTTON_ACCELERATION_X, event.acceleration.x / ASENSOR_STANDARD_GRAVITY);
			HandleFloat(state, previousState, delta, PAD_BUTTON_ACCELERATION_Y, event.acceleration.y / ASENSOR_STANDARD_GRAVITY);
			HandleFloat(state, previousState, delta, PAD_BUTTON_ACCELERATION_Z, event.acceleration.z / ASENSOR_STANDARD_GRAVITY);
		}
	}

	void HandleFloat(InputState& state, InputState& previousState, InputDeltaState* delta, DeviceButtonId buttonId, float value)
	{
		state.Set(buttonId, value);
		
#ifdef GAINPUT_DEBUG
		if (value != previousState.GetFloat(buttonId))
		{
			GAINPUT_LOG("Pad changed: %d, %f\n", buttonId, value);
		}
#endif

		if (delta)
		{
			const float oldValue = previousState.GetFloat(buttonId);
			if (value != oldValue)
			{
				delta->AddChange(device_, buttonId, oldValue, value);
			}
		}
	}

	DeviceId GetDevice() const { return device_; }

	InputDevice::DeviceState GetState() const
	{
		return state_;
	}

private:
	InputManager& manager_;
	DeviceId device_;
	InputDevice::DeviceState state_;
	HashMap<unsigned, DeviceButtonId> buttonDialect_;
	ASensorManager* sensorManager_;
	const ASensor* accelerometerSensor_;
	ASensorEventQueue* sensorEventQueue_;
};



InputDevicePad::InputDevicePad(InputManager& manager, DeviceId device) :
	impl_(new InputDevicePadImpl(manager, device))
{
	GAINPUT_ASSERT(impl_);
	state_ = new InputState(manager.GetAllocator(), PadButtonCount + PadAxisCount);
	GAINPUT_ASSERT(state_);
	previousState_ = new InputState(manager.GetAllocator(), PadButtonCount + PadAxisCount);
	GAINPUT_ASSERT(previousState_);
}

InputDevicePad::~InputDevicePad()
{
	delete state_;
	delete previousState_;
	delete impl_;
}

void
InputDevicePad::Update(InputDeltaState* delta)
{
	*previousState_ = *state_;
	impl_->Update(*state_, *previousState_, delta);
}

InputDevice::DeviceState
InputDevicePad::GetState() const
{
	return impl_->GetState();
}

size_t
InputDevicePad::GetAnyButtonDown(DeviceButtonSpec* outButtons, size_t maxButtonCount) const
{
	return CheckAllButtonsDown(outButtons, maxButtonCount, PAD_BUTTON_LEFT_STICK_X, PAD_BUTTON_MAX, impl_->GetDevice());
}

size_t
InputDevicePad::GetButtonName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	strncpy(buffer, deviceButtonInfos[deviceButton].name, bufferLength);
	buffer[bufferLength-1] = 0;
	const size_t nameLen = strlen(deviceButtonInfos[deviceButton].name);
	return nameLen >= bufferLength ? bufferLength : nameLen+1;
}

ButtonType
InputDevicePad::GetButtonType(DeviceButtonId deviceButton) const
{
	GAINPUT_ASSERT(IsValidButtonId(deviceButton));
	return deviceButtonInfos[deviceButton].type;
}

bool
InputDevicePad::Vibrate(float leftMotor, float rightMotor)
{
	return false;
}

}

#endif
