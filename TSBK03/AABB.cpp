#include "AABB.h"

glm::vec3 AABB::getSize() const
{
	return getMaxPoint() - getMinPoint();
}

glm::vec3 AABB::getCenter() const
{
	return (getMaxPoint() + getMinPoint()) / 2.f;
}

glm::vec3 AABB::getMinPoint() const
{
	return glm::vec3{ minX, minY, minZ };
}

glm::vec3 AABB::getMaxPoint() const
{
	return glm::vec3{ maxX, maxY, maxZ };
}

std::vector<glm::vec3> AABB::getPoints() const
{
	std::vector<glm::vec3> ret;

	ret.push_back(glm::vec3{ minX, minY, minZ });
	ret.push_back(glm::vec3{ minX, minY, maxZ });
	ret.push_back(glm::vec3{ minX, maxY, minZ });
	ret.push_back(glm::vec3{ minX, maxY, maxZ });

	ret.push_back(glm::vec3{ maxX, minY, minZ });
	ret.push_back(glm::vec3{ maxX, minY, maxZ });
	ret.push_back(glm::vec3{ maxX, maxY, minZ });
	ret.push_back(glm::vec3{ maxX, maxY, maxZ });

	return ret;
}

AABB AABB::transform(
	const glm::mat4 &matrix) const
{
	glm::vec4 min = glm::vec4{ getMinPoint(), 1.f };
	glm::vec4 max = glm::vec4{ getMaxPoint(), 1.f };

	min = matrix * min;
	max = matrix * max;

	AABB ret;

	ret.minX = min.x;
	ret.minY = min.y;
	ret.minZ = min.z;

	ret.maxX = max.x;
	ret.maxY = max.y;
	ret.maxZ = max.z;

	return ret;
}

bool AABB::contains(
	const glm::vec3 &point) const
{
	return
		point.x >= minX && point.x <= maxX &&
		point.y >= minY && point.y <= maxY &&
		point.z >= minZ && point.z <= maxZ;
}

bool AABB::intersects(
	const AABB &other,
	AABB *crossSection) const
{
	// Check for each side if the minimum of one box is larger than the
	// maximum of the other. Same logic for all sides. In this case the
	// boxes do not intersect.
	if (this->minX > other.maxX || this->maxX < other.minX ||
		this->minY > other.maxY || this->maxY < other.minY ||
		this->minZ > other.maxZ || this->maxZ < other.minZ)
	{
		return false;
	}

	// If the caller has requested the cross-section of the boxes
	if(crossSection != nullptr)
	{
		crossSection->minX = glm::max(this->minX, other.minX);
		crossSection->minY = glm::max(this->minY, other.minY);
		crossSection->minZ = glm::max(this->minZ, other.minZ);

		crossSection->maxX = glm::min(this->maxX, other.maxX);
		crossSection->maxY = glm::min(this->maxY, other.maxY);
		crossSection->maxZ = glm::min(this->maxZ, other.maxZ);
	}

	return true;
}