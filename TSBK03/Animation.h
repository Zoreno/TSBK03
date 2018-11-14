#pragma once

#include <string>
#include <vector>

#include "AnimationChannel.h"

// TODO: Make this a class

struct Animation
{
	std::string name;

	float ticksPerSecond;
	float duration;

	std::vector<AnimationChannel> channels;
};
