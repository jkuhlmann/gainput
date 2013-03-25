
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
		buttonDialect_(manager_.GetAllocator()),
		sensorManager_(0),
		accelerometerSensor_(0),
		gyroscopeSensor_(0),
		sensorEventQueue_(0)
	{
		ALooper* looper = ALooper_forThread();
		if (!looper)
		{
			looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
		}

		if (!looper)
		{
			return;
		}

		sensorManager_ = ASensorManager_getInstance();
		if (!sensorManager_)
		{
			return;
		}


		sensorEventQueue_ = ASensorManager_createEventQueue(sensorManager_, looper, ALOOPER_POLL_CALLBACK, NULL, NULL);

		accelerometerSensor_ = ASensorManager_getDefaultSensor(sensorManager_, ASENSOR_TYPE_ACCELEROMETER);
		if (accelerometerSensor_)
		{
			ASensorEventQueue_setEventRate(sensorEventQueue_, accelerometerSensor_, ASensor_getMinDelay(accelerometerSensor_));
			ASensorEventQueue_enableSensor(sensorEventQueue_, accelerometerSensor_);
		}

		gyroscopeSensor_ = ASensorManager_getDefaultSensor(sensorManager_, ASENSOR_TYPE_GYROSCOPE);
		if (gyroscopeSensor_)
		{
			ASensorEventQueue_setEventRate(sensorEventQueue_, gyroscopeSensor_, ASensor_getMinDelay(gyroscopeSensor_));
			ASensorEventQueue_enableSensor(sensorEventQueue_, gyroscopeSensor_);
		}

		magneticFieldSensor_ = ASensorManager_getDefaultSensor(sensorManager_, ASENSOR_TYPE_MAGNETIC_FIELD);
		if (magneticFieldSensor_)
		{
			ASensorEventQueue_setEventRate(sensorEventQueue_, magneticFieldSensor_, ASensor_getMinDelay(magneticFieldSensor_));
			ASensorEventQueue_enableSensor(sensorEventQueue_, magneticFieldSensor_);
		}

		state_ = InputDevice::DS_OK;
	}

	~InputDevicePadImpl()
	{
		if (!sensorManager_)
		{
			return;
		}

		if (!sensorEventQueue_)
		{
			return;
		}


		if (accelerometerSensor_)
		{
			ASensorEventQueue_disableSensor(sensorEventQueue_, accelerometerSensor_);
		}

		if (gyroscopeSensor_)
		{
			ASensorEventQueue_disableSensor(sensorEventQueue_, gyroscopeSensor_);
		}

		if (magneticFieldSensor_)
		{
			ASensorEventQueue_disableSensor(sensorEventQueue_, magneticFieldSensor_);
		}

		ASensorManager_destroyEventQueue(sensorManager_, sensorEventQueue_);
	}

	void Update(InputState& state, InputState& previousState, InputDeltaState* delta)
	{
		ASensorEvent event;

		while (ASensorEventQueue_getEvents(sensorEventQueue_, &event, 1) > 0)
		{
			if (event.type == ASENSOR_TYPE_ACCELEROMETER)
			{
				HandleFloat(state, previousState, delta, PAD_BUTTON_ACCELERATION_X, event.acceleration.x / ASENSOR_STANDARD_GRAVITY);
				HandleFloat(state, previousState, delta, PAD_BUTTON_ACCELERATION_Y, event.acceleration.y / ASENSOR_STANDARD_GRAVITY);
				HandleFloat(state, previousState, delta, PAD_BUTTON_ACCELERATION_Z, event.acceleration.z / ASENSOR_STANDARD_GRAVITY);
			}
			else if (event.type == ASENSOR_TYPE_GYROSCOPE)
			{
				HandleFloat(state, previousState, delta, PAD_BUTTON_GYROSCOPE_X, event.vector.x / ASENSOR_STANDARD_GRAVITY);
				HandleFloat(state, previousState, delta, PAD_BUTTON_GYROSCOPE_Y, event.vector.y / ASENSOR_STANDARD_GRAVITY);
				HandleFloat(state, previousState, delta, PAD_BUTTON_GYROSCOPE_Z, event.vector.z / ASENSOR_STANDARD_GRAVITY);
			}
			else if (event.type == ASENSOR_TYPE_MAGNETIC_FIELD)
			{
				HandleFloat(state, previousState, delta, PAD_BUTTON_MAGNETICFIELD_X, event.magnetic.x / ASENSOR_MAGNETIC_FIELD_EARTH_MAX);
				HandleFloat(state, previousState, delta, PAD_BUTTON_MAGNETICFIELD_Y, event.magnetic.y / ASENSOR_MAGNETIC_FIELD_EARTH_MAX);
				HandleFloat(state, previousState, delta, PAD_BUTTON_MAGNETICFIELD_Z, event.magnetic.z / ASENSOR_MAGNETIC_FIELD_EARTH_MAX);
			}
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

	InputManager& GetManager() const { return manager_; }
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
	const ASensor* gyroscopeSensor_;
	const ASensor* magneticFieldSensor_;
	ASensorEventQueue* sensorEventQueue_;
};



InputDevicePad::InputDevicePad(InputManager& manager, DeviceId device)
{
	impl_ = manager.GetAllocator().New<InputDevicePadImpl>(manager, device);
	GAINPUT_ASSERT(impl_);
	state_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), PadButtonCount + PadAxisCount);
	GAINPUT_ASSERT(state_);
	previousState_ = manager.GetAllocator().New<InputState>(manager.GetAllocator(), PadButtonCount + PadAxisCount);
	GAINPUT_ASSERT(previousState_);
}

InputDevicePad::~InputDevicePad()
{
	impl_->GetManager().GetAllocator().Delete(state_);
	impl_->GetManager().GetAllocator().Delete(previousState_);
	impl_->GetManager().GetAllocator().Delete(impl_);
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

DeviceButtonId
InputDevicePad::GetButtonByName(const char* name) const
{
	for (unsigned i = 0; i < PadButtonCount + PadAxisCount; ++i)
	{
		if (strcmp(name, deviceButtonInfos[i].name) == 0)
		{
			return DeviceButtonId(i);
		}
	}
	return InvalidDeviceButtonId;
}

bool
InputDevicePad::Vibrate(float leftMotor, float rightMotor)
{
	return false;
}

}

#endif
