#pragma once

#include "VertexArrayObject.h"
#include "VertexBufferObject.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

class TGA;

class TerrainChunk
{
public:
	explicit TerrainChunk(const std::string& filePath, float offsetX, float offsetZ);

	~TerrainChunk();

	void render() const;

	float getHeight(float x, float z) const;

	float getSizeX() const;
	float getSizeZ() const;

	float getOffsetX() const;
	float getOffsetZ() const;
private:

	unsigned int _width;
	unsigned int _height;

	float _offsetX;
	float _offsetZ;

	VertexArrayObject _vao;
	VertexBufferObject _vertices;
	VertexBufferObject _normals;

	std::vector<float> _heights;

	unsigned int _triangleCount;

	float getGridHeight(int x, int z) const;

	glm::vec3 getVector(
		int x1,
		int z1,
		int x2,
		int z2,
		const TGA &tex,
		float divisor);
};