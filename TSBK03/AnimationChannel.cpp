#include "AnimationChannel.h"
#include <iostream>

unsigned AnimationChannel::findPosition(
	float animationTime)
{
	for (unsigned int i = 0; i < positionKeys.size() - 1; ++i)
	{
		if (animationTime < positionKeys[i + 1].time)
		{
			return i;
		}
	}

	assert(0);

	return 0;
}

unsigned AnimationChannel::findRotation(
	float animationTime)
{
	for (unsigned int i = 0; i < rotationKeys.size() - 1; ++i)
	{
		if (animationTime < rotationKeys[i + 1].time)
		{
			return i;
		}
	}

	assert(0);

	return 0;
}

unsigned AnimationChannel::findScaling(
	float animationTime)
{
	for (unsigned int i = 0; i < scalingKeys.size() - 1; ++i)
	{
		if (animationTime < scalingKeys[i + 1].time)
		{
			return i;
		}
	}

	assert(0);

	return 0;
}

glm::vec3 AnimationChannel::getInterpolatedPosition(
	float animationTime)
{
	// If we onlý have one value, we can't interpolate
	if (positionKeys.size() == 1)
	{
		return positionKeys[0].value;
	}

	unsigned int positionIndex = findPosition(animationTime);
	unsigned int nextPositionIndex = positionIndex + 1;
	assert(nextPositionIndex < positionKeys.size());

	float deltaTime = positionKeys[nextPositionIndex].time - positionKeys[positionIndex].time;
	float factor = (animationTime - positionKeys[positionIndex].time) / deltaTime;

	if (factor < 0.f)
	{
		factor = 0.f;
	}

	assert(factor >= 0.0f && factor <= 1.0f);

	glm::vec3 start = positionKeys[positionIndex].value;
	glm::vec3 end = positionKeys[nextPositionIndex].value;

	glm::vec3 delta = end - start;

	return start + factor * delta;
}

glm::quat AnimationChannel::getInterpolatedRotation(
	float animationTime)
{
	// If we onlý have one value, we can't interpolate
	if (rotationKeys.size() == 1)
	{
		return rotationKeys[0].value;
	}

	unsigned int rotationIndex = findRotation(animationTime);
	unsigned int nextRotationIndex = rotationIndex + 1;
	assert(nextRotationIndex < rotationKeys.size());

	float deltaTime = rotationKeys[nextRotationIndex].time - rotationKeys[rotationIndex].time;
	float factor = (animationTime - rotationKeys[rotationIndex].time) / deltaTime;

	if (factor < 0.f)
	{
		factor = 0.f;
	}

	assert(factor >= 0.0f && factor <= 1.0f);

	glm::quat start = rotationKeys[rotationIndex].value;
	glm::quat end = rotationKeys[nextRotationIndex].value;

	glm::quat res = glm::slerp(start, end, factor);

	return normalize(res);
}

glm::vec3 AnimationChannel::getInterpolatedScaling(
	float animationTime)
{
	// If we onlý have one value, we can't interpolate
	if (scalingKeys.size() == 1)
	{
		return scalingKeys[0].value;
	}

	unsigned int scalingIndex = findScaling(animationTime);
	unsigned int nextScalingIndex = scalingIndex + 1;
	assert(nextScalingIndex < scalingKeys.size());

	float deltaTime = scalingKeys[nextScalingIndex].time - scalingKeys[scalingIndex].time;
	float factor = (animationTime - scalingKeys[scalingIndex].time) / deltaTime;

	if(factor < 0.f)
	{
		factor = 0.f;
	}

	assert(factor >= 0.0f && factor <= 1.0f);

	glm::vec3 start = scalingKeys[scalingIndex].value;
	glm::vec3 end = scalingKeys[nextScalingIndex].value;

	glm::vec3 delta = end - start;

	return start + factor * delta;
}