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
	_normals(VertexBufferObjectTarget::ARRAY_BUFFER)
{
	TGA file{ filePath.c_str() };

	_width = file.getWidth();
	_height = file.getHeight();

	_triangleCount = (_width - 1) * (_height - 1) * 2;

	_heights.resize(_width * _height);

	int bpp = file.hasAlpha() ? 4 : 3;

	GLfloat *vertexArray = static_cast<GLfloat *>(malloc(sizeof(GLfloat) * 3 * 3 * _triangleCount));
	GLfloat *normalArray = static_cast<GLfloat *>(malloc(sizeof(GLfloat) * 3 * 3 * _triangleCount));

	unsigned int offset = 0;

	unsigned int last_x = 0;

	for (unsigned int x = 0; x < _width - 1; ++x)
	{
		last_x = x;

		for (unsigned int z = 0; z < _height - 1; ++z)
		{
			float divisor = 5.f;

			glm::vec3 normal;

			unsigned int p0 = x + z * _width;
			unsigned int p1 = (x + 1) + z * _width;
			unsigned int p2 = x + (z + 1) * _width;
			unsigned int p3 = (x + 1) + (z + 1) * _width;

			// First Triangle P0-P1-P2

			vertexArray[3 * 6 * offset + 0] = x;
			vertexArray[3 * 6 * offset + 1] = static_cast<float>(file.getPixels().at(p0 * bpp)) / divisor;
			vertexArray[3 * 6 * offset + 2] = z;

			vertexArray[3 * 6 * offset + 3] = x;
			vertexArray[3 * 6 * offset + 4] = static_cast<float>(file.getPixels().at(p2 * bpp)) / divisor;
			vertexArray[3 * 6 * offset + 5] = z + 1;

			vertexArray[3 * 6 * offset + 6] = x + 1;
			vertexArray[3 * 6 * offset + 7] = static_cast<float>(file.getPixels().at(p1 * bpp)) / divisor;
			vertexArray[3 * 6 * offset + 8] = z;

			glm::vec3 ab = glm::normalize(getVector(x, z, x + 1, z, file, divisor));
			glm::vec3 ac = glm::normalize(getVector(x, z, x, z + 1, file, divisor));

			normal = glm::cross(ac, ab);

			normalArray[3 * 6 * offset + 0] = normal.x;
			normalArray[3 * 6 * offset + 1] = normal.y;
			normalArray[3 * 6 * offset + 2] = normal.z;

			normalArray[3 * 6 * offset + 3] = normal.x;
			normalArray[3 * 6 * offset + 4] = normal.y;
			normalArray[3 * 6 * offset + 5] = normal.z;

			normalArray[3 * 6 * offset + 6] = normal.x;
			normalArray[3 * 6 * offset + 7] = normal.y;
			normalArray[3 * 6 * offset + 8] = normal.z;

			// Second Triangle P1-P2-P3

			vertexArray[3 * 6 * offset + 9] = x + 1;
			vertexArray[3 * 6 * offset + 10] = static_cast<float>(file.getPixels().at(p1 * bpp)) / divisor;
			vertexArray[3 * 6 * offset + 11] = z;

			vertexArray[3 * 6 * offset + 12] = x;
			vertexArray[3 * 6 * offset + 13] = static_cast<float>(file.getPixels().at(p2 * bpp)) / divisor;
			vertexArray[3 * 6 * offset + 14] = z + 1;

			vertexArray[3 * 6 * offset + 15] = x + 1;
			vertexArray[3 * 6 * offset + 16] = static_cast<float>(file.getPixels().at(p3 * bpp)) / divisor;
			vertexArray[3 * 6 * offset + 17] = z + 1;


			ab = glm::normalize(getVector(x + 1, z, x, z + 1, file, divisor));
			ac = glm::normalize(getVector(x + 1, z, x + 1, z + 1, file, divisor));

			normal = glm::cross(ab, ac);

			normalArray[3 * 6 * offset + 9] = normal.x;
			normalArray[3 * 6 * offset + 10] = normal.y;
			normalArray[3 * 6 * offset + 11] = normal.z;

			normalArray[3 * 6 * offset + 12] = normal.x;
			normalArray[3 * 6 * offset + 13] = normal.y;
			normalArray[3 * 6 * offset + 14] = normal.z;

			normalArray[3 * 6 * offset + 15] = normal.x;
			normalArray[3 * 6 * offset + 16] = normal.y;
			normalArray[3 * 6 * offset + 17] = normal.z;

			offset += 1;

			_heights[p0] = static_cast<float>(file.getPixels().at(p0 * bpp)) / divisor;
			_heights[p1] = static_cast<float>(file.getPixels().at(p1 * bpp)) / divisor;
			_heights[p2] = static_cast<float>(file.getPixels().at(p2 * bpp)) / divisor;
			_heights[p3] = static_cast<float>(file.getPixels().at(p3 * bpp)) / divisor;
		}
	}

	_vao.bind();

	_vertices.bind();
	_vertices.storeData(
		_triangleCount * sizeof(GLfloat) * 3 * 3,
		vertexArray,
		VertexBufferObjectUsage::STATIC_DRAW);

	_vertices.setupVertexAttribPointer(0, 3, 3 * sizeof(GLfloat), 0);

	_normals.bind();
	_normals.storeData(
		_triangleCount * sizeof(GLfloat) * 3 * 3,
		normalArray,
		VertexBufferObjectUsage::STATIC_DRAW);

	_normals.setupVertexAttribPointer(1, 3, 3 * sizeof(GLfloat), 0);

	free(vertexArray);
	free(normalArray);
}

TerrainChunk::~TerrainChunk()
{

}

void TerrainChunk::render() const
{
	_vao.bind();
	_vertices.bind();
	_normals.bind();

	glDrawArrays(
		GL_TRIANGLES,
		0,
		_triangleCount * 3);

	_vertices.unbind();
	_normals.unbind();

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
	const TGA &tex,
	float divisor)
{
	float y1 = tex.getPixels()[(x1 + z1 * tex.getWidth()) * (tex.hasAlpha() ? 4 : 3)] / divisor;
	float y2 = tex.getPixels()[(x2 + z2 * tex.getWidth()) * (tex.hasAlpha() ? 4 : 3)] / divisor;

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
