#pragma once

#include <GL/glew.h>

class VertexArrayObject
{
public:
	VertexArrayObject();
	~VertexArrayObject();

	VertexArrayObject(const VertexArrayObject& other) = delete;
	VertexArrayObject(VertexArrayObject&& other) noexcept;

	VertexArrayObject& operator=(const VertexArrayObject& other) = delete;
	VertexArrayObject& operator=(VertexArrayObject&& other) noexcept;

	void bind() const;
	static void unbind();

	bool isValid() const;

	GLuint getHandle() const;

	friend void swap(VertexArrayObject& rhs, VertexArrayObject& lhs) noexcept;
private:

	GLuint _handle{0};
};