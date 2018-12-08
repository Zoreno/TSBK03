/**
* @file	AudioSource.cpp
* @Author	Joakim Bertils
* @date	2017-02-20
* @brief	OpenAL audio source abstraction.
*/

#include "AudioSource.h"

AudioSource::AudioSource(const std::string& path, ALboolean looping)
	: _buffer{ path }
{
	ALenum error;

	// Dummy read
	alGetError();

	alGenSources(1, &_handle);
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		throw AudioException("Could not generate buffer.");
	}

	// Attach the buffer to this source.
	alSourcei(_handle, AL_BUFFER, _buffer.getHandle());
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		throw AudioException("Could not attach buffer to source");
	}

	setLooping(looping);
}

AudioSource::~AudioSource()
{
	if (alIsSource(_handle))
	{
		alDeleteSources(1, &_handle);
	}
}

void AudioSource::play()
{
	alSourcePlay(_handle);
}

void AudioSource::stop()
{
	alSourceStop(_handle);
}

void AudioSource::pause()
{
	alSourcePause(_handle);
}

void AudioSource::rewind()
{
	alSourceRewind(_handle);
}

ALuint AudioSource::getHandle() const
{
	return _handle;
}

ALfloat AudioSource::getPitch() const
{
	return _pitch;
}

void AudioSource::setPitch(ALfloat pitch)
{
	this->_pitch = pitch;

	ALenum error;
	alGetError();
	alSourcef(_handle, AL_PITCH, pitch);
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		throw AudioException("Could not set pitch value");
	}
}

ALfloat AudioSource::getGain() const
{
	return _gain;
}

void AudioSource::setGain(ALfloat gain)
{
	this->_gain = gain;

	ALenum error;
	alGetError();
	alSourcef(_handle, AL_GAIN, gain);
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		throw AudioException("Could not set gain value");
	}
}

glm::vec3 AudioSource::getPosition() const
{
	return _position;
}

void AudioSource::setPosition(const glm::vec3& position)
{
	this->_position = position;

	ALenum error;
	alGetError();
	alSource3f(_handle, AL_POSITION, position.x, position.y, position.z);

	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		throw AudioException("Could not set position value");
	}
}

glm::vec3 AudioSource::getVelocity() const
{
	return _velocity;
}

void AudioSource::setVelocity(const glm::vec3& velocity)
{
	this->_velocity = velocity;

	ALenum error;
	alGetError();
	alSource3f(_handle, AL_VELOCITY, velocity.x, velocity.y, velocity.z);

	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		throw AudioException("Could not set velocity value");
	}
}

glm::vec3 AudioSource::getDirection() const
{
	return _direction;
}

void AudioSource::setDirection(const glm::vec3& direction)
{
	this->_direction = direction;

	ALenum error;
	alGetError();
	alSource3f(_handle, AL_DIRECTION, direction.x, direction.y, direction.z);

	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		throw AudioException("Could not set direction value");
	}
}

ALboolean AudioSource::getLooping() const
{
	return _looping;
}

void AudioSource::setLooping(ALboolean looping)
{
	this->_looping = looping;

	ALenum error;
	alGetError();
	alSourcei(_handle, AL_LOOPING, looping);

	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		throw AudioException("Could not set looping value");
	}
}