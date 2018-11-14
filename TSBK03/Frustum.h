#pragma once

#include "Plane.h"
#include <vector>

struct Frustum
{
	explicit Frustum(const glm::mat4& matrix, bool normalize = true);

	Plane planes[6];

	int boxIntersect(const std::vector<glm::vec3>& points) const;
};
