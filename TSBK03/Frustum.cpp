#include "Frustum.h"

Frustum::Frustum(
	const glm::mat4 &matrix,
	bool normalize)
{
	// http://www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf
#if 0
	// Left Plane
	planes[0].a = matrix[3][0] + matrix[0][0];
	planes[0].b = matrix[3][1] + matrix[0][1];
	planes[0].c = matrix[3][2] + matrix[0][2];
	planes[0].d = matrix[3][3] + matrix[0][3];

	// Right Plane
	planes[1].a = matrix[3][0] - matrix[0][0];
	planes[1].b = matrix[3][1] - matrix[0][1];
	planes[1].c = matrix[3][2] - matrix[0][2];
	planes[1].d = matrix[3][3] - matrix[0][3];

	// Top Plane
	planes[2].a = matrix[3][0] - matrix[1][0];
	planes[2].b = matrix[3][1] - matrix[1][1];
	planes[2].c = matrix[3][2] - matrix[1][2];
	planes[2].d = matrix[3][3] - matrix[1][3];

	// Bottom Plane
	planes[3].a = matrix[3][0] + matrix[1][0];
	planes[3].b = matrix[3][1] + matrix[1][1];
	planes[3].c = matrix[3][2] + matrix[1][2];
	planes[3].d = matrix[3][3] + matrix[1][3];

	// Near Plane
	planes[4].a = matrix[3][0] + matrix[2][0];
	planes[4].b = matrix[3][1] + matrix[2][1];
	planes[4].c = matrix[3][2] + matrix[2][2];
	planes[4].d = matrix[3][3] + matrix[2][3];

	// Far Plane
	planes[5].a = matrix[3][0] - matrix[2][0];
	planes[5].b = matrix[3][1] - matrix[2][1];
	planes[5].c = matrix[3][2] - matrix[2][2];
	planes[5].d = matrix[3][3] - matrix[2][3];

#else
	// Left Plane
	planes[0].a = matrix[0][3] + matrix[0][0];
	planes[0].b = matrix[1][3] + matrix[1][0];
	planes[0].c = matrix[2][3] + matrix[2][0];
	planes[0].d = matrix[3][3] + matrix[3][0];

	// Right Plane
	planes[1].a = matrix[0][3] - matrix[0][0];
	planes[1].b = matrix[1][3] - matrix[1][0];
	planes[1].c = matrix[2][3] - matrix[2][0];
	planes[1].d = matrix[3][3] - matrix[3][0];

	// Top Plane
	planes[2].a = matrix[0][3] - matrix[0][1];
	planes[2].b = matrix[1][3] - matrix[1][1];
	planes[2].c = matrix[2][3] - matrix[2][1];
	planes[2].d = matrix[3][3] - matrix[3][1];

	// Bottom Plane
	planes[3].a = matrix[0][3] + matrix[0][1];
	planes[3].b = matrix[1][3] + matrix[1][1];
	planes[3].c = matrix[2][3] + matrix[2][1];
	planes[3].d = matrix[3][3] + matrix[3][1];

	// Near Plane
	planes[4].a = matrix[0][3] + matrix[0][2];
	planes[4].b = matrix[1][3] + matrix[1][2];
	planes[4].c = matrix[2][3] + matrix[2][2];
	planes[4].d = matrix[3][3] + matrix[3][2];

	// Far Plane
	planes[5].a = matrix[0][3] - matrix[0][2];
	planes[5].b = matrix[1][3] - matrix[1][2];
	planes[5].c = matrix[2][3] - matrix[2][2];
	planes[5].d = matrix[3][3] - matrix[3][2];
#endif

	if(normalize)
	{
		for(auto& it : planes)
		{
			it.normalize();
		}
	}
}

int Frustum::boxIntersect(
	const std::vector<glm::vec3> &points) const
{
	int res = 1;

	for(auto plane : planes)
	{
		int in = 0;
		int out = 0;

		for(int i = 0; i < 8 && (in == 0 || out == 0); ++i)
		{
			if(plane.classifyPoint(points.at(i)) < 0)
			{
				++out;
			}
			else
			{
				++in;
			}
		}

		if(!in)
		{
			return -1;
		}
		if(out)
		{
			res = 0;
		}
	}

	return res;
}
