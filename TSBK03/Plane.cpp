#include "Plane.h"

// http://www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf

void Plane::normalize()
{
	float magnitude;

	magnitude = glm::sqrt(a * a + b * b + c * c);

	a /= magnitude;
	b /= magnitude;
	c /= magnitude;
	d /= magnitude;
}

int Plane::classifyPoint(
	const glm::vec3 &point) const
{
	float distance;

	distance = a * point.x + b * point.y + c * point.z + d;

	if (distance < 0)
	{
		return -1;
	}

	if (distance > 0)
	{
		return 1;
	}

	return 0;
}