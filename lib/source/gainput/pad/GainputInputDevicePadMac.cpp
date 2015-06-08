
#include <gainput/gainput.h>

#ifdef GAINPUT_PLATFORM_MAC

#include "GainputInputDevicePadImpl.h"
#include "../GainputInputDeltaState.h"
#include "../GainputHelpers.h"
#include "../GainputLog.h"

#include "GainputInputDevicePadMac.h"

#import <CoreFoundation/CoreFoundation.h>
#import <IOKit/hid/IOHIDManager.h>
#import <IOKit/hid/IOHIDUsageTables.h>


namespace gainput
{

extern bool MacIsApplicationKey();

namespace {

static void OnDeviceInput(void* inContext, IOReturn inResult, void* inSender, IOHIDValueRef value)
{
	if (!MacIsApplicationKey())
	{
		return;
	}

	IOHIDElementRef elem = IOHIDValueGetElement(value);

	InputDevicePadImplMac* device = reinterpret_cast<InputDevicePadImplMac*>(inContext);
	GAINPUT_ASSERT(device);

	uint32_t usagePage = IOHIDElementGetUsagePage(elem);
	uint32_t usage = IOHIDElementGetUsage(elem);

	CFIndex state = (int)IOHIDValueGetIntegerValue(value);
	float analog = IOHIDValueGetScaledValue(value, kIOHIDValueScaleTypePhysical);
	analog = analog < device->minAxis_ ? device->minAxis_ : analog > device->maxAxis_ ? device->maxAxis_ : analog; // clamp
	analog -= device->minAxis_;
	analog /= (Abs(device->minAxis_) + Abs(device->maxAxis_))*0.5f;
	anaglog -= 1.0f;

	if (usagePage == kHIDPage_Button && device->buttonDialect_.count(usage))
	{
		const DeviceButtonId buttonId = device->buttonDialect_[usage];
		HandleButton(device->device_, device->nextState_, device->delta_, buttonId, state != 0);
	}
	else if (usagePage == kHIDPage_GenericDesktop)
	{
		if (usage == kHIDUsage_GD_Hatswitch)
		{
			int dpadX = 0;
			int dpadY = 0;
			switch(state)
			{
				case  0: dpadX =  0; dpadY =  1; break;
				case  1: dpadX =  1; dpadY =  1; break;
				case  2: dpadX =  1; dpadY =  0; break;
				case  3: dpadX =  1; dpadY = -1; break;
				case  4: dpadX =  0; dpadY = -1; break;
				case  5: dpadX = -1; dpadY = -1; break;
				case  6: dpadX = -1; dpadY =  0; break;
				case  7: dpadX = -1; dpadY =  1; break;
				default: dpadX =  0; dpadY =  0; break;
			}
			HandleButton(device->device_, device->nextState_, device->delta_, PadButtonLeft, dpadX < 0);
			HandleButton(device->device_, device->nextState_, device->delta_, PadButtonRight, dpadX > 0);
			HandleButton(device->device_, device->nextState_, device->delta_, PadButtonUp, dpadY > 0);
			HandleButton(device->device_, device->nextState_, device->delta_, PadButtonDown, dpadY < 0);
		}
		else if (device->axisDialect_.count(usage))
		{
			const DeviceButtonId buttonId = device->axisDialect_[usage];
			HandleAxes(device->device_, device->nextState_, device->delta_, buttonId, analog);
		}
	}
#ifdef GAINPUT_DEBUG
	else
	{
		GAINPUT_LOG("Unmapped button: %d\n", usage);
	}
#endif
}

static void OnDeviceConnected(void* inContext, IOReturn inResult, void* inSender, IOHIDDeviceRef inIOHIDDeviceRef)
{
	InputDevicePadImplMac* device = reinterpret_cast<InputDevicePadImplMac*>(inContext);
	GAINPUT_ASSERT(device);
	device->deviceState_ = InputDevice::DS_OK;

	long vendorId = 0;
	long productId = 0;

	IOHIDDevice_GetLongProperty(inIOHIDDeviceRef, CFSTR(kIOHIDVendorIDKey), &vendorId);
	IOHIDDevice_GetLongProperty(inIOHIDDeviceRef, CFSTR(kIOHIDProductIDKey), &productId);

	if (vendorId == 0x054c && productId == 0x5c4) // Sony DualShock 4
	{
		device->minAxis_ = 0;
		device->maxAxis_ = 256;
		device->axisDialect_[kHIDUsage_GD_X] = PadButtonLeftStickX;
		device->axisDialect_[kHIDUsage_GD_Y] = PadButtonLeftStickY;
		device->axisDialect_[kHIDUsage_GD_Z] = PadButtonRightStickX;
		device->axisDialect_[kHIDUsage_GD_Rz] = PadButtonRightStickY;
		device->axisDialect_[kHIDUsage_GD_Rx] = PadButtonAxis4;
		device->axisDialect_[kHIDUsage_GD_Ry] = PadButtonAxis5;
		//device->buttonDialect_[0] = PadButtonSelect;
		//device->buttonDialect_[1] = PadButtonL3;
		//device->buttonDialect_[2] = PadButtonR3;
		device->buttonDialect_[0x0A] = PadButtonStart;
		//device->buttonDialect_[4] = PadButtonUp;
		//device->buttonDialect_[5] = PadButtonRight;
		//device->buttonDialect_[6] = PadButtonDown;
		//device->buttonDialect_[7] = PadButtonLeft;
		device->buttonDialect_[0x05] = PadButtonL1;
		device->buttonDialect_[0x06] = PadButtonL2;
		//device->buttonDialect_[] = PadButtonL2;
		//device->buttonDialect_[] = PadButtonR2;
		device->buttonDialect_[0x04] = PadButtonY;
		device->buttonDialect_[0x03] = PadButtonB;
		device->buttonDialect_[0x02] = PadButtonA;
		device->buttonDialect_[0x01] = PadButtonX;
		//device->buttonDialect_[] = PadButtonHome;
	}
	else if (vendorId == 0x045e && (productId == 0x028E || productId == 0x028F)) // Microsoft 360 Controller wired/wireless
	{
		device->minAxis_ = -(1<<15);
		device->maxAxis_ = 1<<15;
		device->axisDialect_[kHIDUsage_GD_X] = PadButtonLeftStickX;
		device->axisDialect_[kHIDUsage_GD_Y] = PadButtonLeftStickY;
		device->axisDialect_[kHIDUsage_GD_Rx] = PadButtonRightStickX;
		device->axisDialect_[kHIDUsage_GD_Ry] = PadButtonRightStickY;
		device->axisDialect_[kHIDUsage_GD_Z] = PadButtonAxis4;
		device->axisDialect_[kHIDUsage_GD_Rz] = PadButtonAxis5;
		//device->buttonDialect_[0] = PadButtonSelect;
		//device->buttonDialect_[1] = PadButtonL3;
		//device->buttonDialect_[2] = PadButtonR3;
		device->buttonDialect_[0x09] = PadButtonStart;
		device->buttonDialect_[0x0c] = PadButtonUp;
		device->buttonDialect_[0x0f] = PadButtonRight;
		device->buttonDialect_[0x0d] = PadButtonDown;
		device->buttonDialect_[0x0e] = PadButtonLeft;
		//device->buttonDialect_[0x05] = PadButtonL2;
		//device->buttonDialect_[0x06] = PadButtonR2;
		device->buttonDialect_[0x05] = PadButtonL1;
		device->buttonDialect_[0x06] = PadButtonR1;
		device->buttonDialect_[0x04] = PadButtonY;
		device->buttonDialect_[0x02] = PadButtonB;
		device->buttonDialect_[0x01] = PadButtonA;
		device->buttonDialect_[0x03] = PadButtonX;
		//device->buttonDialect_[] = PadButtonHome;
	}
}

static void OnDeviceRemoved(void* inContext, IOReturn inResult, void* inSender, IOHIDDeviceRef inIOHIDDeviceRef)
{
	InputDevicePadImplMac* device = reinterpret_cast<InputDevicePadImplMac*>(inContext);
	GAINPUT_ASSERT(device);
	device->deviceState_ = InputDevice::DS_UNAVAILABLE;
}

}

InputDevicePadImplMac::InputDevicePadImplMac(InputManager& manager, InputDevice& device, InputState& state, InputState& previousState) :
	minAxis_(-FLT_MAX),
	maxAxis_(FLT_MAX),
	manager_(manager),
	device_(device),
	deviceState_(InputDevice::DS_UNAVAILABLE),
	textInputEnabled_(true),
	dialect_(manager_.GetAllocator()),
	state_(&state),
	previousState_(&previousState),
	nextState_(manager.GetAllocator(), KeyCount_),
	delta_(0)
{
	IOHIDManagerRef ioManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDManagerOptionNone);

