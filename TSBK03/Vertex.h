#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Vertex
{
public:
	Vertex();
	
	Vertex(const Vertex& other);
	Vertex(Vertex&& other) noexcept;

	virtual ~Vertex();

	Vertex& operator=(const Vertex& other);
	Vertex& operator=(Vertex&& other) noexcept;

	glm::vec3 getPosition() const;
	void setPosition(const glm::vec3& newPosition);

	glm::vec3 getNormal() const;
	void setNormal(const glm::vec3& newNormal);

	glm::vec2 getTexCoord() const;
	void setTexCoord(const glm::vec2& newTexCoord);

	glm::vec3 getTangent() const;
	void setTangent(const glm::vec3& newTangent);

	glm::vec3 getBitangent() const;
	void setBitangent(const glm::vec3& newBitangent);

	friend void swap(Vertex& lhs, Vertex& rhs) noexcept;
private:

	friend class Model;
	friend class Mesh;

	glm::vec3 _position;

	glm::vec3 _normal;

	glm::vec2 _texCoord;

	glm::vec3 _tangent;

	glm::vec3 _bitangent;
};