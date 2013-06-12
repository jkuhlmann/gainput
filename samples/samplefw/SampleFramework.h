
#ifndef SAMPLEFRAMEWORK_H_
#define SAMPLEFRAMEWORK_H_


void SfwOpenWindow(const char* title);
void SfwCloseWindow();
void SfwUpdate();
bool SfwIsDone();

void SfwSetInputManager(gainput::InputManager* manager);

int SfwGetWidth();
int SfwGetHeight();


#if defined(GAINPUT_PLATFORM_LINUX)

Display* SfwGetXDisplay();
#include <stdio.h>
#define SFW_LOG(...) printf(__VA_ARGS__);

#elif defined(GAINPUT_PLATFORM_WIN)

HWND SfwGetHWnd();

#include <stdio.h>
#define SFW_LOG(...) { char buf[1024]; sprintf(buf, __VA_ARGS__); OutputDebugStringA(buf); }

#elif defined(GAINPUT_PLATFORM_ANDROID)

#include <android/log.h>
#define SFW_LOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, "gainput", __VA_ARGS__))

#endif

#endif

