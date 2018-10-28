#include "Vertex.h"

Vertex::Vertex()
	:_position{},
	_normal{},
	_texCoord{},
	_tangent{},
	_bitangent{}
{

}

Vertex::Vertex(
	const Vertex &other)
	:_position{ other._position },
	_normal{ other._normal },
	_texCoord{ other._texCoord },
	_tangent{ other._tangent },
	_bitangent{ other._bitangent }
{

}

Vertex::Vertex(
	Vertex &&other) noexcept
{
	swap(*this, other);
}

Vertex::~Vertex()
{
	// Nothing to do.
}

Vertex & Vertex::operator=(
	const Vertex &other)
{
	_position = other._position;
	_normal = other._normal;
	_texCoord = other._texCoord;
	_tangent = other._tangent;
	_bitangent = other._bitangent;

	return *this;
}

Vertex & Vertex::operator=(
	Vertex &&other) noexcept
{
	swap(*this, other);

	return *this;
}

glm::vec3 Vertex::getPosition() const
{
	return _position;
}

void Vertex::setPosition(
	const glm::vec3 &newPosition)
{
	_position = newPosition;
}

glm::vec3 Vertex::getNormal() const
{
	return _normal;
}

void Vertex::setNormal(
	const glm::vec3 &newNormal)
{
	_normal = newNormal;
}

glm::vec2 Vertex::getTexCoord() const
{
	return _texCoord;
}

void Vertex::setTexCoord(
	const glm::vec2 &newTexCoord)
{
	_texCoord = newTexCoord;
}

glm::vec3 Vertex::getTangent() const
{
	return _tangent;
}

void Vertex::setTangent(
	const glm::vec3 &newTangent)
{
	_tangent = newTangent;
}

glm::vec3 Vertex::getBitangent() const
{
	return _bitangent;
}

void Vertex::setBitangent(
	const glm::vec3 &newBitangent)
{
	_bitangent = newBitangent;
}

static void swap(glm::vec2& lhs, glm::vec2& rhs) noexcept
{
	const glm::vec2 temp{ lhs };
	lhs = rhs;
	rhs = temp;
}

static void swap(glm::vec3& lhs, glm::vec3& rhs) noexcept
{
	const glm::vec3 temp{ lhs };
	lhs = rhs;
	rhs = temp;
}

void swap(
	Vertex &lhs,
	Vertex &rhs) noexcept
{
	swap(lhs._position, rhs._position);
	swap(lhs._normal, rhs._normal);
	swap(lhs._texCoord, rhs._texCoord);
	swap(lhs._tangent, rhs._tangent);
	swap(lhs._bitangent, rhs._bitangent);
}
