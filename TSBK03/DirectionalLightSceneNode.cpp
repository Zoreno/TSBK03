#include "DirectionalLightSceneNode.h"

DirectionalLightSceneNode::DirectionalLightSceneNode(
	const DirectionalLight &directionalLight,
	const std::string &tag)
	:SceneNode{SceneNodeType::DIRECTIONAL_LIGHT, tag}, _directionalLight{directionalLight}
{
	
}

DirectionalLight & DirectionalLightSceneNode::getDirectionalLight()
{
	return _directionalLight;
}

const DirectionalLight & DirectionalLightSceneNode::
getDirectionalLight() const
{
	return _directionalLight;
}
