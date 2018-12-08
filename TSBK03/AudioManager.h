#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include "AudioListener.h"

#include <glm/glm.hpp>

class AudioManager
{
public:
	AudioManager();

	~AudioManager();

	AudioListener& getAudioListener();

	void setGain(float value);
	float getGain() const;

	void setMuted(bool value);
	bool getMuted() const;

private:

	ALCcontext *_context{ nullptr };
	ALCdevice *_device{ nullptr };

	float _gain{ 1.f };
	bool _muted{ true };

	AudioListener _audioListener;
};
