#pragma once

#include "VertexArrayObject.h"
#include "VertexBufferObject.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

#include "TerrainChunk.h"

class Terrain
{
public:
	Terrain(const std::vector<std::string>& chunkPaths, const std::vector<glm::vec3>& offsets);

	~Terrain();

	float getHeight(float x, float z) const;

	const std::vector<TerrainChunk *>& getChunks() const;
private:
	std::vector<TerrainChunk *> _chunks;
};