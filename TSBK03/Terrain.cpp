#include "Terrain.h"

#include "TGA.h"

Terrain::Terrain(
	const std::vector<std::string> &chunkPaths,
	const std::vector<glm::vec3> &offsets)
{
	for (unsigned int i = 0; i < chunkPaths.size(); ++i)
	{
		_chunks.push_back(new TerrainChunk{ chunkPaths[i], offsets[i].x, offsets[i].z });
	}
}

Terrain::~Terrain()
{
	for (auto it : _chunks)
	{
		delete it;
	}
}

float Terrain::getHeight(
	float x,
	float z) const
{
	float maxHeight = 0;

	for (auto it : _chunks)
	{
		float x1 = x - it->getOffsetX();
		float z1 = z - it->getOffsetZ();

		if (x1 > 0 && x1 < it->getSizeX() &&
			z1 > 0 && z1 < it->getSizeZ())
		{
			maxHeight = glm::max(maxHeight, it->getHeight(x1, z1));
		}
	}

	return maxHeight;
}

const std::vector<TerrainChunk *> & Terrain::getChunks() const
{
	return _chunks;
}
