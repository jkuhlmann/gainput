

#include <gainput/gainput.h>
#include <gainput/GainputInputDeltaState.h>

#if defined(GAINPUT_PLATFORM_LINUX)
#include <time.h>
#include <X11/Xlib.h>
#include "keyboard/GainputInputDeviceKeyboardLinux.h"
#include "mouse/GainputInputDeviceMouseLinux.h"
#elif defined(GAINPUT_PLATFORM_WIN)
#include "keyboard/GainputInputDeviceKeyboardWin.h"
#include "keyboard/GainputInputDeviceKeyboardWinRaw.h"
#include "mouse/GainputInputDeviceMouseWin.h"
#include "mouse/GainputInputDeviceMouseWinRaw.h"
#include "touch/GainputInputDeviceTouchWin.h"
#include "GainputWindows.h"
#elif defined(GAINPUT_PLATFORM_ANDROID)
#include <time.h>
#include <jni.h>
#include "keyboard/GainputInputDeviceKeyboardAndroid.h"
#include "pad/GainputInputDevicePadAndroid.h"
#include "touch/GainputInputDeviceTouchAndroid.h"
static gainput::InputManager* gGainputInputManager;
#elif defined(GAINPUT_PLATFORM_IOS) || defined(GAINPUT_PLATFORM_MAC) || defined(GAINPUT_PLATFORM_TVOS)
#include <mach/mach.h>
#include <mach/clock.h>
#endif

#include <stdlib.h>

#include "dev/GainputDev.h"
#include <gainput/GainputHelpers.h>


namespace gainput
{

InputManager::InputManager(bool useSystemTime, Allocator& allocator) :
		allocator_(allocator),
		devices_(allocator_),
		nextDeviceId_(0),
		listeners_(allocator_),
		nextListenerId_(0),
		sortedListeners_(allocator_),
		modifiers_(allocator_),
		nextModifierId_(0),
		deltaState_(allocator_.New<InputDeltaState>(allocator_)),
		currentTime_(0),
        GAINPUT_CONC_CONSTRUCT(concurrentInputs_),
		displayWidth_(-1),
		displayHeight_(-1),
		useSystemTime_(useSystemTime),
		debugRenderingEnabled_(false),
		debugRenderer_(0)
{
	GAINPUT_DEV_INIT(this);
#ifdef GAINPUT_PLATFORM_ANDROID
	gGainputInputManager = this;
#endif
}

//==========================================================================
#if defined (GAINPUT_PLATFORM_WIN)
// Capped at 128 instances per process
#define MAX_INSTANCES 128
// Container of our manager pointers and old WNDPROC callbacks to get provide context to new static WNDPROC callback.
static InputManager* sMgrMap[MAX_INSTANCES];
static WNDPROC sMgrMapOldWndProcs[MAX_INSTANCES];
// Corresponding Hwnds
static HWND sMgrMapHwnds[MAX_INSTANCES];
static int sMgrMapIdx = 0;

// Simple mutex, Windows-only, which is okay as this is a Windows-only segment
class SimpleSemaphore
{
	LONG m_counter;
	HANDLE m_semaphore;

public:
	SimpleSemaphore() { m_counter = 0;	m_semaphore = CreateSemaphore(NULL, 0, 1, NULL); }
	~SimpleSemaphore() { CloseHandle(m_semaphore);	}

	void Lock()	{
		if (_InterlockedIncrement(&m_counter) > 1) // x86/64 guarantees acquire semantics
			WaitForSingleObject(m_semaphore, INFINITE);
	}

