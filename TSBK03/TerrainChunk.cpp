#include "TerrainChunk.h"

#include "TGA.h"
#include <iostream>
#include <vector>
#include <cstdlib>

#include "Triangle.h"
#include "HilbertCurve.h"
#include "STBTextureFile.h"

static QuadTreeNode *constructQuadTreeNode(
	int x,
	int z,
	int width,
	int height,
	unsigned int hilbertDimension,
	unsigned int leafSize,
	unsigned int currentSize,
	const std::vector<float>& minHeights,
	const std::vector<float>& maxHeights)
{
	// Allocate memory for a new node
	QuadTreeNode *node = new QuadTreeNode{};

	// Constuct the world space position of the node and the world space size
	// of the node.
	node->x = x;
	node->z = z;
	node->width = width;
	node->height = height;

	// Get the position in the grid (with the leaf size as unit size.
	int x_grid = x / currentSize;
	int z_grid = z / currentSize;

	// Get the offset into the hilbert curve associated with this node.
	int d = hilbert_xy2d(hilbertDimension, x_grid, z_grid);

	// Calculate the size of the blocks in number of quads.
	unsigned int blockSize = currentSize * currentSize;

	// Assign the offset into the vertex buffer and the number of entries in
	// the buffer for this node.
	node->start = d * blockSize;
	node->count = blockSize;

	/*size_t count = 9 - glm::log2(static_cast<float>(currentSize));
*/
	std::string str{};

	/*
	for (unsigned int i = 0; i < count; ++i)
	{
		str += ' ';
	}
*/

//std::cout << str << "x: " << x;
//std::cout << str << " z: " << z;
//std::cout << str << " width: " << width;
//std::cout << str << " height: " << height;
//std::cout << str << " x grid: " << x_grid;
//std::cout << str << " z grid: " << z_grid;
//std::cout << str << " d: " << d;
//std::cout << str << " start: " << node->start;
//std::cout << str << " count: " << node->count;
//std::cout << str << std::endl;

	// Recursively construct the sub-nodes for this node.
	if (currentSize != leafSize)
	{
		// Construct the node for the first quadrants.
		node->q0 = constructQuadTreeNode(
			x,
			z,
			width / 2,
			height / 2,
			hilbertDimension * 2,
			leafSize,
			currentSize / 2,
			minHeights,
			maxHeights);

		// Construct the node for the second quadrants.
		node->q1 = constructQuadTreeNode(
			x + (width / 2),
			z,
			width / 2,
			height / 2,
			hilbertDimension * 2,
			leafSize,
			currentSize / 2,
			minHeights,
			maxHeights);

		// Construct the node for the third quadrants.
		node->q2 = constructQuadTreeNode(
			x,
			z + (height / 2),
			width / 2,
			height / 2,
			hilbertDimension * 2,
			leafSize,
			currentSize / 2,
			minHeights,
			maxHeights);

		// Construct the node for the fourth quadrants.
		node->q3 = constructQuadTreeNode(
			x + (width / 2),
			z + (height / 2),
			width / 2,
			height / 2,
			hilbertDimension * 2,
			leafSize,
			currentSize / 2,
			minHeights,
			maxHeights);

		// Update the min and max Y values from the min and the max values from
		// the child nodes.
		node->minY =
			glm::min(
				glm::min(
					node->q0->minY,
					node->q1->minY),
				glm::min(
					node->q2->minY,
					node->q3->minY));

		node->maxY =
			glm::max(
				glm::max(
					node->q0->maxY,
					node->q1->maxY),
				glm::max(
					node->q2->maxY,
					node->q3->maxY));
	}
	else
	{
		// If this is a leaf node, we can get the min and max Y values from
		// the buffers we passed into the constructor.
		node->minY = minHeights.at(d);
		node->maxY = maxHeights.at(d);
	}

	// Return the address of the node we constructed.
	return node;
}

void destroyQuadTree(QuadTreeNode *node)
{
	if (node->q0 != nullptr)
	{
		destroyQuadTree(node->q0);
		destroyQuadTree(node->q1);
		destroyQuadTree(node->q2);
		destroyQuadTree(node->q3);
	}

	delete node;
}

static std::vector<glm::vec3> getPoints(QuadTreeNode *node)
{
	std::vector<glm::vec3> ret;

	ret.push_back(glm::vec3{ node->x, node->minY, node->z });
	ret.push_back(glm::vec3{ node->x + node->width, node->minY, node->z });
	ret.push_back(glm::vec3{ node->x, node->minY, node->z + node->height });
	ret.push_back(glm::vec3{ node->x + node->width, node->minY, node->z + node->height });

	ret.push_back(glm::vec3{ node->x, node->maxY, node->z });
	ret.push_back(glm::vec3{ node->x + node->width, node->maxY, node->z });
	ret.push_back(glm::vec3{ node->x, node->maxY, node->z + node->height });
	ret.push_back(glm::vec3{ node->x + node->width, node->maxY, node->z + node->height });

	return ret;
}

