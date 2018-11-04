#include "VertexArrayObject.h"

VertexArrayObject::VertexArrayObject()
{
	glGenVertexArrays(1, &_handle);
}

VertexArrayObject::~VertexArrayObject()
{
	if (isValid())
	{
		glDeleteVertexArrays(1, &_handle);
	}
}

VertexArrayObject::VertexArrayObject(
	VertexArrayObject &&other) noexcept
{
	swap(*this, other);
}

VertexArrayObject & VertexArrayObject::operator=(
	VertexArrayObject &&other) noexcept
{
	swap(*this, other);

	return *this;
}

void VertexArrayObject::bind() const
{
	glBindVertexArray(_handle);
}

void VertexArrayObject::unbind()
{
	glBindVertexArray(0);
}

bool VertexArrayObject::isValid() const
{
	return glIsVertexArray(_handle) != 0;
}

GLuint VertexArrayObject::getHandle() const
{
	return _handle;
}

void swap(
	VertexArrayObject &rhs,
	VertexArrayObject &lhs) noexcept
{
	GLuint temp;

	temp = rhs._handle;
	rhs._handle = lhs._handle;
	lhs._handle = temp;
}
