#include "PointLight.h"

PointLight::PointLight(
	const glm::vec3 &ambient,
	const glm::vec3 &diffuse,
	const glm::vec3 &specular,
	float constant,
	float linear,
	float quadratic)
	: _constant{constant},
	  _linear{linear},
	  _quadratic{quadratic},
	  _ambient{ambient},
	  _diffuse{diffuse},
	  _specular{specular}
{

}

float PointLight::getConstant() const
{
	return _constant;
}

void PointLight::setConstant(
	float newConstant)
{
	_constant = newConstant;
}

float PointLight::getLinear() const
{
	return _linear;
}

void PointLight::setLinear(
	float newLinear)
{
	_linear = newLinear;
}

float PointLight::getQuadratic() const
{
	return _quadratic;
}

void PointLight::setQuadratic(
	float newQuadratic)
{
	_quadratic = newQuadratic;
}

glm::vec3 PointLight::getAmbient() const
{
	return _ambient;
}

void PointLight::setAmbient(
	const glm::vec3 &newAmbient)
{
	_ambient = newAmbient;
}

glm::vec3 PointLight::getDiffuse() const
{
	return _diffuse;
}

void PointLight::setDiffuse(
	const glm::vec3 &newDiffuse)
{
	_diffuse = newDiffuse;
}

glm::vec3 PointLight::getSpecular() const
{
	return _specular;
}

void PointLight::setSpecular(
	const glm::vec3 &newSpecular)
{
	_specular = newSpecular;
}

float PointLight::calculateAttenuation(
	float distance) const
{
	return 1.f / (_constant + distance * (_linear + distance * _quadratic));
}