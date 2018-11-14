#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// TODO: Make this a class

struct BoneInfo
{
	glm::mat4 boneOffset{};
	glm::mat4 finalTransformation{};
};