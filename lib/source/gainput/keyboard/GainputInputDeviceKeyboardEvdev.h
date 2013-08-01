
#ifndef GAINPUTINPUTDEVICEKEYBOARDLINUX_H_
#define GAINPUTINPUTDEVICEKEYBOARDLINUX_H_

namespace gainput
{

class InputDeviceKeyboardImpl
{
public:
	InputDeviceKeyboardImpl(InputManager& manager, DeviceId device);
	~InputDeviceKeyboardImpl();

	void Update(InputState& state, InputState& previousState, InputDeltaState* delta);

	InputManager& GetManager() const { return manager_; }
	DeviceId GetDevice() const { return device_; }

	size_t GetKeyName(DeviceButtonId deviceButton, char* buffer, size_t bufferLength) const;
	DeviceButtonId GetButtonByName(const char* name) const;

	bool IsTextInputEnabled() const { return textInputEnabled_; }
	void SetTextInputEnabled(bool enabled) { textInputEnabled_ = enabled; }

	char GetNextCharacter()
	{
		if (!textBuffer_.CanGet())
		{
			return 0;
		}
		return textBuffer_.Get();
	}

private:
	InputManager& manager_;
	DeviceId device_;
	bool textInputEnabled_;
	RingBuffer<GAINPUT_TEXT_INPUT_QUEUE_LENGTH, char> textBuffer_;
	HashMap<unsigned, DeviceButtonId> dialect_;
	HashMap<Key, const char*> keyNames_;
	int fd_;
	InputState* state_;
	InputState* previousState_;
	InputDeltaState* delta_;
};


}

#endif

