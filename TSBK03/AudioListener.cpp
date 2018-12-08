/**
* @file	AudioListener.cpp
* @Author	Joakim Bertils
* @date	2017-05-09
* @brief	Audio Listener exception
*/

#include "AudioListener.h"

AudioListener::AudioListener()
{

}

AudioListener::~AudioListener()
{

}

ALfloat AudioListener::getGain() const
{
	return _gain;
}

void AudioListener::setGain(ALfloat gain)
{
	this->_gain = gain;

	ALenum error;
	alGetError();
	alListenerf(AL_GAIN, gain);
	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		throw AudioException("Could not set gain value");
	}
}

glm::vec3 AudioListener::getPosition() const
{
	return _position;
}

void AudioListener::setPosition(const glm::vec3& position)
{
	this->_position = position;

	ALenum error;
	alGetError();
	alListener3f(AL_POSITION, position.x, position.y, position.z);

	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		throw AudioException("Could not set position value");
	}
}

glm::vec3 AudioListener::getVelocity() const
{
	return _velocity;
}

void AudioListener::setVelocity(const glm::vec3& velocity)
{
	this->_velocity = velocity;

	ALenum error;
	alGetError();
	alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);

	error = alGetError();
	if (error != AL_NO_ERROR)
	{
		throw AudioException("Could not set velocity value");
	}
}

glm::vec3 AudioListener::getOrientationAt() const
{
	return _orientationAt;
}

void AudioListener::setOrientationAt(const glm::vec3& orientationAt)
{
	this->_orientationAt = orientationAt;

	if (_orientationUp != glm::vec3{})
	{
		ALfloat vector[6] = {
			orientationAt.x, orientationAt.y, orientationAt.z,
			_orientationUp.x, _orientationUp.y, _orientationUp.z,
		};

		ALenum error;
		alGetError();
		updateOrientation(vector);

		error = alGetError();
		if (error != AL_NO_ERROR)
		{
			throw AudioException("Could not set velocity value");
		}
	}
}

glm::vec3 AudioListener::getOrientationUp() const
{
	return _orientationUp;
}

void AudioListener::setOrientationUp(const glm::vec3& orientationUp)
{
	this->_orientationUp = orientationUp;

	if (_orientationAt != glm::vec3{})
	{
		ALfloat vector[6] = {
			_orientationAt.x, _orientationAt.y, _orientationAt.z,
			orientationUp.x, orientationUp.y, orientationUp.z,
		};

		ALenum error;
		alGetError();
		updateOrientation(vector);

		error = alGetError();
		if (error != AL_NO_ERROR)
		{
			throw AudioException("Could not set velocity value");
		}
	}
}

void AudioListener::updateOrientation(ALfloat* vector)
{
	alListenerfv(AL_ORIENTATION, vector);
}