
#include <gainput/gainput.h>

#ifdef GAINPUT_PLATFORM_MAC

#include "GainputInputDeviceMouseMac.h"

#include "../GainputInputDeltaState.h"
#include "../GainputHelpers.h"
#include "../GainputLog.h"

#import <AppKit/AppKit.h>
#import <Carbon/Carbon.h>

namespace {

static float titleBarHeight()
{
	NSRect frame = NSMakeRect (0, 0, 100, 100);
	NSRect contentRect = [NSWindow contentRectForFrameRect:frame
		styleMask:NSTitledWindowMask];
	return (frame.size.height - contentRect.size.height);
}

CGEventRef MouseTap(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void* user)
{
	gainput::InputDeviceMouseImplMac* device = reinterpret_cast<gainput::InputDeviceMouseImplMac*>(user);
	GAINPUT_ASSERT(device->previousState_);

	if (type == kCGEventTapDisabledByTimeout
			|| type == kCGEventTapDisabledByUserInput)
	{
		// Probably timeout, re-enable!
		CGEventTapEnable(reinterpret_cast<CFMachPortRef>(device->eventTap_), true);
	}

	NSApplication* app = [NSApplication sharedApplication];
	NSWindow* window = app.keyWindow;
	if (window)
	{
		CGPoint theLocation = CGEventGetLocation(event);
		NSRect rect = NSMakeRect(theLocation.x, theLocation.y, 0, 0);
		NSRect rect2 = [window convertRectFromScreen:rect];
		NSScreen* screen = window.screen;
		rect2.origin.y = screen.frame.size.height - rect.origin.y - window.frame.origin.y + screen.frame.origin.y;
		rect2.origin.y = window.frame.size.height - rect2.origin.y - titleBarHeight();
		const float x = rect2.origin.x / window.frame.size.width;
		const float y = rect2.origin.y / window.frame.size.height;

		gainput::HandleAxis(device->device_, device->nextState_, device->delta_, gainput::MouseAxisX, x);
		gainput::HandleAxis(device->device_, device->nextState_, device->delta_, gainput::MouseAxisY, y);

		if (type == kCGEventLeftMouseDown || type == kCGEventLeftMouseUp)
		{
			gainput::HandleButton(device->device_, device->nextState_, device->delta_, gainput::MouseButton0, type == kCGEventLeftMouseDown);
		}
		else if (type == kCGEventRightMouseDown || type == kCGEventRightMouseUp)
		{
			gainput::HandleButton(device->device_, device->nextState_, device->delta_, gainput::MouseButton2, type == kCGEventRightMouseDown);
		}
		else if (type == kCGEventOtherMouseDown || type == kCGEventOtherMouseUp)
		{
			int buttonNum = CGEventGetIntegerValueField(event, kCGMouseEventButtonNumber);
			gainput::DeviceButtonId buttonId = gainput::MouseButton1;
			if (buttonNum > kCGMouseButtonCenter)
			{
				buttonId = gainput::MouseButton3 + buttonNum - kCGMouseButtonCenter;
			}
			gainput::HandleButton(device->device_, device->nextState_, device->delta_, buttonId, type == kCGEventOtherMouseDown);
		}
	}

	return event;
}

}

namespace gainput
{

InputDeviceMouseImplMac::InputDeviceMouseImplMac(InputManager& manager, InputDevice& device, InputState& state, InputState& previousState) :
	manager_(manager),
	device_(device),
	state_(&state),
	previousState_(&previousState),
	nextState_(manager.GetAllocator(), MouseButtonCount + MouseAxisCount),
	delta_(0)
{
	CGEventMask eventMask =
		CGEventMaskBit(kCGEventLeftMouseDown)
		| CGEventMaskBit(kCGEventLeftMouseUp)
		| CGEventMaskBit(kCGEventRightMouseDown)
		| CGEventMaskBit(kCGEventRightMouseUp)
		| CGEventMaskBit(kCGEventOtherMouseDown)
		| CGEventMaskBit(kCGEventOtherMouseUp)
		| CGEventMaskBit(kCGEventMouseMoved)
		| CGEventMaskBit(kCGEventLeftMouseDragged)
		| CGEventMaskBit(kCGEventRightMouseDragged)
		;

	CFMachPortRef eventTap = CGEventTapCreate(kCGSessionEventTap,
			kCGHeadInsertEventTap,   
			0,   
			eventMask,
			MouseTap,
			this);

	if (!eventTap)
	{  
		GAINPUT_ASSERT(false);
		return;
	}

	eventTap_ = eventTap;

	CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);

	CGEventTapEnable(eventTap, true);
}

InputDeviceMouseImplMac::~InputDeviceMouseImplMac()
{
	CFRelease(reinterpret_cast<CFMachPortRef>(eventTap_));
}

}

#endif

