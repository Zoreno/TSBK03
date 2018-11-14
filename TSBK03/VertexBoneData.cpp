#include "VertexBoneData.h"
#include <cassert>

void VertexBoneData::addBoneData(
	unsigned int id,
	float weight)
{
	for(unsigned int i = 0; i < NUM_BONES_PER_VERTEX; ++i)
	{
		if(weights[i] == 0.f)
		{
			IDs[i] = id;
			weights[i] = weight;
			return;
		}
	}
}