	if (CFGetTypeID(ioManager) != IOHIDManagerGetTypeID())
	{
		return;
	}

	ioManager_ = ioManager;

	static const unsigned kKeyCount = 2;

	CFStringRef keys[kKeyCount] = {
		CFSTR(kIOHIDDeviceUsagePageKey),
		CFSTR(kIOHIDDeviceUsageKey),
	};

	int usagePage = kHIDPage_GenericDesktop;
	int usage = kHIDUsage_GD_GamePad;
	CFNumberRef values[kKeyCount] = {
		CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &usagePage),
		CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &usage),
	};

	CFArrayRef matchingArray = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);

	CFDictionaryRef matchingDict = CFDictionaryCreate(kCFAllocatorDefault,
			(const void **) keys, (const void **) values, kKeyCount,
			&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFArrayAppendValue(matchingArray, matchingDict);
	CFRelease(matchingDict);
	CFRelease(values[1]);

	usage = kHIDUsage_GD_MultiAxisController;
	values[1] = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &usage);
	
	matchingDict = CFDictionaryCreate(kCFAllocatorDefault,
			(const void **) keys, (const void **) values, kKeyCount,
			&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFArrayAppendValue(matchingArray, matchingDict);
	CFRelease(matchingDict);

	for (unsigned i = 0; i < kKeyCount; ++i)
	{
		CFRelease(keys[i]);
		CFRelease(values[i]);
	}

	IOHIDManagerSetDeviceMatchingMultiple(ioManager, matchingArray);
	CFRelease(matchingArray);

	IOHIDManagerRegisterDeviceMatchingCallback(ioManager, OnDeviceConnected, this);
	IOHIDManagerRegisterDeviceRemovalCallback(ioManager, OnDeviceRemoved, this);
	IOHIDManagerRegisterInputValueCallback(ioManager, OnDeviceInput, this);

	IOHIDManagerOpen(ioManager, kIOHIDOptionsTypeNone);

	IOHIDManagerScheduleWithRunLoop(ioManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

}

InputDevicePadImplMac::~InputDevicePadImplMac()
{
	IOHIDManagerRef ioManager = reinterpret_cast<IOHIDManagerRef>(ioManager_);
	IOHIDManagerUnscheduleFromRunLoop(ioManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
	IOHIDManagerClose(ioManager, 0);
	CFRelease(ioManager);
}

void InputDevicePadImplMac::Update(InputDeltaState* delta)
{
	delta_ = delta;
	*state_ = nextState_;
}

bool InputDevicePadImplMac::IsValidButton(DeviceButtonId deviceButton) const
{
	if (buttonDialect_.empty())
	{
		return deviceButton < PadButtonMax_;
	}

	for (HashMap<unsigned, DeviceButtonId>::const_iterator it = buttonDialect_.begin();
			it != buttonDialect_.end();
			++it)
	{
		if (it->second == deviceButton)
		{
			return true;
		}
	}

	for (HashMap<unsigned, DeviceButtonId>::const_iterator it = axisDialect_.begin();
			it != axisDialect_.end();
			++it)
	{
		if (it->second == deviceButton)
		{
			return true;
		}
	}

	return false;
}

}

#endif

