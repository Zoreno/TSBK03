#pragma once

#include <string>
#include <vector>

#include "Vertex.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "VertexBoneData.h"

class Mesh
{
public:
	Mesh(
		const std::vector<Vertex>& vertices,
		const std::vector<unsigned int>& indices,
		const std::vector<VertexBoneData>& boneData,
		unsigned int materialIndex);
	~Mesh();

	void render() const;

	const std::vector<Vertex>& getVertices() const;
	const std::vector<unsigned int>& getIndices() const;
	const std::vector<VertexBoneData>& getBoneData() const;

	unsigned int getMaterialIndex() const;
private:

	void setupMesh();

	unsigned int _materialIndex{ 0 };

	std::vector<Vertex> _vertices{};
	std::vector<unsigned int> _indices{};
	std::vector<VertexBoneData> _vertexBoneData{};

	VertexArrayObject _vao;
	VertexBufferObject _vbo;
	VertexBufferObject _boneData;
	VertexBufferObject _ibo;
};
