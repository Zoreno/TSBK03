#include "PointLightSceneNode.h"

PointLightSceneNode::PointLightSceneNode(
	const PointLight &pointLight,
	const std::string &tag)
	:SceneNode{ SceneNodeType::POINT_LIGHT, tag }, _pointLight{pointLight}
{

}

PointLight & PointLightSceneNode::getPointLight()
{
	return _pointLight;
}

const PointLight & PointLightSceneNode::getPointLight() const
{
	return _pointLight;
}
