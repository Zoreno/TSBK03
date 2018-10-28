#pragma once

#include "SceneNode.h"
#include "DirectionalLight.h"

class DirectionalLightSceneNode : public SceneNode
{
public:
	DirectionalLightSceneNode(
		const DirectionalLight& directionalLight, 
		const std::string& tag);

	DirectionalLight& getDirectionalLight();
	const DirectionalLight& getDirectionalLight() const;
private:
	DirectionalLight _directionalLight;
};