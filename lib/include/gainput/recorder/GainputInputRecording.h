
#ifndef GAINPUTINPUTRECORDING_H_
#define GAINPUTINPUTRECORDING_H_

#ifdef GAINPUT_ENABLE_RECORDER

namespace gainput
{

struct RecordedDeviceButtonChange
{
	uint64_t time;
	DeviceId deviceId;
	DeviceButtonId buttonId;

	union
	{
		bool b;
		float f;
	};
};

class InputRecording
{
public:
	InputRecording(Allocator& allocator = GetDefaultAllocator());
	InputRecording(InputManager& manager, void* data, size_t size, Allocator& allocator = GetDefaultAllocator());

	void AddChange(uint64_t time, DeviceId deviceId, DeviceButtonId buttonId, bool value);
	void AddChange(uint64_t time, DeviceId deviceId, DeviceButtonId buttonId, float value);
	void Clear();

	/// Gets the next button change before and including the given time and returns it.
	bool GetNextChange(uint64_t time, RecordedDeviceButtonChange& outChange);
	void Reset() { position_ = 0; }

	uint64_t GetDuration() const;

	size_t GetSerializedSize() const;
	void GetSerialized(InputManager& manager, void* data) const;

private:
	Array<RecordedDeviceButtonChange> changes_;

	/// The position in changes_ for reading.
	unsigned position_;
};

}

#endif

#endif

