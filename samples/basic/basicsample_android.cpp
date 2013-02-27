
#include <gainput/gainput.h>

#if defined(GAINPUT_PLATFORM_ANDROID)

#include <jni.h>
#include <errno.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "gainput", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "gainput", __VA_ARGS__))


// Define your user buttons
enum Button
{
	ButtonMenu,
	ButtonConfirm
};


static int32_t MyHandleInput(struct android_app* app, AInputEvent* event)
{
	// Forward input events to Gainput
	gainput::InputManager* inputManager = (gainput::InputManager*)app->userData;
	static bool resSet = false;
	if (!resSet)
	{
		inputManager->SetDisplaySize(ANativeWindow_getWidth(app->window), ANativeWindow_getHeight(app->window));
		resSet = true;
	}
	return inputManager->HandleInput(event);
}

void android_main(struct android_app* state)
{
	app_dummy();

	// Set up Gainput
	gainput::InputManager manager;
	const gainput::DeviceId touchId = manager.CreateDevice<gainput::InputDeviceTouch>();
	const gainput::DeviceId keyboardId = manager.CreateDevice<gainput::InputDeviceKeyboard>();

	// Make sure the app frowards input events to Gainput
	state->userData = &manager;
	state->onInputEvent = &MyHandleInput;

	for (;;)
	{
		// Update Gainput
		manager.Update();

		int ident;
		int events;
		struct android_poll_source* source;

		while ((ident=ALooper_pollAll(-1, NULL, &events, (void**)&source)) >= 0)
		{
			if (source != NULL)
			{
				source->process(state, source);
			}

			if (state->destroyRequested != 0)
			{
				return;
			}
		}
	}
}


#endif

