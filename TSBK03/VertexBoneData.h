#pragma once

#define NUM_BONES_PER_VERTEX 4

// TODO: Make this a class

struct VertexBoneData
{
	unsigned int IDs[NUM_BONES_PER_VERTEX];
	float weights[NUM_BONES_PER_VERTEX];

	void addBoneData(unsigned int id, float weight);
};