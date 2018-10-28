#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct ModelNode
{
	std::string name;
	unsigned int id;
	unsigned int parent;
	std::vector<unsigned int> children;

	glm::mat4 transform;
};
