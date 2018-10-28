#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct AABB
{
	float minX;
	float minY;
	float minZ;

	float maxX;
	float maxY;
	float maxZ;

	glm::vec3 getSize() const;
	glm::vec3 getCenter() const;

	glm::vec3 getMinPoint() const;
	glm::vec3 getMaxPoint() const;

	AABB transform(const glm::mat4& matrix) const;

	bool contains(const glm::vec3& point) const;

	bool intersects(const AABB& other, AABB *crossSection = nullptr) const;
};