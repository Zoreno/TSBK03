#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(
	const glm::vec3 &direction,
	const glm::vec3 &ambient,
	const glm::vec3 &diffuse,
	const glm::vec3 &specular)
	:_direction{ direction },
	_ambient{ ambient },
	_diffuse{ diffuse },
	_specular{ specular }
{

}

glm::vec3 DirectionalLight::getDirection() const
{
	return _direction;
}

void DirectionalLight::setDirection(
	const glm::vec3 &newDirection)
{
	_direction = newDirection;
}

glm::vec3 DirectionalLight::getAmbient() const
{
	return _ambient;
}

void DirectionalLight::setAmbient(
	const glm::vec3 &newAmbient)
{
	_ambient = newAmbient;
}

glm::vec3 DirectionalLight::getDiffuse() const
{
	return _diffuse;
}

void DirectionalLight::setDiffuse(
	const glm::vec3 &newDiffuse)
{
	_diffuse = newDiffuse;
}

glm::vec3 DirectionalLight::getSpecular() const
{
	return _specular;
}

void DirectionalLight::setSpecular(
	const glm::vec3 &newSpecular)
{
	_specular = newSpecular;
}
