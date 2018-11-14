#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class DirectionalLight
{
public:
	DirectionalLight(
		const glm::vec3& direction, 
		const glm::vec3& ambient,
		const glm::vec3& diffuse, 
		const glm::vec3& specular);

	glm::vec3 getDirection() const;
	void setDirection(const glm::vec3& newDirection);

	glm::vec3 getAmbient() const;
	void setAmbient(const glm::vec3& newAmbient);

	glm::vec3 getDiffuse() const;
	void setDiffuse(const glm::vec3& newDiffuse);

	glm::vec3 getSpecular() const;
	void setSpecular(const glm::vec3& newSpecular);
private:
	glm::vec3 _direction;

	glm::vec3 _ambient;
	glm::vec3 _diffuse;
	glm::vec3 _specular;
};