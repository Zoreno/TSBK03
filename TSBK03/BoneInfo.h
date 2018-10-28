#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct BoneInfo
{
	glm::mat4 boneOffset{};
	glm::mat4 finalTransformation{};
};