#include "StaticModelSceneNode.h"

StaticModelSceneNode::StaticModelSceneNode(
	const std::string &model,
	const std::string &tag)
	:SceneNode(SceneNodeType::STATIC_MODEL, tag), _model{ model }
{
	
}

const std::string & StaticModelSceneNode::getModel() const
{
	return _model;
}

void StaticModelSceneNode::setModel(
	const std::string &newModel)
{
	_model = newModel;
}

const std::string & StaticModelSceneNode::getTexture() const
{
	return _texture;
}

void StaticModelSceneNode::setTexture(
	const std::string &newTexture)
{
	_texture = newTexture;
}

std::string StaticModelSceneNode::getCurrentAnimation() const
{
	return _currentAnimation;
}

void StaticModelSceneNode::setCurrentAnimation(
	const std::string &newAnimation)
{
	_currentAnimation = newAnimation;
}
