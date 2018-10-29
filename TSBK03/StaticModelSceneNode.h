#pragma once

#include "SceneNode.h"
#include "Model.h"

class StaticModelSceneNode : public SceneNode
{
public:
	StaticModelSceneNode(
		const std::string& model,
		const std::string& tag);

	const std::string& getModel() const;

	void setModel(const std::string& newModel);

	const std::string& getTexture() const;
	void setTexture(const std::string& newTexture);

	std::string getCurrentAnimation() const;
	void setCurrentAnimation(const std::string& newAnimation);

	bool getOutline() const;
	void setOutline(bool newOutline);
private:

	std::string _model;

	std::string _texture;

	std::string _currentAnimation;

	bool _outline;
};