#pragma once
#include "SceneNode.h"
#include "PointLight.h"

class PointLightSceneNode : public SceneNode
{
public:
	PointLightSceneNode(
		const PointLight& pointLight, 
		const std::string& tag);

	PointLight& getPointLight();
	const PointLight& getPointLight() const;

private:
	PointLight _pointLight;
};
