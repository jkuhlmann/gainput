
#include <gainput/gainput.h>


#if defined(GAINPUT_PLATFORM_ANDROID)

#include "../GainputInputDeltaState.h"
#include "../GainputHelpers.h"


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
		if (!sensorEventQueue_)
		{
			return;
		}

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
				HandleAxis(device_, state, previousState, delta, PAD_BUTTON_ACCELERATION_X, event.acceleration.x / ASENSOR_STANDARD_GRAVITY);
				HandleAxis(device_, state, previousState, delta, PAD_BUTTON_ACCELERATION_Y, event.acceleration.y / ASENSOR_STANDARD_GRAVITY);
				HandleAxis(device_, state, previousState, delta, PAD_BUTTON_ACCELERATION_Z, event.acceleration.z / ASENSOR_STANDARD_GRAVITY);
			}
			else if (event.type == ASENSOR_TYPE_GYROSCOPE)
			{
				HandleAxis(device_, state, previousState, delta, PAD_BUTTON_GYROSCOPE_X, event.vector.x / ASENSOR_STANDARD_GRAVITY);
				HandleAxis(device_, state, previousState, delta, PAD_BUTTON_GYROSCOPE_Y, event.vector.y / ASENSOR_STANDARD_GRAVITY);
				HandleAxis(device_, state, previousState, delta, PAD_BUTTON_GYROSCOPE_Z, event.vector.z / ASENSOR_STANDARD_GRAVITY);
			}
			else if (event.type == ASENSOR_TYPE_MAGNETIC_FIELD)
			{
				HandleAxis(device_, state, previousState, delta, PAD_BUTTON_MAGNETICFIELD_X, event.magnetic.x / ASENSOR_MAGNETIC_FIELD_EARTH_MAX);
				HandleAxis(device_, state, previousState, delta, PAD_BUTTON_MAGNETICFIELD_Y, event.magnetic.y / ASENSOR_MAGNETIC_FIELD_EARTH_MAX);
				HandleAxis(device_, state, previousState, delta, PAD_BUTTON_MAGNETICFIELD_Z, event.magnetic.z / ASENSOR_MAGNETIC_FIELD_EARTH_MAX);
			}
		}
	}

	InputManager& GetManager() const { return manager_; }
	DeviceId GetDevice() const { return device_; }

	InputDevice::DeviceState GetState() const
	{
		return state_;
	}

	bool IsValidButton(DeviceButtonId deviceButton) const
	{
		if (deviceButton >= PAD_BUTTON_ACCELERATION_X && deviceButton <= PAD_BUTTON_ACCELERATION_Z && accelerometerSensor_)
		{
			return true;
		}
		
		if (deviceButton >= PAD_BUTTON_GYROSCOPE_X && deviceButton <= PAD_BUTTON_GYROSCOPE_Z && gyroscopeSensor_)
		{
			return true;
		}

		if (deviceButton >= PAD_BUTTON_MAGNETICFIELD_X && deviceButton <= PAD_BUTTON_MAGNETICFIELD_Z && magneticFieldSensor_)
		{
			return true;
		}

		return false;
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

}

#include "GainputPadCommon.h"

namespace gainput
{

bool
InputDevicePad::Vibrate(float leftMotor, float rightMotor)
{
	return false;
}

}

#endif
