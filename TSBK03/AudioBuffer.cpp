/**
* @file	AudioBuffer.cpp
* @Author	Joakim Bertils
* @date	2017-02-18
* @brief	OpenAL static audio buffer abstraction.
*/

#include "AudioBuffer.h"

#include <AL/alc.h>

#include "WaveFile.h"

AudioBuffer::AudioBuffer(const std::string& path)
	: _path{ path }
{
	ALenum error;

	// Dummy read
	alGetError();

	alGenBuffers(1, &_handle);

	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		throw AudioException("Could not generate buffer.");
	}

	WaveFile file{ path };

	alBufferData(_handle, file.getFormat(), file.getData(), file.getSize(), file.getFrequency());

	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		throw AudioException("Could not fill buffer data.");
	}
}

AudioBuffer::~AudioBuffer()
{
	if (alIsBuffer(_handle))
	{
		alDeleteBuffers(1, &_handle);
	}
}

ALuint AudioBuffer::getHandle() const
{
	return _handle;
}

const std::string& AudioBuffer::getPath() const
{
	return _path;
}