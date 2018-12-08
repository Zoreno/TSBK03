#include "AudioManager.h"

#include <iostream>

AudioManager::AudioManager()
{
	const ALCchar *devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);

	const ALCchar *device = devices, *next = devices + 1;
	size_t len = 0;

	fprintf(stdout, "Devices list:\n");
	fprintf(stdout, "----------\n");
	while (device && *device != '\0' && next && *next != '\0') {
		fprintf(stdout, "%s\n", device);
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
	fprintf(stdout, "----------\n");

	ALCenum error;

	_device = alcOpenDevice(nullptr);

	error = alcGetError(_device);

	if(error != ALC_NO_ERROR)
	{
		std::cout << "Error opening OpenAL device" << std::endl;
	}

	_context = alcCreateContext(_device, nullptr);

	error = alcGetError(_device);

	if (error != ALC_NO_ERROR)
	{
		std::cout << "Error opening OpenAL context" << std::endl;
	}

	alcMakeContextCurrent(_context);

	_audioListener.setGain(_muted ? 0.f : _gain);
}

AudioManager::~AudioManager()
{
	alcMakeContextCurrent(_context);

	alcDestroyContext(_context);

	alcCloseDevice(_device);
}

AudioListener & AudioManager::getAudioListener()
{
	return _audioListener;
}

void AudioManager::setGain(
	float value)
{
	_gain = glm::clamp(value, 0.f, 1.f);
	_audioListener.setGain(_muted ? 0.f : _gain);
}

float AudioManager::getGain() const
{
	return _gain;
}

void AudioManager::setMuted(
	bool value)
{
	_muted = value;

	_audioListener.setGain(_muted ? 0.f : _gain);
}

bool AudioManager::getMuted() const
{
	return _muted;
}
