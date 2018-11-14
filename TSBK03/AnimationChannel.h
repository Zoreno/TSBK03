#pragma once

#include <string>
#include <vector>

#include "AnimationChannelVectorKey.h"
#include "AnimationChannelQuaternionKey.h"

// TODO: Make this a class

struct AnimationChannel
{
	std::string name;

	std::vector<AnimationChannelVectorKey> positionKeys;
	std::vector<AnimationChannelQuaternionKey> rotationKeys;
	std::vector<AnimationChannelVectorKey> scalingKeys;

	unsigned int findPosition(float animationTime);
	unsigned int findRotation(float animationTime);
	unsigned int findScaling(float animationTime);

	glm::vec3 getInterpolatedPosition(float animationTime);
	glm::quat getInterpolatedRotation(float animationTime);
	glm::vec3 getInterpolatedScaling(float animationTime);
};
