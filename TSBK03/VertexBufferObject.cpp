#include "VertexBufferObject.h"
#include <iostream>

VertexBufferObject::VertexBufferObject(
	VertexBufferObjectTarget target)
	:_target{ target }
{
	switch (target)
	{
	case VertexBufferObjectTarget::ARRAY_BUFFER:
		_targetGL = GL_ARRAY_BUFFER;
		break;
	case VertexBufferObjectTarget::ELEMENT_BUFFER:
		_targetGL = GL_ELEMENT_ARRAY_BUFFER;
		break;
	default:
		std::cerr << "Error defining target for vertex buffer object" << std::endl;
		break;;
	}

	glGenBuffers(1, &_handle);
}

VertexBufferObject::VertexBufferObject(
	VertexBufferObject &&other) noexcept
{
	swap(*this, other);
}

VertexBufferObject & VertexBufferObject::operator=(
	VertexBufferObject &&other) noexcept
{
	swap(*this, other);

	return *this;
}

VertexBufferObject::~VertexBufferObject()
{
	glDeleteBuffers(1, &_handle);
}

void VertexBufferObject::bind() const
{
	glBindBuffer(_targetGL, _handle);
}

void VertexBufferObject::unbind() const
{
	glBindBuffer(_targetGL, 0);
}

void VertexBufferObject::storeData(
	unsigned int size,
	const void *data,
	VertexBufferObjectUsage usage)
{
	GLenum usageGL;

	switch (usage)
	{
	case VertexBufferObjectUsage::STATIC_DRAW:
		usageGL = GL_STATIC_DRAW;
		break;
	default:
		std::cerr << "Error defining usage for vertex buffer object" << std::endl;
		break;
	}

	bind();
	glBufferData(_targetGL, size, data, usageGL);
	_size = size;
	unbind();
}

void VertexBufferObject::storeSubData(
	unsigned int offset,
	unsigned int size,
	const void *data)
{
	bind();
	glBufferSubData(_targetGL, offset, size, data);
	unbind();
}

void VertexBufferObject::setupVertexAttribPointer(
	GLuint location,
	unsigned int elementSize,
	unsigned int stride,
	unsigned int offset)
{
	bind();

	glVertexAttribPointer(
		location,
		elementSize,
		GL_FLOAT,
		GL_FALSE,
		stride,
		reinterpret_cast<void *>(offset));

	glEnableVertexAttribArray(location);

	unbind();
}

GLuint VertexBufferObject::getHandle() const
{
	return _handle;
}

VertexBufferObjectTarget VertexBufferObject::getTarget() const
{
	return _target;
}

unsigned VertexBufferObject::getSize() const
{
	return _size;
}

void swap(
	VertexBufferObject &rhs,
	VertexBufferObject &lhs) noexcept
{
	std::swap(rhs._handle, lhs._handle);
	std::swap(rhs._targetGL, lhs._targetGL);
	std::swap(rhs._target, lhs._target);
	std::swap(rhs._size, lhs._size);
}
