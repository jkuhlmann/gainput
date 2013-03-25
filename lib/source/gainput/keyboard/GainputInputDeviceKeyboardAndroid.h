
#ifndef GAINPUTINPUTDEVICEKEYBOARDANDROID_H_
#define GAINPUTINPUTDEVICEKEYBOARDANDROID_H_


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

	int32_t HandleInput(AInputEvent* event);

private:
	InputManager& manager_;
	DeviceId device_;
	bool textInputEnabled_;
	RingBuffer<GAINPUT_TEXT_INPUT_QUEUE_LENGTH, char> textBuffer_;
	HashMap<unsigned, DeviceButtonId> dialect_;
	HashMap<Key, const char*> keyNames_;
	InputState* state_;
	InputState* previousState_;
	InputDeltaState* delta_;
};


}

#endif

