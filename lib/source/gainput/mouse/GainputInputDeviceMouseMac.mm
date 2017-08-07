
#include <gainput/gainput.h>

#ifdef GAINPUT_PLATFORM_MAC

#include "GainputInputDeviceMouseMac.h"

#include <gainput/GainputInputDeltaState.h>
#include <gainput/GainputHelpers.h>
#include <gainput/GainputLog.h>

#import <AppKit/AppKit.h>
#import <Carbon/Carbon.h>

namespace {

static float theoreticalTitleBarHeight()
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

    gainput::InputManager& manager = device->manager_;
	NSApplication* app = [NSApplication sharedApplication];
	NSWindow* window = app.keyWindow;
	if (window)
	{
		CGPoint theLocation = CGEventGetUnflippedLocation(event);
        
		NSRect rect = NSMakeRect(theLocation.x, theLocation.y, 0, 0);
		NSRect rect2 = [window convertRectFromScreen:rect];
        
        float titleBarHeight = (window.styleMask & NSFullScreenWindowMask) ? 0.0f : theoreticalTitleBarHeight();
        const float x = rect2.origin.x / window.frame.size.width;
        const float y = 1.0f - (rect2.origin.y / (window.frame.size.height - titleBarHeight));

		manager.EnqueueConcurrentChange(device->device_, device->nextState_, device->delta_, gainput::MouseAxisX, x);
		manager.EnqueueConcurrentChange(device->device_, device->nextState_, device->delta_, gainput::MouseAxisY, y);

		if (type == kCGEventLeftMouseDown || type == kCGEventLeftMouseUp)
		{
			manager.EnqueueConcurrentChange(device->device_, device->nextState_, device->delta_, gainput::MouseButton0, type == kCGEventLeftMouseDown);
		}
		else if (type == kCGEventRightMouseDown || type == kCGEventRightMouseUp)
		{
			manager.EnqueueConcurrentChange(device->device_, device->nextState_, device->delta_, gainput::MouseButton2, type == kCGEventRightMouseDown);
		}
		else if (type == kCGEventOtherMouseDown || type == kCGEventOtherMouseUp)
		{
			int buttonNum = CGEventGetIntegerValueField(event, kCGMouseEventButtonNumber);
			gainput::DeviceButtonId buttonId = gainput::MouseButton1;
			if (buttonNum > kCGMouseButtonCenter)
			{
				buttonId = gainput::MouseButton3 + buttonNum - kCGMouseButtonCenter;
			}
			manager.EnqueueConcurrentChange(device->device_, device->nextState_, device->delta_, buttonId, type == kCGEventOtherMouseDown);
		}
		else if (type == kCGEventScrollWheel)
		{
			int const deltaAxis = CGEventGetIntegerValueField(event, kCGScrollWheelEventDeltaAxis1);
            if (deltaAxis != 0)
            {
                manager.EnqueueConcurrentChange(
                    device->device_,
                    device->nextState_,
                    device->delta_,
                    deltaAxis > 0 ? gainput::MouseButtonWheelDown : gainput::MouseButtonWheelUp,
                    true);
            }
		}
	}
    else
    {
        // Window was unfocused.
        for (unsigned i = gainput::MouseButton0; i < gainput::MouseButtonCount; ++ i)
        {
            gainput::HandleButton(device->device_, device->nextState_, device->delta_, i, false);
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
        | CGEventMaskBit(kCGEventOtherMouseDragged)
        | CGEventMaskBit(kCGEventScrollWheel)
		;

	CFMachPortRef eventTap = CGEventTapCreate(kCGSessionEventTap,
			kCGHeadInsertEventTap,
			kCGEventTapOptionDefault,
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


void InputDeviceMouseImplMac::Update(InputDeltaState* delta)
{
    delta_ = delta;
    
    // Reset mouse wheel buttons
    HandleButton(device_, nextState_, delta_, MouseButtonWheelUp, false);
    HandleButton(device_, nextState_, delta_, MouseButtonWheelDown, false);

    *state_ = nextState_;
}

}

#endif

