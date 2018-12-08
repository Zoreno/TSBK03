#pragma once

#include <string>
#include <AL/al.h>
#include "AudioException.h"

class AudioBuffer
{
public:

	explicit AudioBuffer(const std::string& file);
	~AudioBuffer();

	AudioBuffer(const AudioBuffer& other) = delete;
	AudioBuffer(AudioBuffer&& other) = delete;
	AudioBuffer& operator=(const AudioBuffer& other) = delete;
	AudioBuffer& operator=(AudioBuffer&& other) = delete;

	ALuint getHandle() const;
	const std::string& getPath() const;

private:
	std::string _path{};
	ALuint _handle;
};