	void Unlock() {
		if (_InterlockedDecrement(&m_counter) > 0) // x86/64 guarantees release semantics
			ReleaseSemaphore(m_semaphore, 1, NULL);
	}
};

// Make it thread safe
static SimpleSemaphore sWindProcHookMtx;

// Retrieves InputManager ptr from table using HWND ptr as key
static InputManager* HwndToInputManager(HWND hwnd) {
	for (int i = 0; i < MAX_INSTANCES; i++) {
		if (sMgrMapHwnds[i] == hwnd)
			return sMgrMap[i];
	}
	return NULL;
}

// Retrieves WNDPROC ptr from table using HWND ptr as key
static WNDPROC HwndToOldWndProc(HWND hwnd) {
	for (int i = 0; i < MAX_INSTANCES; i++) {
		if (sMgrMapHwnds[i] == hwnd)
			return sMgrMapOldWndProcs[i];
	}
	return NULL;
}

static LRESULT WindowProcHook(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Get old WNDPROC
	WNDPROC fptr = HwndToOldWndProc(hwnd);
	if (!fptr) {
		GAINPUT_LOG("%s (%d): Old WNDPROC ptr == NULL", __FILE__, __LINE__);
		return LRESULT();
	}

	// Get corresponding input manager
	InputManager* mgr = HwndToInputManager(hwnd);

	// If none exists, just pass to old WNDPROC
	if (!mgr) {
		GAINPUT_LOG("%s (%d): InputManager ptr == NULL", __FILE__, __LINE__);
		return CallWindowProc(fptr, hwnd, uMsg, wParam, lParam);
	}

	// Construct MSG, timestamp not needed
	MSG msg;
	msg.hwnd = hwnd;
	msg.message = uMsg;
	msg.wParam = wParam;
	msg.lParam = lParam;

	// Pass to manager
	mgr->HandleMessage(msg);

	// Pass back to old WNDPROC
	return CallWindowProc(fptr, hwnd, uMsg, wParam, lParam);
}

static void InputManagerWinHook(HWND hwnd, InputManager* input_manager)
{
	// Enter critical section
	sWindProcHookMtx.Lock();

	if (!hwnd) { GAINPUT_LOG("%s (%d): Window handle == NULL", __FILE__, __LINE__); sWindProcHookMtx.Unlock(); return; }

	// Get old WNDPROC, assign new callback
	WNDPROC old_wndproc = (WNDPROC)SetWindowLongPtr((HWND)hwnd, GWLP_WNDPROC, (LONG_PTR)&WindowProcHook);

	if (sMgrMapIdx == 0) {
		memset(sMgrMap, NULL, sizeof(sMgrMap));
		memset(sMgrMapHwnds, NULL, sizeof(sMgrMapHwnds));
		memset(sMgrMapOldWndProcs, NULL, sizeof(sMgrMapOldWndProcs));
	}

	sMgrMap[sMgrMapIdx] = input_manager;
	sMgrMapHwnds[sMgrMapIdx] = hwnd;
	sMgrMapOldWndProcs[sMgrMapIdx] = old_wndproc;

	++sMgrMapIdx;

	// Exit critical section
	sWindProcHookMtx.Unlock();
}


InputManager::InputManager(void* hWnd, bool useSystemTime/* = true*/, Allocator& allocator/* = GetDefaultAllocator()*/) :
	allocator_(allocator),
	devices_(allocator_),
	nextDeviceId_(0),
	listeners_(allocator_),
	nextListenerId_(0),
	sortedListeners_(allocator_),
	modifiers_(allocator_),
	nextModifierId_(0),
	deltaState_(allocator_.New<InputDeltaState>(allocator_)),
	currentTime_(0),
	GAINPUT_CONC_CONSTRUCT(concurrentInputs_),
	displayWidth_(-1),
	displayHeight_(-1),
	useSystemTime_(useSystemTime),
	debugRenderingEnabled_(false),
	debugRenderer_(0)
{
	GAINPUT_DEV_INIT(this);
	
	// Hook our custom WNDPROC to dispatch messages automatically to InputManager::HandleMessage()
	if (hWnd && (IsWindow((HWND)hWnd) == TRUE) ? true : false) {
		InputManagerWinHook((HWND)hWnd, this);
	}

}
#endif

InputManager::~InputManager()
{
	allocator_.Delete(deltaState_);

	for (DeviceMap::iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
		allocator_.Delete(it->second);
	}
	GAINPUT_DEV_SHUTDOWN(this);
}

void
InputManager::Update()
{
    Change change;
    while (GAINPUT_CONC_DEQUEUE(concurrentInputs_, change))
    {
        if (change.type == BT_BOOL)
        {
            HandleButton(*change.device, *change.state, change.delta, change.buttonId, change.b);
        }
        else if (change.type == BT_FLOAT)
        {
            HandleAxis(*change.device, *change.state, change.delta, change.buttonId, change.f);
        }
    }
    
	InputDeltaState* ds = listeners_.empty() ? 0 : deltaState_;

	for (DeviceMap::iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
		if (!it->second->IsLateUpdate())
		{
			it->second->Update(ds);
		}
	}

	GAINPUT_DEV_UPDATE(ds);

	for (HashMap<ModifierId, DeviceStateModifier*>::iterator it = modifiers_.begin();
			it != modifiers_.end();
			++it)
	{
		it->second->Update(ds);
	}

	for (DeviceMap::iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
		if (it->second->IsLateUpdate())
		{
			it->second->Update(ds);
		}
	}

	if (ds)
	{
		ds->NotifyListeners(sortedListeners_);
		ds->Clear();
	}
}

void
InputManager::Update(uint64_t deltaTime)
{
	GAINPUT_ASSERT(useSystemTime_ == false);
	currentTime_ += deltaTime;
	Update();
}

uint64_t
InputManager::GetTime() const
{
	if (useSystemTime_)
	{
#if defined(GAINPUT_PLATFORM_LINUX) || defined(GAINPUT_PLATFORM_ANDROID)
	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
	{
		return -1;
	}

	uint64_t t = ts.tv_sec*1000ul + ts.tv_nsec/1000000ul;
	return t;
#elif defined(GAINPUT_PLATFORM_WIN)
	static LARGE_INTEGER perfFreq = { 0 };
	if (perfFreq.QuadPart == 0)
	{
		QueryPerformanceFrequency(&perfFreq);
		GAINPUT_ASSERT(perfFreq.QuadPart != 0);
	}
	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	double t = 1000.0 * double(count.QuadPart) / double(perfFreq.QuadPart);
	return static_cast<uint64_t>(t);
#elif defined(GAINPUT_PLATFORM_IOS) || defined(GAINPUT_PLATFORM_MAC) || defined(GAINPUT_PLATFORM_TVOS)
	clock_serv_t cclock;
	mach_timespec_t mts;
		host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	uint64_t t = mts.tv_sec*1000ul + mts.tv_nsec/1000000ul;
	return t;
#else
#error Gainput: No time support
#endif
}
	else
	{
		return currentTime_;
	}
}

DeviceId
InputManager::FindDeviceId(const char* typeName, unsigned index) const
{
	for (DeviceMap::const_iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
		if (strcmp(typeName, it->second->GetTypeName()) == 0
			&& it->second->GetIndex() == index)
		{
			return it->first;
		}
	}
	return InvalidDeviceId;
}

DeviceId
InputManager::FindDeviceId(InputDevice::DeviceType type, unsigned index) const
{
	for (DeviceMap::const_iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
		if (it->second->GetType() == type
			&& it->second->GetIndex() == index)
		{
			return it->first;
		}
	}
	return InvalidDeviceId;
}

ListenerId
InputManager::AddListener(InputListener* listener)
{
	listeners_[nextListenerId_] = listener;
	ReorderListeners();
	return nextListenerId_++;
}

void
InputManager::RemoveListener(ListenerId listenerId)
{
	listeners_.erase(listenerId);
	ReorderListeners();
}

namespace {
static int CompareListeners(const void* a, const void* b)
{
	const InputListener* listener1 = *reinterpret_cast<const InputListener* const*>(a);
	const InputListener* listener2 = *reinterpret_cast<const InputListener* const*>(b);
	return listener2->GetPriority() - listener1->GetPriority();
}
}

void
InputManager::ReorderListeners()
{
	sortedListeners_.clear();
	for (HashMap<ListenerId, InputListener*>::iterator it = listeners_.begin();
		it != listeners_.end();
		++it)
	{
		sortedListeners_.push_back(it->second);
	}

	if (sortedListeners_.empty())
	{
		return;
	}

	qsort(&sortedListeners_[0], 
		sortedListeners_.size(), 
		sizeof(InputListener*), 
		&CompareListeners);
}

ModifierId
InputManager::AddDeviceStateModifier(DeviceStateModifier* modifier)
{
	modifiers_[nextModifierId_] = modifier;
	return nextModifierId_++;
}

void
InputManager::RemoveDeviceStateModifier(ModifierId modifierId)
{
	modifiers_.erase(modifierId);
}

size_t
InputManager::GetAnyButtonDown(DeviceButtonSpec* outButtons, size_t maxButtonCount) const
{
	size_t buttonsFound = 0;
	for (DeviceMap::const_iterator it = devices_.begin();
			it != devices_.end() && maxButtonCount > buttonsFound;
			++it)
	{
		buttonsFound += it->second->GetAnyButtonDown(outButtons+buttonsFound, maxButtonCount-buttonsFound);
	}
	return buttonsFound;
}

unsigned
InputManager::GetDeviceCountByType(InputDevice::DeviceType type) const
{
	unsigned count = 0;
	for (DeviceMap::const_iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
		if (it->second->GetType() == type)
		{
			++count;
		}
	}
	return count;
}

void
InputManager::DeviceCreated(InputDevice* device)
{
	GAINPUT_UNUSED(device);
	GAINPUT_DEV_NEW_DEVICE(device);
}

#if defined(GAINPUT_PLATFORM_LINUX)
void
InputManager::HandleEvent(XEvent& event)
{
	for (DeviceMap::const_iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
#if defined(GAINPUT_DEV)
		if (it->second->IsSynced())
		{
			continue;
		}
#endif
		if (it->second->GetType() == InputDevice::DT_KEYBOARD
			&& it->second->GetVariant() == InputDevice::DV_STANDARD)
		{
			InputDeviceKeyboard* keyboard = static_cast<InputDeviceKeyboard*>(it->second);
			InputDeviceKeyboardImplLinux* keyboardImpl = static_cast<InputDeviceKeyboardImplLinux*>(keyboard->GetPimpl());
			GAINPUT_ASSERT(keyboardImpl);
			keyboardImpl->HandleEvent(event);
		}
		else if (it->second->GetType() == InputDevice::DT_MOUSE
			&& it->second->GetVariant() == InputDevice::DV_STANDARD)
		{
			InputDeviceMouse* mouse = static_cast<InputDeviceMouse*>(it->second);
			InputDeviceMouseImplLinux* mouseImpl = static_cast<InputDeviceMouseImplLinux*>(mouse->GetPimpl());
			GAINPUT_ASSERT(mouseImpl);
			mouseImpl->HandleEvent(event);
		}
	}
}
#endif

#if defined(GAINPUT_PLATFORM_WIN)
void
InputManager::HandleMessage(const MSG& msg)
{
	for (DeviceMap::const_iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
#if defined(GAINPUT_DEV)
		if (it->second->IsSynced())
		{
			continue;
		}
#endif
		if (it->second->GetType() == InputDevice::DT_KEYBOARD)
		{
			InputDeviceKeyboard* keyboard = static_cast<InputDeviceKeyboard*>(it->second);
			if (it->second->GetVariant() == InputDevice::DV_STANDARD)
			{
				InputDeviceKeyboardImplWin* keyboardImpl = static_cast<InputDeviceKeyboardImplWin*>(keyboard->GetPimpl());
				GAINPUT_ASSERT(keyboardImpl);
				keyboardImpl->HandleMessage(msg);
			}
			else if (it->second->GetVariant() == InputDevice::DV_RAW)
			{
				InputDeviceKeyboardImplWinRaw* keyboardImpl = static_cast<InputDeviceKeyboardImplWinRaw*>(keyboard->GetPimpl());
				GAINPUT_ASSERT(keyboardImpl);
				keyboardImpl->HandleMessage(msg);
			}
		}
		else if (it->second->GetType() == InputDevice::DT_MOUSE)
		{
			InputDeviceMouse* mouse = static_cast<InputDeviceMouse*>(it->second);
			if (it->second->GetVariant() == InputDevice::DV_STANDARD)
			{
				InputDeviceMouseImplWin* mouseImpl = static_cast<InputDeviceMouseImplWin*>(mouse->GetPimpl());
				GAINPUT_ASSERT(mouseImpl);
				mouseImpl->HandleMessage(msg);
			}
			else if (it->second->GetVariant() == InputDevice::DV_RAW)
			{
				InputDeviceMouseImplWinRaw* mouseImpl = static_cast<InputDeviceMouseImplWinRaw*>(mouse->GetPimpl());
				GAINPUT_ASSERT(mouseImpl);
				mouseImpl->HandleMessage(msg);
			}
		}
#if defined(GAINPUT_PLATFORM_WIN)
		// Process touch messages for Windows for multi-touch support.
		//
		// Either pass messages directly from your own message handler, or pass an HWND window handle on the construction of
		// the InputManager to automatically hook a message pump. This latter solution is especially helpful for situations
		// where you (a) don't feel like cluttering your native-side code with extra hooks to Gainput, or (b) instances where
		// you may not have access to the native message handler but _do_ have access to the window handle, e.g., with GLFW.
		else if (it->second->GetType() == InputDevice::DT_TOUCH)
		{
			if (it->second->GetVariant() == InputDevice::DV_STANDARD)
			{
				InputDeviceTouch* touch = static_cast<InputDeviceTouch*>(it->second);
				InputDeviceTouchImplWin* touchImpl = static_cast<InputDeviceTouchImplWin*>(touch->GetPimpl());
				GAINPUT_ASSERT(touchImpl);
				touchImpl->HandleMessage(msg);
			}
		}
#endif
	}
}
#endif

#if defined(GAINPUT_PLATFORM_ANDROID)
int32_t
InputManager::HandleInput(AInputEvent* event)
{
	int handled = 0;
	for (DeviceMap::const_iterator it = devices_.begin();
			it != devices_.end();
			++it)
	{
#if defined(GAINPUT_DEV)
		if (it->second->IsSynced())
		{
			continue;
		}
#endif
		if (it->second->GetType() == InputDevice::DT_TOUCH)
		{
			InputDeviceTouch* touch = static_cast<InputDeviceTouch*>(it->second);
			InputDeviceTouchImplAndroid* touchImpl = static_cast<InputDeviceTouchImplAndroid*>(touch->GetPimpl());
			GAINPUT_ASSERT(touchImpl);
			handled |= touchImpl->HandleInput(event);
		}
		else if (it->second->GetType() == InputDevice::DT_KEYBOARD)
		{
			InputDeviceKeyboard* keyboard = static_cast<InputDeviceKeyboard*>(it->second);
			InputDeviceKeyboardImplAndroid* keyboardImpl = static_cast<InputDeviceKeyboardImplAndroid*>(keyboard->GetPimpl());
			GAINPUT_ASSERT(keyboardImpl);
			handled |= keyboardImpl->HandleInput(event);
		}
	}
	return handled;
}

void
InputManager::HandleDeviceInput(DeviceInput const& input)
{
	DeviceId devId = FindDeviceId(input.deviceType, input.deviceIndex);
	if (devId == InvalidDeviceId)
	{
		return;
	}

	InputDevice* device = GetDevice(devId);
	if (!device)
	{
		return;
	}

#if defined(GAINPUT_DEV)
	if (device->IsSynced())
	{
		return;
	}
#endif

    InputState* state = device->GetNextInputState();
    if (!state)
    {
        state = device->GetInputState();
    }
	if (!state)
	{
		return;
	}

	if (input.buttonType == BT_BOOL)
	{
		EnqueueConcurrentChange(*device, *state, deltaState_, input.buttonId, input.value.b);
	}
	else if (input.buttonType == BT_FLOAT)
	{
		EnqueueConcurrentChange(*device, *state, deltaState_, input.buttonId, input.value.f);
	}
	else if (input.buttonType == BT_COUNT && input.deviceType == InputDevice::DT_PAD)
	{
		InputDevicePad* pad = static_cast<InputDevicePad*>(device);
		InputDevicePadImplAndroid* impl = static_cast<InputDevicePadImplAndroid*>(pad->GetPimpl());
		GAINPUT_ASSERT(impl);
		if (input.value.b)
		{
			impl->SetState(InputDevice::DeviceState::DS_OK);
		}
		else
		{
			impl->SetState(InputDevice::DeviceState::DS_UNAVAILABLE);
		}
	}
}

#endif

void
InputManager::ConnectForStateSync(const char* ip, unsigned port)
{
	GAINPUT_UNUSED(ip); GAINPUT_UNUSED(port);
	GAINPUT_DEV_CONNECT(this, ip, port);
}

void
InputManager::StartDeviceStateSync(DeviceId deviceId)
{
	GAINPUT_ASSERT(GetDevice(deviceId));
	GAINPUT_ASSERT(GetDevice(deviceId)->GetType() != InputDevice::DT_GESTURE);
	GAINPUT_DEV_START_SYNC(deviceId);
}

void
InputManager::SetDebugRenderingEnabled(bool enabled)
{
	debugRenderingEnabled_ = enabled;
	if (enabled)
	{
		GAINPUT_ASSERT(debugRenderer_);
	}
}

void
InputManager::SetDebugRenderer(DebugRenderer* debugRenderer)
{
	debugRenderer_ = debugRenderer;
}

void
InputManager::EnqueueConcurrentChange(InputDevice& device, InputState& state, InputDeltaState* delta, DeviceButtonId buttonId, bool value)
{
    Change change;
    change.device = &device;
    change.state = &state;
    change.delta = delta;
    change.buttonId = buttonId;
    change.type = BT_BOOL;
    change.b = value;
    GAINPUT_CONC_ENQUEUE(concurrentInputs_, change);
}

void
InputManager::EnqueueConcurrentChange(InputDevice& device, InputState& state, InputDeltaState* delta, DeviceButtonId buttonId, float value)
{
    Change change;
    change.device = &device;
    change.state = &state;
    change.delta = delta;
    change.buttonId = buttonId;
    change.type = BT_FLOAT;
    change.f = value;
    GAINPUT_CONC_ENQUEUE(concurrentInputs_, change);
}

}

