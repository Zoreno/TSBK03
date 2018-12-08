#pragma once

/**
* @file	AudioSource.h
* @Author	Joakim Bertils
* @date	2017-02-18
* @brief	OpenAL audio source abstraction.
*/

#pragma once

#include <string>

#include <AL/al.h>
#include <AL/alc.h>

#include <glm/glm.hpp>

#include "AudioBuffer.h"
#include "AudioException.h"

/**
* @brief Class representing an OpenAL audio source
*/
class AudioSource
{
public:
	/**
	* @brief Constructor
	*
	* Creates a new audio source.
	*
	* @param path Path to wave file containing data.
	* @param looping True if sound should be looped.
	*/
	explicit AudioSource(const std::string& path, ALboolean looping = false);

	/**
	* @brief Destructor
	*
	* Deletes the audio source and the data associated with it.
	*/
	~AudioSource();

	/**
	* @brief Plays the audio source data.
	*/
	void play();

	/**
	* @brief Stops the audio source data.
	*/
	void stop();

	/**
	* @brief Pauses the audio source data.
	*/
	void pause();

	/**
	* @brief Rewinds the audio source data.
	*/
	void rewind();

	/**
	* @brief Gets the openAL audio source handle.
	* @return Handle to source.
	*/
	ALuint getHandle() const;

	/**
	* @brief Gets the pitch value.
	* @return Pitch value.
	*/
	ALfloat getPitch() const;

	/**
	* @brief Sets a new value for the pitch.
	* @param pitch New pitch value.
	*/
	void setPitch(ALfloat pitch);

	/**
	* @brief Gets the gain value.
	* @return Gain value.
	*/
	ALfloat getGain() const;

	/**
	* @brief Sets a new value for the gain.
	* @param gain New gain value.
	*/
	void setGain(ALfloat gain);

	/**
	* @brief Gets the audio source position.
	* @return Position.
	*/
	glm::vec3 getPosition() const;

	/**
	* @brief Sets a new value for the position.
	* @param position 3D vector containing position data.
	*/
	void setPosition(const glm::vec3& position);

	/**
	* @brief Gets the audio source velocity.
	* @return Velocity.
	*/
	glm::vec3 getVelocity() const;

	/**
	* @brief Sets a new value for the velocity.
	* @param velocity 3D vector containing velocity data.
	*/
	void setVelocity(const glm::vec3& velocity);

	/**
	* @brief Gets the direction the audio source is facing.
	* @return Direction.
	*/
	glm::vec3 getDirection() const;

	/**
	* @brief Sets a new value for the direction.
	* @param direction 3D vector containing direction data.
	*/
	void setDirection(const glm::vec3& direction);

	/**
	* @brief Gets the looping flag.
	* @return Looping flag.
	*/
	ALboolean getLooping() const;

	/**
	* @brief Sets a new value for the looping flag.
	* @param looping New value for the looping flag.
	*/
	void setLooping(ALboolean looping);
private:

	/**
	* @brief Pitch of the audio source.
	*
	* Doubling this value will increase the pitch by one octave.
	*/
	ALfloat _pitch{ 1.f };

	/**
	* @brief Gain of the audio source.
	*
	* Volume of the source. Valid values are 0.f to 1.f inclusive.
	*/
	ALfloat _gain{ 1.f };

	/**
	* @brief Position of the audio source.
	*/
	glm::vec3 _position{};

	/**
	* @brief Velocity of the audio source.
	*/
	glm::vec3 _velocity{};

	/**
	* @brief Direction of the audio source.
	*/
	glm::vec3 _direction{};

	/**
	* @brief If true, the audio should be looped.
	*/
	ALboolean _looping{ false };

	/**
	* @brief Buffer containing audio data.
	*/
	AudioBuffer _buffer;

	/**
	* @brief OpenAL audio source handle.
	*/
	ALuint _handle;
};