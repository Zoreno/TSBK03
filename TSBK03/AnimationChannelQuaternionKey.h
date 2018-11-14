#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// TODO: Make this a class

struct AnimationChannelQuaternionKey
{
	glm::quat value;
	float time;
};