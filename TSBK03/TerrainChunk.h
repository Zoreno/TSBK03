#pragma once

#include "VertexArrayObject.h"
#include "VertexBufferObject.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include "AABB.h"
#include "Frustum.h"

class TextureFile;
class TGA;


struct QuadTreeNode
{
	int x;
	int z;
	int width;
	int height;
	int minY;
	int maxY;

	// Both in number of quads!
	unsigned int start;
	unsigned int count;

	QuadTreeNode *q0{ nullptr };
	QuadTreeNode *q1{ nullptr };
	QuadTreeNode *q2{ nullptr };
	QuadTreeNode *q3{ nullptr };
};

// TODO: Make this accept all types of texture files.

class TerrainChunk
{
public:
	explicit TerrainChunk(const std::string& filePath, float offsetX, float offsetZ);

	~TerrainChunk();

	void render() const;
	void render(const Frustum& frustum) const;

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

	unsigned int _largestDimension;
	unsigned int _hilbertDimension;

	const unsigned int _leafSize = 32;

	QuadTreeNode *_treeRoot{ nullptr };

	float getGridHeight(int x, int z) const;

	void renderWithFrustum(QuadTreeNode *node, const Frustum& frustum) const;

	glm::vec3 getVector(
		int x1,
		int z1,
		int x2,
		int z2,
		int width,
		int height,
		const TextureFile *tex,
		float divisor);
};