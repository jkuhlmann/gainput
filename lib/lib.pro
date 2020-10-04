TARGET = gainput
TEMPLATE = lib
CONFIG += staticlib

DESTDIR = $${OUT_PWD}

#win32:DEFINES += GAINPUT_LIB_DYNAMIC=1
#win32:LIBS += -lws2_32 -luser32 -lxinput

INCLUDEPATH += include

HEADERS += include\gainput\gainput.h \
    include\gainput\GainputAllocator.h \
    include\gainput\GainputContainers.h \
    include\gainput\GainputDebugRenderer.h \
    include\gainput\GainputHelpers.h \
    include\gainput\GainputInputDeltaState.h \
    include\gainput\GainputInputDevice.h \
    include\gainput\GainputInputDeviceBuiltIn.h \
    include\gainput\GainputInputDeviceKeyboard.h \
    include\gainput\GainputInputDeviceMouse.h \
    include\gainput\GainputInputDevicePad.h \
    include\gainput\GainputInputDeviceTouch.h \
    include\gainput\GainputInputListener.h \
    include\gainput\GainputInputManager.h \
    include\gainput\GainputInputMap.h \
    include\gainput\GainputInputState.h \
    include\gainput\GainputIos.h \
    include\gainput\GainputLog.h \
    include\gainput\GainputMapFilters.h \
    include\gainput\gestures\GainputButtonStickGesture.h \
    include\gainput\gestures\GainputDoubleClickGesture.h \
    include\gainput\gestures\GainputGestures.h \
    include\gainput\gestures\GainputHoldGesture.h \
    include\gainput\gestures\GainputPinchGesture.h \
    include\gainput\gestures\GainputRotateGesture.h \
    include\gainput\gestures\GainputSimultaneouslyDownGesture.h \
    include\gainput\gestures\GainputTapGesture.h \
    include\gainput\recorder\GainputInputPlayer.h \
    include\gainput\recorder\GainputInputRecorder.h \
    include\gainput\recorder\GainputInputRecording.h \
    source\gainput\GainputHelpersEvdev.h \
    source\gainput\GainputWindows.h \
    source\gainput\builtin\GainputInputDeviceBuiltInAndroid.h \
    source\gainput\builtin\GainputInputDeviceBuiltInImpl.h \
    source\gainput\builtin\GainputInputDeviceBuiltInIos.h \
    source\gainput\builtin\GainputInputDeviceBuiltInNull.h \
    source\gainput\dev\GainputDev.h \
    source\gainput\dev\GainputDevProtocol.h \
    source\gainput\dev\GainputMemoryStream.h \
    source\gainput\dev\GainputNetAddress.h \
    source\gainput\dev\GainputNetConnection.h \
    source\gainput\dev\GainputNetListener.h \
    source\gainput\dev\GainputStream.h \
    source\gainput\keyboard\GainputInputDeviceKeyboardAndroid.h \
    source\gainput\keyboard\GainputInputDeviceKeyboardEvdev.h \
    source\gainput\keyboard\GainputInputDeviceKeyboardImpl.h \
    source\gainput\keyboard\GainputInputDeviceKeyboardLinux.h \
    source\gainput\keyboard\GainputInputDeviceKeyboardMac.h \
    source\gainput\keyboard\GainputInputDeviceKeyboardNull.h \
    source\gainput\keyboard\GainputInputDeviceKeyboardWin.h \
    source\gainput\keyboard\GainputInputDeviceKeyboardWinRaw.h \
    source\gainput\keyboard\GainputKeyboardKeyNames.h \
    source\gainput\mouse\GainputInputDeviceMouseEvdev.h \
    source\gainput\mouse\GainputInputDeviceMouseImpl.h \
    source\gainput\mouse\GainputInputDeviceMouseLinux.h \
    source\gainput\mouse\GainputInputDeviceMouseMac.h \
    source\gainput\mouse\GainputInputDeviceMouseNull.h \
    source\gainput\mouse\GainputInputDeviceMouseWin.h \
    source\gainput\mouse\GainputInputDeviceMouseWinRaw.h \
    source\gainput\mouse\GainputMouseInfo.h \
    source\gainput\pad\GainputInputDevicePadAndroid.h \
    source\gainput\pad\GainputInputDevicePadImpl.h \
    source\gainput\pad\GainputInputDevicePadIos.h \
    source\gainput\pad\GainputInputDevicePadLinux.h \
    source\gainput\pad\GainputInputDevicePadMac.h \
    source\gainput\pad\GainputInputDevicePadNull.h \
    source\gainput\pad\GainputInputDevicePadWin.h \
    source\gainput\touch\GainputInputDeviceTouchAndroid.h \
    source\gainput\touch\GainputInputDeviceTouchImpl.h \
    source\gainput\touch\GainputInputDeviceTouchIos.h \
    source\gainput\touch\GainputInputDeviceTouchNull.h \
    source\gainput\touch\GainputTouchInfo.h

SOURCES += source\gainput\gainput.cpp \
    source\gainput\GainputAllocator.cpp \
    source\gainput\GainputInputDeltaState.cpp \
    source\gainput\GainputInputDevice.cpp \
    source\gainput\GainputInputManager.cpp \
    source\gainput\GainputInputMap.cpp \
    source\gainput\GainputInputState.cpp \
    source\gainput\GainputMapFilters.cpp \
    source\gainput\builtin\GainputInputDeviceBuiltIn.cpp \
    source\gainput\dev\GainputDev.cpp \
    source\gainput\dev\GainputMemoryStream.cpp \
    source\gainput\dev\GainputNetAddress.cpp \
    source\gainput\dev\GainputNetConnection.cpp \
    source\gainput\dev\GainputNetListener.cpp \
    source\gainput\gestures\GainputButtonStickGesture.cpp \
    source\gainput\gestures\GainputDoubleClickGesture.cpp \
    source\gainput\gestures\GainputHoldGesture.cpp \
    source\gainput\gestures\GainputPinchGesture.cpp \
    source\gainput\gestures\GainputRotateGesture.cpp \
    source\gainput\gestures\GainputSimultaneouslyDownGesture.cpp \
    source\gainput\gestures\GainputTapGesture.cpp \
    source\gainput\keyboard\GainputInputDeviceKeyboard.cpp \
    source\gainput\keyboard\GainputInputDeviceKeyboardMac.cpp \
    source\gainput\mouse\GainputInputDeviceMouse.cpp \
    source\gainput\pad\GainputInputDevicePad.cpp \
    source\gainput\pad\GainputInputDevicePadMac.cpp \
    source\gainput\recorder\GainputInputPlayer.cpp \
    source\gainput\recorder\GainputInputRecorder.cpp \
    source\gainput\recorder\GainputInputRecording.cpp \
    source\gainput\touch\GainputInputDeviceTouch.cpp
