
#include <gainput/gainput.h>

#ifdef GAINPUT_ENABLE_RECORDER

namespace gainput
{

InputRecording::InputRecording(Allocator& allocator) :
	changes_(allocator),
	position_(0)
{
}

void
InputRecording::AddChange(uint64_t time, DeviceId deviceId, DeviceButtonId buttonId, bool value)
{
	RecordedDeviceButtonChange change;
	change.time = time;
	change.deviceId = deviceId;
	change.buttonId = buttonId;
	change.b = value;
	changes_.push_back(change);
}

void
InputRecording::AddChange(uint64_t time, DeviceId deviceId, DeviceButtonId buttonId, float value)
{
	RecordedDeviceButtonChange change;
	change.time = time;
	change.deviceId = deviceId;
	change.buttonId = buttonId;
	change.f = value;
	changes_.push_back(change);
}

void
InputRecording::Clear()
{
	changes_.clear();
}

bool
InputRecording::GetNextChange(uint64_t time, RecordedDeviceButtonChange& outChange)
{
	if (position_ >= changes_.size())
	{
		return false;
	}

	if (changes_[position_].time > time)
	{
		return false;
	}

	outChange = changes_[position_];
	++position_;

	return true;
}

uint64_t
InputRecording::GetDuration() const
{
	return changes_.empty() ? 0 : changes_[changes_.size() - 1].time;
}

}

#endif

