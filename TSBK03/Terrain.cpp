#include "Terrain.h"

#include "TGA.h"

TerrainChunk::TerrainChunk(
	const std::string &filePath,
	float offsetX,
	float offsetZ)
	: _offsetX{ offsetX },
	_offsetZ{ offsetZ },
	_vao{},
	_vertices(VertexBufferObjectTarget::ARRAY_BUFFER),
	_normals(VertexBufferObjectTarget::ARRAY_BUFFER),
	_indices(VertexBufferObjectTarget::ELEMENT_BUFFER)
{
	TGA file{ filePath.c_str() };

	_width = file.getWidth();
	_height = file.getHeight();

	int vertexCount = _width * _height;
	_triangleCount = (_width - 1) * (_height - 1) * 2;

	_heights.resize(_width * _height);

	int bpp = file.hasAlpha() ? 4 : 3;

	GLfloat *vertexArray = static_cast<GLfloat *>(malloc(sizeof(GLfloat) * 3 * vertexCount));
	GLfloat *normalArray = static_cast<GLfloat *>(malloc(sizeof(GLfloat) * 3 * vertexCount));
	GLuint *indexArray = static_cast<GLuint *>(malloc(sizeof(GLuint) * 3 * _triangleCount));

	for (unsigned int x = 0; x < _width; ++x)
	{
		for (unsigned int z = 0; z < _height; ++z)
		{
			unsigned int vertexOffset = x + z * _width;

			float height = static_cast<float>(file.getPixels().at(vertexOffset * bpp));

			_heights[vertexOffset] = height / 10.f;

			vertexArray[3 * vertexOffset + 0] = x;
			vertexArray[3 * vertexOffset + 1] = height / 10.f;
			vertexArray[3 * vertexOffset + 2] = z;

			glm::vec3 normal{ 0.f, 1.f, 0.f };

			// Last row don't need normals
			if (z < _height - 1)
			{
				// Provoking vertex for the first triangle
				if (vertexOffset % 2 == 0)
				{
					glm::vec3 ab = glm::normalize(getVector(x, z, x + 1, z, file));
					glm::vec3 ac = glm::normalize(getVector(x, z, x + 1, z + 1, file));

					normal = glm::cross(ab, ac);
				}
				else
				{
					glm::vec3 ab = glm::normalize(getVector(x + 1, z, x, z + 1, file));
					glm::vec3 ac = glm::normalize(getVector(x + 1, z, x + 1, z + 1, file));

					normal = glm::cross(ab, ac);
				}
			}
			
			normalArray[3 * vertexOffset + 0] = normal.x;
			normalArray[3 * vertexOffset + 1] = normal.y;
			normalArray[3 * vertexOffset + 2] = normal.z;
		}
	}

	for (unsigned int x = 0; x < _width - 1; ++x)
	{
		for (unsigned int z = 0; z < _height - 1; ++z)
		{
			unsigned int indexOffset = (x + z * (_width - 1)) * 6;

			// Triangle 1
			indexArray[indexOffset + 0] = x + z * _width;
			indexArray[indexOffset + 1] = x + (z + 1) * _width;
			indexArray[indexOffset + 2] = (x + 1) + z * _width;

			// Triangle 2
			indexArray[indexOffset + 3] = (x + 1) + z * _width;
			indexArray[indexOffset + 4] = x + (z + 1) * _width;
			indexArray[indexOffset + 5] = (x + 1) + (z + 1) * _width;
		}
	}

	_vao.bind();

	_vertices.bind();
	_vertices.storeData(
		vertexCount * sizeof(GLfloat) * 3,
		vertexArray,
		VertexBufferObjectUsage::STATIC_DRAW);

	_vertices.setupVertexAttribPointer(0, 3, 3 * sizeof(GLfloat), 0);

	_normals.bind();
	_normals.storeData(
		vertexCount * sizeof(GLfloat) * 3,
		normalArray,
		VertexBufferObjectUsage::STATIC_DRAW);

	_normals.setupVertexAttribPointer(1, 3, 3 * sizeof(GLfloat), 0);

	_indices.storeData(
		_triangleCount * sizeof(unsigned int) * 3,
		indexArray,
		VertexBufferObjectUsage::STATIC_DRAW);

	free(vertexArray);
	free(normalArray);
	free(indexArray);
}

TerrainChunk::~TerrainChunk()
{

}

void TerrainChunk::render() const
{
	_vao.bind();
	_vertices.bind();
	_normals.bind();
	_indices.bind();

	glDrawElements(
		GL_TRIANGLES,
		_triangleCount * 3,
		GL_UNSIGNED_INT,
		nullptr);

	_vertices.unbind();
	_normals.unbind();
	_indices.unbind();

	VertexArrayObject::unbind();
}

float TerrainChunk::getHeight(
	float x,
	float z) const
{
	// Round the values to get the coordinates of the containing triangle's
	// corners
	int x_int = static_cast<int>(x);
	int z_int = static_cast<int>(z);

	// Get the remainder of the coordinates.
	float x_rem = x - x_int;
	float z_rem = z - z_int;

	// If we are on the first triangle.
	if (x_rem + z_rem < 1.f)
	{
		float c = getGridHeight(x_int, z_int);

		float a = getGridHeight(x_int + 1, z_int) - c;

		float b = getGridHeight(x_int, z_int + 1) - c;

		return a * x_rem + b * z_rem + c;
	}

	x_int += 1;
	z_int += 1;

	x_rem = -1 + x_rem;
	z_rem = -1 + z_rem;

	float c = getGridHeight(x_int, z_int);
	float a = c - getGridHeight(x_int - 1, z_int);
	float b = c - getGridHeight(x_int, z_int - 1);

	return a * x_rem + b * z_rem + c;
}

float TerrainChunk::getSizeX() const
{
	return static_cast<float>(_width);
}

float TerrainChunk::getSizeZ() const 
{
	return static_cast<float>(_height);
}

float TerrainChunk::getOffsetX() const
{
	return _offsetX;
}

float TerrainChunk::getOffsetZ() const
{
	return _offsetZ;
}

float TerrainChunk::getGridHeight(
	int x,
	int z) const
{
	return _heights[x + z * _width];
}

glm::vec3 TerrainChunk::getVector(
	int x1,
	int z1,
	int x2,
	int z2,
	const TGA &tex)
{
	float y1 = tex.getPixels()[(x1 + z1 * tex.getWidth()) * tex.hasAlpha() ? 4 : 3] / 10.f;
	float y2 = tex.getPixels()[(x2 + z2 * tex.getWidth()) * tex.hasAlpha() ? 4 : 3] / 10.f;

	glm::vec3 vec;

	vec.x = x2 - x1;
	vec.y = y2 - y1;
	vec.z = z2 - z1;

	return vec;
}

Terrain::Terrain(
	const std::vector<std::string> &chunkPaths,
	const std::vector<glm::vec3> &offsets)
{
	for(unsigned int i = 0; i < chunkPaths.size(); ++i)
	{
		_chunks.push_back(new TerrainChunk{ chunkPaths[i], offsets[i].x, offsets[i].z });
	}
}

Terrain::~Terrain()
{
	for(auto it : _chunks)
	{
		delete it;
	}
}

float Terrain::getHeight(
	float x,
	float z) const
{
	float maxHeight = 0;

	for(auto it : _chunks)
	{
		float x1 = x - it->getOffsetX();
		float z1 = z - it->getOffsetZ();

		if(x1 > 0 && x1 < it->getSizeX() &&
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