#if defined(GAINPUT_PLATFORM_ANDROID)
extern "C" {
JNIEXPORT void JNICALL
Java_de_johanneskuhlmann_gainput_Gainput_nativeOnInputBool(JNIEnv * /*env*/, jobject /*thiz*/,
                                                           jint deviceType, jint deviceIndex,
                                                           jint buttonId, jboolean value)
{
	if (!gGainputInputManager)
	{
		return;
	}
	using namespace gainput;
    InputManager::DeviceInput input;
    input.deviceType = static_cast<InputDevice::DeviceType>(deviceType);
    input.deviceIndex = deviceIndex;
    input.buttonType = BT_BOOL;
	if (input.deviceType == InputDevice::DT_KEYBOARD)
	{
		DeviceId deviceId = gGainputInputManager->FindDeviceId(input.deviceType, deviceIndex);
		if (deviceId != InvalidDeviceId)
		{
			InputDevice* device = gGainputInputManager->GetDevice(deviceId);
			if (device)
			{
				InputDeviceKeyboard* keyboard = static_cast<InputDeviceKeyboard*>(device);
				InputDeviceKeyboardImplAndroid* keyboardImpl = static_cast<InputDeviceKeyboardImplAndroid*>(keyboard->GetPimpl());
				GAINPUT_ASSERT(keyboardImpl);
				DeviceButtonId newId = keyboardImpl->Translate(buttonId);
				if (newId != InvalidDeviceButtonId)
				{
					buttonId = newId;
				}
			}
		}
	}
    input.buttonId = buttonId;
    input.value.b = value;
    gGainputInputManager->HandleDeviceInput(input);
}

JNIEXPORT void JNICALL
Java_de_johanneskuhlmann_gainput_Gainput_nativeOnInputFloat(JNIEnv * /*env*/, jobject /*thiz*/,
                                                            jint deviceType, jint deviceIndex,
                                                            jint buttonId, jfloat value)
{
	if (!gGainputInputManager)
	{
		return;
	}
	using namespace gainput;
    InputManager::DeviceInput input;
    input.deviceType = static_cast<InputDevice::DeviceType>(deviceType);
    input.deviceIndex = deviceIndex;
    input.buttonType = BT_FLOAT;
    input.buttonId = buttonId;
    input.value.f = value;
    gGainputInputManager->HandleDeviceInput(input);
}

JNIEXPORT void JNICALL
Java_de_johanneskuhlmann_gainput_Gainput_nativeOnDeviceChanged(JNIEnv * /*env*/, jobject /*thiz*/,
                                                               jint deviceId, jboolean value)
{
	if (!gGainputInputManager)
	{
		return;
	}
    using namespace gainput;
    InputManager::DeviceInput input;
    input.deviceType = InputDevice::DT_PAD;
    input.deviceIndex = deviceId;
    input.buttonType = BT_COUNT;
    input.value.b = value;
    gGainputInputManager->HandleDeviceInput(input);
}
}
#endif
