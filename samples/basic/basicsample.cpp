
#include <gainput/gainput.h>

#if defined(GAINPUT_PLATFORM_LINUX)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <GL/glx.h>
#endif

#include <iostream>


enum Button
{
	ButtonMenu,
	ButtonConfirm
};


const char* windowName = "Gainput basic sample";
const int width = 800;
const int height = 600;


#if defined(GAINPUT_PLATFORM_LINUX)
int main(int argc, char** argv)
{
	
	gainput::InputManager manager;
	gainput::DeviceId mouseId = manager.CreateDevice<gainput::InputDeviceMouse>();
	gainput::DeviceId keyboardId = manager.CreateDevice<gainput::InputDeviceKeyboard>();
	gainput::DeviceId padId = manager.CreateDevice<gainput::InputDevicePad>();
	
	gainput::InputMap map(manager);
	map.MapBool(ButtonMenu, keyboardId, gainput::KEY_ESCAPE);
	map.MapBool(ButtonConfirm, mouseId, gainput::MOUSE_BUTTON_LEFT);
	map.MapBool(ButtonConfirm, padId, gainput::PAD_BUTTON_A);

	static int attributeListDbl[] = {      GLX_RGBA,      GLX_DOUBLEBUFFER, /*In case single buffering is not supported*/      GLX_RED_SIZE,   1,      GLX_GREEN_SIZE, 1,      GLX_BLUE_SIZE,  1,
			None };

	Display* xDisplay = XOpenDisplay(0);
	if (xDisplay == 0)
	{
		std::cerr << "Cannot connect to X server." << std::endl;
		return -1;
	}

	Window root = DefaultRootWindow(xDisplay);

	XVisualInfo* vi = glXChooseVisual(xDisplay, DefaultScreen(xDisplay), attributeListDbl);
	assert(vi);

	GLXContext context = glXCreateContext(xDisplay, vi, 0, GL_TRUE);

	Colormap cmap = XCreateColormap(xDisplay, root,                   vi->visual, AllocNone);

	XSetWindowAttributes swa;
	swa.colormap = cmap;
	swa.event_mask = ExposureMask
		| KeyPressMask | KeyReleaseMask
		| PointerMotionMask | ButtonPressMask | ButtonReleaseMask;

	Window xWindow = XCreateWindow(
		xDisplay, root,
		0, 0, width, height, 0,
		CopyFromParent, InputOutput,
		CopyFromParent, CWEventMask,
		&swa
		);


	/* connect the context to the window */
	glXMakeCurrent(xDisplay, xWindow, context);


	XSetWindowAttributes xattr;

	xattr.override_redirect = False;
	XChangeWindowAttributes(xDisplay, xWindow, CWOverrideRedirect, &xattr);

	XMapWindow(xDisplay, xWindow);
	XStoreName(xDisplay, xWindow, windowName);
	
	manager.SetXDisplay(xDisplay, width, height);
	
	for (;;)
	{
		manager.Update();

		XEvent xEvent;
		const long eventMask = (~0) & ~(KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
		while (XCheckMaskEvent(xDisplay, eventMask, &xEvent))
		{
			//XNextEvent(xDisplay, &xEvent);
			if (xEvent.type == KeyPress
					|| xEvent.type == KeyRelease
					|| xEvent.type == MotionNotify
					|| xEvent.type == ButtonPress
					|| xEvent.type == ButtonRelease)
			{
				std::cout << "We shouldn't get here: " << xEvent.type << " | " <<MotionNotify << std::endl;
			}
		}

		if (map.GetBoolWasDown(ButtonMenu))
		{
			std::cout << "Open menu!!" << std::endl;
		}
		if (map.GetBoolWasDown(ButtonConfirm))
		{
			std::cout << "Confirmed!!" << std::endl;
		}

	}
	
	XDestroyWindow(xDisplay, xWindow);
	XCloseDisplay(xDisplay);
	
	return 0;
}
#endif


#if defined(GAINPUT_PLATFORM_WIN)
#include <windows.h>

static char szWindowClass[] = "win32app";
static char szTitle[] = "Gainput basic sample";

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    char greeting[] = "Hello, World!";

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        TextOut(hdc, 5, 5, greeting, strlen(greeting));
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL, "Call to RegisterClassEx failed!", "Gainput basic sample", NULL);
        return 1;
    }

    HWND hWnd = CreateWindow(
        szWindowClass,
        windowName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL, "Call to CreateWindow failed!", "Gainput basic sample", NULL);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

	gainput::InputManager manager;
	manager.SetDisplaySize(width, height);
	gainput::DeviceId mouseId = manager.CreateDevice<gainput::InputDeviceMouse>();
	gainput::DeviceId keyboardId = manager.CreateDevice<gainput::InputDeviceKeyboard>();
	gainput::DeviceId padId = manager.CreateDevice<gainput::InputDevicePad>();
	
	gainput::InputMap map(manager);
	map.MapBool(ButtonMenu, keyboardId, gainput::KEY_ESCAPE);
	map.MapBool(ButtonConfirm, mouseId, gainput::MOUSE_BUTTON_LEFT);
	map.MapBool(ButtonConfirm, padId, gainput::PAD_BUTTON_A);
    
	// Main message loop:
	for (;;)
	{
		manager.Update();
		MSG msg;

		while (PeekMessage(&msg, hWnd,  0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			manager.HandleMessage(msg);
		}

		if (map.GetBoolWasDown(ButtonConfirm))
		{
			gainput::InputDevicePad* pad = static_cast<gainput::InputDevicePad*>(manager.GetDevice(padId));
			pad->Vibrate(1.0f, 0.0f);
		}
		if (map.GetBoolWasDown(ButtonMenu))
		{
			gainput::InputDevicePad* pad = static_cast<gainput::InputDevicePad*>(manager.GetDevice(padId));
			pad->Vibrate(0.0f, 0.0f);
		}
	}

	return 0;
}
#endif

