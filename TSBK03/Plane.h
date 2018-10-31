#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Plane
{

	// The parameters for the plane equation

	float a;
	float b;
	float c;
	float d;

	void normalize();

	int classifyPoint(const glm::vec3& point) const;
};