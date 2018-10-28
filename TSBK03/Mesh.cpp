#include "Mesh.h"
#include <GL/glew.h>

#include <iostream>

Mesh::Mesh(
	const std::vector<Vertex>& vertices,
	const std::vector<unsigned int>& indices,
	const std::vector<VertexBoneData>& boneData,
	unsigned int materialIndex)
	:_materialIndex{ materialIndex },
	_vertices{ vertices },
	_indices{ indices },
	_vertexBoneData{ boneData },
	_vbo{ VertexBufferObjectTarget::ARRAY_BUFFER },
	_boneData{ VertexBufferObjectTarget::ARRAY_BUFFER },
	_ibo{ VertexBufferObjectTarget::ELEMENT_BUFFER }
{
	setupMesh();
}

Mesh::~Mesh()
{

}

void Mesh::render() const
{
	_vao.bind();
	_vbo.bind();
	_ibo.bind();

	glDrawElements(
		GL_TRIANGLES, 
		static_cast<GLsizei>(_indices.size()), 
		GL_UNSIGNED_INT, 
		nullptr);

	_ibo.unbind();
	_vbo.unbind();
	VertexArrayObject::unbind();
}

const std::vector<Vertex> & Mesh::getVertices() const
{
	return _vertices;
}

const std::vector<unsigned> & Mesh::getIndices() const
{
	return _indices;
}

const std::vector<VertexBoneData> & Mesh::getBoneData() const
{
	return _vertexBoneData;
}

unsigned Mesh::getMaterialIndex() const
{
	return _materialIndex;
}

void Mesh::setupMesh()
{
	_vao.bind();

	_vbo.storeData(
		static_cast<unsigned int>(_vertices.size()) * sizeof(Vertex),
		&_vertices[0],
		VertexBufferObjectUsage::STATIC_DRAW);

	_ibo.storeData(
		static_cast<unsigned int>(_indices.size()) * sizeof(unsigned int),
		&_indices[0],
		VertexBufferObjectUsage::STATIC_DRAW);

	_vbo.setupVertexAttribPointer(0, 3, sizeof(Vertex), offsetof(Vertex, _position));
	_vbo.setupVertexAttribPointer(1, 3, sizeof(Vertex), offsetof(Vertex, _normal));
	_vbo.setupVertexAttribPointer(2, 2, sizeof(Vertex), offsetof(Vertex, _texCoord));
	_vbo.setupVertexAttribPointer(3, 3, sizeof(Vertex), offsetof(Vertex, _tangent));
	_vbo.setupVertexAttribPointer(4, 3, sizeof(Vertex), offsetof(Vertex, _bitangent));

	if (!_vertexBoneData.empty())
	{
		_boneData.storeData(
			static_cast<unsigned int>(_vertexBoneData.size()) * sizeof(VertexBoneData),
			&_vertexBoneData[0],
			VertexBufferObjectUsage::STATIC_DRAW);

		_boneData.setupVertexAttribPointer(5, 4, sizeof(VertexBoneData), offsetof(VertexBoneData, IDs));
		_boneData.setupVertexAttribPointer(6, 4, sizeof(VertexBoneData), offsetof(VertexBoneData, weights));
	}

	VertexArrayObject::unbind();
}

