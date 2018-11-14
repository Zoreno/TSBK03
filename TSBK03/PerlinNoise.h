#pragma once

#include <vector>

class PerlinNoise
{
public:
	PerlinNoise();

	explicit PerlinNoise(unsigned int seed);

	float noise(float x, float y, float z);
private:

	float fade(float t);
	float lerp(float t, float a, float b);
	float grad(int hash, float x, float y, float z);

	std::vector<int> _p;
};