static float sampleHeightmap(TextureFile *file, int x, int z, int width, int height, int bpp)
{
	if (x >= width || z >= height)
	{
		return 0.f;
	}

	return static_cast<float>(file->getPixels().at((x + z * width)* bpp));
}

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
#if USE_OWN_IMAGE_LOADER
	TGA *file{ new TGA{filePath.c_str()} };
#else
	TextureFile *file = new STBTextureFile{ filePath };
#endif
	_width = file->getWidth();
	_height = file->getHeight();

	int bpp = file->hasAlpha() ? 4 : 3;

	_largestDimension = glm::max(_width, _height);

	unsigned int pow2 = 1;
	while (pow2 < _largestDimension)
	{
		pow2 *= 2;
	}

	_largestDimension = pow2;

	_hilbertDimension = (_largestDimension + _leafSize - 1) / _leafSize;

	_triangleCount = _hilbertDimension * _hilbertDimension * _leafSize * _leafSize * 2;

	_heights.resize(_largestDimension * _largestDimension);

	std::vector<Triangle> vertexVector;
	std::vector<Triangle> normalVector;
	vertexVector.resize(_triangleCount);
	normalVector.resize(_triangleCount);

	std::vector<float> minHeights;
	std::vector<float> maxHeights;

	minHeights.resize(_hilbertDimension * _hilbertDimension);
	maxHeights.resize(_hilbertDimension * _hilbertDimension);

	for (unsigned int i = 0; i < _hilbertDimension * _hilbertDimension; ++i)
	{
		int x_offset;
		int z_offset;

		std::cout << "Processing chunk " << i << " of " << _hilbertDimension * _hilbertDimension << std::endl;

		hilbert_d2xy(_hilbertDimension, i, &x_offset, &z_offset);

		x_offset *= _leafSize;
		z_offset *= _leafSize;

		float minHeight = 256.f;
		float maxHeight = 0.f;

		for (unsigned int z = z_offset; z < z_offset + _leafSize; ++z)
		{
			for (unsigned int x = x_offset; x < x_offset + _leafSize; ++x)
			{

				unsigned int offset = 2 * (i * _leafSize * _leafSize + (z - z_offset) * _leafSize + (x - x_offset));

				float divisor = 5.f;

				glm::vec3 normal;

				/*
				 * P0 - - P1
				 * |    /  |
				 * |  /    |
				 * P2 - - P3
				 */

				 // First Triangle P0-P2-P1

				vertexVector.at(offset).p0.x = x;
				vertexVector.at(offset).p0.y = sampleHeightmap(file, x, z, _width, _height, bpp) / divisor;
				vertexVector.at(offset).p0.z = z;

				vertexVector.at(offset).p1.x = x;
				vertexVector.at(offset).p1.y = sampleHeightmap(file, x, z + 1, _width, _height, bpp) / divisor;
				vertexVector.at(offset).p1.z = z + 1;

				vertexVector.at(offset).p2.x = x + 1;
				vertexVector.at(offset).p2.y = sampleHeightmap(file, x + 1, z, _width, _height, bpp) / divisor;
				vertexVector.at(offset).p2.z = z;

				minHeight = glm::min(minHeight, vertexVector.at(offset).p0.y);
				minHeight = glm::min(minHeight, vertexVector.at(offset).p1.y);
				minHeight = glm::min(minHeight, vertexVector.at(offset).p2.y);

				maxHeight = glm::max(maxHeight, vertexVector.at(offset).p0.y);
				maxHeight = glm::max(maxHeight, vertexVector.at(offset).p1.y);
				maxHeight = glm::max(maxHeight, vertexVector.at(offset).p2.y);

				/*
				vertexArray[offset + 0] = x;
				vertexArray[offset + 1] = static_cast<float>(file->getPixels().at(p0 * bpp)) / divisor;
				vertexArray[offset + 2] = z;

				vertexArray[offset + 3] = x;
				vertexArray[offset + 4] = static_cast<float>(file->getPixels().at(p2 * bpp)) / divisor;
				vertexArray[offset + 5] = z + 1;

				vertexArray[offset + 6] = x + 1;
				vertexArray[offset + 7] = static_cast<float>(file->getPixels().at(p1 * bpp)) / divisor;
				vertexArray[offset + 8] = z;
				*/
				glm::vec3 ab = glm::normalize(getVector(x, z, x + 1, z, _width, _height, file, divisor));
				glm::vec3 ac = glm::normalize(getVector(x, z, x, z + 1, _width, _height, file, divisor));

				normal = glm::cross(ac, ab);

				normalVector.at(offset).p0.x = normal.x;
				normalVector.at(offset).p0.y = normal.y;
				normalVector.at(offset).p0.z = normal.z;

				normalVector.at(offset).p1.x = normal.x;
				normalVector.at(offset).p1.y = normal.y;
				normalVector.at(offset).p1.z = normal.z;

				normalVector.at(offset).p2.x = normal.x;
				normalVector.at(offset).p2.y = normal.y;
				normalVector.at(offset).p2.z = normal.z;
				/*
				normalArray[offset + 0] = normal.x;
				normalArray[offset + 1] = normal.y;
				normalArray[offset + 2] = normal.z;

				normalArray[offset + 3] = normal.x;
				normalArray[offset + 4] = normal.y;
				normalArray[offset + 5] = normal.z;

				normalArray[offset + 6] = normal.x;
				normalArray[offset + 7] = normal.y;
				normalArray[offset + 8] = normal.z;
				*/
				// Second Triangle P1-P2-P3

				vertexVector.at(offset + 1).p0.x = x + 1;
				vertexVector.at(offset + 1).p0.y = sampleHeightmap(file, x + 1, z, _width, _height, bpp) / divisor;
				vertexVector.at(offset + 1).p0.z = z;

				vertexVector.at(offset + 1).p1.x = x;
				vertexVector.at(offset + 1).p1.y = sampleHeightmap(file, x, z + 1, _width, _height, bpp) / divisor;
				vertexVector.at(offset + 1).p1.z = z + 1;

				vertexVector.at(offset + 1).p2.x = x + 1;
				vertexVector.at(offset + 1).p2.y = sampleHeightmap(file, x + 1, z + 1, _width, _height, bpp) / divisor;
				vertexVector.at(offset + 1).p2.z = z + 1;

				minHeight = glm::min(minHeight, vertexVector.at(offset + 1).p0.y);
				minHeight = glm::min(minHeight, vertexVector.at(offset + 1).p1.y);
				minHeight = glm::min(minHeight, vertexVector.at(offset + 1).p2.y);

				maxHeight = glm::max(maxHeight, vertexVector.at(offset + 1).p0.y);
				maxHeight = glm::max(maxHeight, vertexVector.at(offset + 1).p1.y);
				maxHeight = glm::max(maxHeight, vertexVector.at(offset + 1).p2.y);

				/*
				vertexArray[offset + 9] = x + 1;
				vertexArray[offset + 10] = static_cast<float>(file->getPixels().at(p1 * bpp)) / divisor;
				vertexArray[offset + 11] = z;

				vertexArray[offset + 12] = x;
				vertexArray[offset + 13] = static_cast<float>(file->getPixels().at(p2 * bpp)) / divisor;
				vertexArray[offset + 14] = z + 1;

				vertexArray[offset + 15] = x + 1;
				vertexArray[offset + 16] = static_cast<float>(file->getPixels().at(p3 * bpp)) / divisor;
				vertexArray[offset + 17] = z + 1;
				*/

				ab = glm::normalize(getVector(x + 1, z, x, z + 1, _width, _height, file, divisor));
				ac = glm::normalize(getVector(x + 1, z, x + 1, z + 1, _width, _height, file, divisor));

				normal = glm::cross(ab, ac);

				normalVector.at(offset + 1).p0.x = normal.x;
				normalVector.at(offset + 1).p0.y = normal.y;
				normalVector.at(offset + 1).p0.z = normal.z;

				normalVector.at(offset + 1).p1.x = normal.x;
				normalVector.at(offset + 1).p1.y = normal.y;
				normalVector.at(offset + 1).p1.z = normal.z;

				normalVector.at(offset + 1).p2.x = normal.x;
				normalVector.at(offset + 1).p2.y = normal.y;
				normalVector.at(offset + 1).p2.z = normal.z;
				/*
				normalArray[offset + 9] = normal.x;
				normalArray[offset + 10] = normal.y;
				normalArray[offset + 11] = normal.z;

				normalArray[offset + 12] = normal.x;
				normalArray[offset + 13] = normal.y;
				normalArray[offset + 14] = normal.z;

				normalArray[offset + 15] = normal.x;
				normalArray[offset + 16] = normal.y;
				normalArray[offset + 17] = normal.z;
				*/

				_heights[x + z * _largestDimension] = sampleHeightmap(file, x, z, _width, _height, bpp) / divisor;

				if (z < _largestDimension - 1 && x < _largestDimension - 1)
				{
					_heights[x + (z + 1) * _largestDimension] = sampleHeightmap(file, x, z + 1, _width, _height, bpp) / divisor;
					_heights[x + 1 + (z)* _largestDimension] = sampleHeightmap(file, x + 1, z, _width, _height, bpp) / divisor;
					_heights[x + 1 + (z + 1) * _largestDimension] = sampleHeightmap(file, x + 1, z + 1, _width, _height, bpp) / divisor;
				}
			}
		}

		minHeights.at(i) = minHeight;
		maxHeights.at(i) = maxHeight;
	}

	_treeRoot = constructQuadTreeNode(
		static_cast<int>(_offsetX),
		static_cast<int>(_offsetZ),
		_largestDimension,
		_largestDimension,
		1,
		_leafSize,
		_largestDimension,
		minHeights,
		maxHeights);

	_vao.bind();

	_vertices.bind();
	_vertices.storeData(
		_triangleCount * sizeof(Triangle),
		vertexVector.data(),
		VertexBufferObjectUsage::STATIC_DRAW);

	_vertices.setupVertexAttribPointer(0, 3, 3 * sizeof(GLfloat), 0);

	_normals.bind();
	_normals.storeData(
		_triangleCount * sizeof(Triangle),
		normalVector.data(),
		VertexBufferObjectUsage::STATIC_DRAW);

	_normals.setupVertexAttribPointer(1, 3, 3 * sizeof(GLfloat), 0);

	delete file;
}

