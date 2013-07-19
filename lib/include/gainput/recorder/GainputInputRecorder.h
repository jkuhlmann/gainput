
#ifndef GAINPUTINPUTRECORDER_H_
#define GAINPUTINPUTRECORDER_H_

#ifdef GAINPUT_ENABLE_RECORDER

namespace gainput
{

class InputRecorder
{
public:
	InputRecorder(InputManager& manager);
	~InputRecorder();

	void Start();
	void Stop();
	bool IsRecording() const { return isRecording_; }

	void AddDeviceToRecord(DeviceId device) { recordedDevices_[device] = true; }
	bool IsDeviceToRecord(DeviceId device) { return recordedDevices_.empty() || recordedDevices_.count(device) > 0; }

	InputRecording* GetRecording() { return recording_; }
	const InputRecording* GetRecording() const { return recording_; }
	uint64_t GetStartTime() const { return startTime_; }

private:
	InputManager& manager_;

	bool isRecording_;
	InputListener* recordingListener_;
	ListenerId recordingListenerId_;
	InputRecording* recording_;
	uint64_t startTime_;
	HashMap<DeviceId, bool> recordedDevices_;

};

}

#endif

#endif

