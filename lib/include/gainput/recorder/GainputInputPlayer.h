
#ifndef GAINPUTINPUTPLAYER_H_
#define GAINPUTINPUTPLAYER_H_

#ifdef GAINPUT_ENABLE_RECORDER

namespace gainput
{

class InputPlayer : public DeviceStateModifier
{
public:
	InputPlayer(InputManager& manager, InputRecording* recording = 0);
	~InputPlayer();

	void Update(InputDeltaState* delta);

	void Start();
	void Stop();
	bool IsPlaying() const { return isPlaying_; }

	void SetRecording(InputRecording* recording);
	InputRecording* GetRecording() { return recording_; }
	const InputRecording* GetRecording() const { return recording_; }

private:
	InputManager& manager_;

	bool isPlaying_;
	InputRecording* recording_;
	uint64_t startTime_;

	Array<DeviceId> devicesToReset_;

	ModifierId playingModifierId_;
};

}

#endif

#endif