TerrainChunk::~TerrainChunk()
{
	destroyQuadTree(_treeRoot);
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

static int trianglesDrawn = 0;

void TerrainChunk::render(
	const Frustum &frustum) const
{
	_vao.bind();
	_vertices.bind();
	_normals.bind();

	trianglesDrawn = 0;

	renderWithFrustum(_treeRoot, frustum);

	float culled = 100.f * static_cast<float>(trianglesDrawn) / static_cast<float>(_triangleCount);

	//std::cout << "Triangles drawn: " << trianglesDrawn << " " << 100 - culled << "% removed" << std::endl;

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

void TerrainChunk::renderWithFrustum(
	QuadTreeNode *node,
	const Frustum &frustum) const
{
	// Extract the points of the corners of the current quad-tree node. The
	// points are given in world space.
	std::vector<glm::vec3> points = getPoints(node);

	// This will return the result of intersection test of the box spanned by
	// the points with the frustum passed to the function. A result of one
	// indicates that the whole box is inside the frustum. A result of negative
	// one indicates that the box is entirely outside the view frustum and a
	// result of zero indicates that the box is partially inside the frustum.
	// The frustum is given in world space coordinates.
	int res = frustum.boxIntersect(points);

	// If the object is entirely inside the frustum, we can send the object
	// for rendering at the current level as the hilbert-ordered memory is
	// guaranteeed to be contiguous for every level of the quad-tree.
	// If this is a leaf node, we have to render it anyways.
	// Note that q0 equals zero is equivalent to all q's equals zero.
	if (res == 1 || (res == 0 && node->q0 == nullptr))
	{
		glDrawArrays(GL_TRIANGLES,
			node->start * 6,
			node->count * 6);

		trianglesDrawn += node->count * 2;
	}
	// If there was an intersection with the planes, we have to go further into
	// the tree to determine what to cull. We call this function on each of the
	// subnodes to this tree.
	else if (res == 0)
	{
		renderWithFrustum(node->q0, frustum);
		renderWithFrustum(node->q1, frustum);
		renderWithFrustum(node->q2, frustum);
		renderWithFrustum(node->q3, frustum);
	}
	// If there was no intersection, we can safely discard the full chunk at
	// this level. The entire chunk is outside the frustum and thus not visible
	// to the camera at all.
}

glm::vec3 TerrainChunk::getVector(
	int x1,
	int z1,
	int x2,
	int z2,
	int width,
	int height,
	const TextureFile *tex,
	float divisor)
{
	float y1;
	float y2;

	if (x1 >= width || z1 >= height)
	{
		y1 = 0.f;
	}
	else
	{
		y1 = tex->getPixels()[(x1 + z1 * tex->getWidth()) * (tex->hasAlpha() ? 4 : 3)] / divisor;
	}


	if (x2 >= width || z2 >= height)
	{
		y2 = 0.f;
	}
	else
	{
		y2 = tex->getPixels()[(x2 + z2 * tex->getWidth()) * (tex->hasAlpha() ? 4 : 3)] / divisor;
	}

	glm::vec3 vec;

	vec.x = x2 - x1;
	vec.y = y2 - y1;
	vec.z = z2 - z1;

	return vec;
}
