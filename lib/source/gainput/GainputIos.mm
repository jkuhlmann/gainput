
#include <gainput/gainput.h>

#ifdef GAINPUT_PLATFORM_IOS

#include <gainput/GainputIos.h>

#include "touch/GainputInputDeviceTouchIos.h"

@implementation GainputView
{
	gainput::InputManager* inputManager_;
}

- (id)initWithFrame:(CGRect)frame inputManager:(gainput::InputManager&)inputManager
{
	self = [super initWithFrame:frame];
	if (self)
	{
		inputManager_ = &inputManager;
		[self setMultipleTouchEnabled:YES];
	}
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	gainput::DeviceId deviceId = inputManager_->FindDeviceId(gainput::InputDevice::DT_TOUCH, 0);
	if (deviceId == gainput::InvalidDeviceId)
	{
		return;
	}
	gainput::InputDeviceTouch* device = static_cast<gainput::InputDeviceTouch*>(inputManager_->GetDevice(deviceId));
	if (!device)
	{
		return;
	}
	gainput::InputDeviceTouchImplIos* deviceImpl = static_cast<gainput::InputDeviceTouchImplIos*>(device->GetPimpl());
	if (!deviceImpl)
	{
		return;
	}


	for (UITouch *touch in touches)
	{
		CGPoint point = [touch locationInView:self];

		float x = point.x / self.bounds.size.width;
		float y = point.y / self.bounds.size.height;

		deviceImpl->HandleTouch(static_cast<void*>(touch), x, y);
	}
}


- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	gainput::DeviceId deviceId = inputManager_->FindDeviceId(gainput::InputDevice::DT_TOUCH, 0);
	if (deviceId == gainput::InvalidDeviceId)
	{
		return;
	}
	gainput::InputDeviceTouch* device = static_cast<gainput::InputDeviceTouch*>(inputManager_->GetDevice(deviceId));
	if (!device)
	{
		return;
	}
	gainput::InputDeviceTouchImplIos* deviceImpl = static_cast<gainput::InputDeviceTouchImplIos*>(device->GetPimpl());
	if (!deviceImpl)
	{
		return;
	}

	for (UITouch *touch in touches)
	{
		CGPoint point = [touch locationInView:self];

		float x = point.x / self.bounds.size.width;
		float y = point.y / self.bounds.size.height;

		deviceImpl->HandleTouch(static_cast<void*>(touch), x, y);
	}
}


- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	gainput::DeviceId deviceId = inputManager_->FindDeviceId(gainput::InputDevice::DT_TOUCH, 0);
	if (deviceId == gainput::InvalidDeviceId)
	{
		return;
	}
	gainput::InputDeviceTouch* device = static_cast<gainput::InputDeviceTouch*>(inputManager_->GetDevice(deviceId));
	if (!device)
	{
		return;
	}
	gainput::InputDeviceTouchImplIos* deviceImpl = static_cast<gainput::InputDeviceTouchImplIos*>(device->GetPimpl());
	if (!deviceImpl)
	{
		return;
	}

	for (UITouch *touch in touches)
	{
		CGPoint point = [touch locationInView:self];

		float x = point.x / self.bounds.size.width;
		float y = point.y / self.bounds.size.height;

		deviceImpl->HandleTouchEnd(static_cast<void*>(touch), x, y);
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	[self touchesEnded:touches withEvent:event];
}

@end

#endif
