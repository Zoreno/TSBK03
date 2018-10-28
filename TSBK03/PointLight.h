#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class PointLight
{
public:
	PointLight(
		const glm::vec3 &ambient,
		const glm::vec3 &diffuse,
		const glm::vec3 &specular,
		float constant,
		float linear,
		float quadratic);

	float getConstant() const;
	void setConstant(float newConstant);

	float getLinear() const;
	void setLinear(float newLinear);

	float getQuadratic() const;
	void setQuadratic(float newQuadratic);

	glm::vec3 getAmbient() const;
	void setAmbient(const glm::vec3& newAmbient);

	glm::vec3 getDiffuse() const;
	void setDiffuse(const glm::vec3& newDiffuse);

	glm::vec3 getSpecular() const;
	void setSpecular(const glm::vec3& newSpecular);

	float calculateAttenuation(float distance) const;

	// TODO: float calculateBoundingRadius() const;

private:

	float _constant{1.f};
	float _linear{0.f};
	float _quadratic{0.f};

	glm::vec3 _ambient;
	glm::vec3 _diffuse;
	glm::vec3 _specular;
};