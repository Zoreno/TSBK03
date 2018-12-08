/**
* @file	AudioListener.h
* @Author	Joakim Bertils
* @date	2017-05-09
* @brief	Audio Listener object
*/

#pragma once

#include <AL/al.h>
#include <glm/glm.hpp>

#include "AudioException.h"

/**
* @brief Audio Listener object
*/
class AudioListener
{
public:
	/**
	* @brief Constructor
	*/
	AudioListener();

	/**
	* @brief Destructor
	*/
	~AudioListener();

	/**
	* @brief Constructor
	*/
	AudioListener(const AudioListener&) = delete;

	/**
	* @brief Constructor
	*/
	AudioListener(AudioListener&&) = delete;

	/**
	* @brief Assignment operator
	* @return Ref. to self
	*/
	const AudioListener& operator=(const AudioListener&) = delete;

	/**
	* @brief Assignment operator.
	* @return Ref. to self.
	*/
	AudioListener& operator=(AudioListener&&) = delete;

	/**
	* @brief Getter for gain.
	* @return Gain
	*/
	ALfloat getGain() const;

	/**
	* @brief Setter for gain
	* @param gain New gain.
	*/
	void setGain(ALfloat gain);

	/**
	* @brief Getter for position
	* @return Position
	*/
	glm::vec3 getPosition() const;

	/**
	* @brief Setter for position
	* @param position New position
	*/
	void setPosition(const glm::vec3& position);

	/**
	* @brief Getter for velocity
	* @return Velocity
	*/
	glm::vec3 getVelocity() const;

	/**
	* @brief Setter for velocity
	* @param velocity New velocity
	*/
	void setVelocity(const glm::vec3& velocity);

	/**
	* @brief Getter for orientation at vector.
	* @return Orientation at.
	*/
	glm::vec3 getOrientationAt() const;

	/**
	* @brief Setter for orientation at vector
	* @param orientationAt New orientation at
	*/
	void setOrientationAt(const glm::vec3& orientationAt);

	/**
	* @brief Getter for orientation up vector.
	* @return Orientation up.
	*/
	glm::vec3 getOrientationUp() const;

	/**
	* @brief Setter for orientation up vector.
	* @param orientationUp New orientation up
	*/
	void setOrientationUp(const glm::vec3& orientationUp);

private:

	/**
	* @brief Update orientation vector
	* @param vector New orientation vector.
	*/
	void updateOrientation(ALfloat* vector);

	/**
	* @brief Gain of audio listener
	*/
	ALfloat _gain{ 1.f };

	/**
	* @brief Position of audio listener
	*/
	glm::vec3 _position{};

	/**
	* @brief Velocity of audio listener
	*/
	glm::vec3 _velocity{};

	/**
	* @brief The listener's forward direcion
	*/
	glm::vec3 _orientationAt{ 0.f, 0.f, -1.f };

	/**
	* @brief The listener's up direction
	*/
	glm::vec3 _orientationUp{ 0.f, 1.f, 0.f };
};