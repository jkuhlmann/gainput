
#ifndef GAINPUTINPUTPLAYER_H_
#define GAINPUTINPUTPLAYER_H_

#ifdef GAINPUT_ENABLE_RECORDER

namespace gainput
{

class InputPlayer
{
public:
	InputPlayer(InputManager& manager, InputRecording* recording = 0);

	void Update();

	void SetRecording(InputRecording* recording);
	InputRecording* GetRecording() { return recording_; }
	const InputRecording* GetRecording() const { return recording_; }

	void Start();
	void Stop();
	bool IsPlaying() const { return isPlaying_; }

private:
	InputManager& manager_;

	bool isPlaying_;
	InputRecording* recording_;
	uint64_t startTime_;

	Array<DeviceId> devicesToReset_;
};

}

#endif

#endif

