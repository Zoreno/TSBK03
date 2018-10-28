#pragma once

#include <GL/glew.h>

enum class VertexBufferObjectTarget
{
	ARRAY_BUFFER,
	ELEMENT_BUFFER
};

enum class VertexBufferObjectUsage
{
	STATIC_DRAW,
};

class VertexBufferObject
{
public:
	explicit VertexBufferObject(VertexBufferObjectTarget target);

	VertexBufferObject(const VertexBufferObject& other) = delete;
	VertexBufferObject(VertexBufferObject&& other) noexcept;

	VertexBufferObject& operator=(const VertexBufferObject& other) = delete;
	VertexBufferObject& operator=(VertexBufferObject&& other) noexcept;

	~VertexBufferObject();

	void bind() const;
	void unbind() const;

	void storeData(
		unsigned int size,
		const void *data,
		VertexBufferObjectUsage usage);

	void setupVertexAttribPointer(
		GLuint location,
		unsigned int elementSize,
		unsigned int stride,
		unsigned int offset);

	GLuint getHandle() const;
	VertexBufferObjectTarget getTarget() const;
	unsigned int getSize() const;

	friend void swap(VertexBufferObject& rhs, VertexBufferObject& lhs) noexcept;

private:
	GLuint _handle;
	VertexBufferObjectTarget _target;
	GLuint _targetGL{};
	unsigned int _size;
};