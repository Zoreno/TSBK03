#pragma once

#include <string>
#include <vector>

#include "AnimationChannel.h"

struct Animation
{
	std::string name;

	float ticksPerSecond;
	float duration;

	std::vector<AnimationChannel> channels;
};
