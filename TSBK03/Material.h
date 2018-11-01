#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// TODO: Build on the existing material engine with textures
// and blending operations to be able to represent any material
// that we can create.

// TODO: Make this a class

struct Material
{
	std::string name;

	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;

	float exponent;
};
