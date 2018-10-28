#pragma once

#include <vector>
#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SceneNodeType.h"

class SceneNode
{
public:
	SceneNode();

	explicit SceneNode(SceneNodeType type, const std::string& tag);

	explicit SceneNode(const SceneNode& other) = delete;
	explicit SceneNode(SceneNode&& other) = delete;
	SceneNode& operator=(const SceneNode& other) = delete;
	SceneNode& operator=(SceneNode&& other) = delete;

	virtual ~SceneNode();

	SceneNodeType getSceneNodeType() const;

	glm::vec3 getPosition() const;
	void setPosition(const glm::vec3& newPos);

	glm::vec3 getRotation() const;
	void setRotation(const glm::vec3& newRotation);

	glm::vec3 getScale() const;
	void setScale(const glm::vec3& newScale);

	glm::mat4 getLocalTransformationMatrix() const;
	glm::mat4 getTransformationMatrix() const;

	SceneNode *getParent() const;
	void setParent(SceneNode *newParent);

	const std::string& getTag() const;
	void setTag(const std::string& newTag);

	const std::vector<SceneNode *>& getChildren() const;
	std::vector<SceneNode *>& getChildren();
	unsigned int getChildrenCount() const;

	void addChild(SceneNode *newChild);

	SceneNode *findChildrenByTag(const std::string& tag) const;

	unsigned int getID() const;

	SceneNode *findNodeByID(unsigned int id) const;
protected:

	SceneNodeType _type{ SceneNodeType::INVALID };

	//============================================================================
	// Transformation
	//============================================================================

	glm::vec3 _position{ 0.f, 0.f, 0.f };

	// TODO: Change rotation representation to Quaternions
	glm::vec3 _rotation{ 1.f, 0.f, 0.f };

	glm::vec3 _scale{ 1.f, 1.f, 1.f };

	//============================================================================
	// Tree Structure
	//============================================================================

	SceneNode *_parent{ nullptr };

	// TODO: Spatial Partitioning
	// TODO: Map these elements to the tag to reduce searching time for large scenes.
	std::vector<SceneNode *> _children{};

	//============================================================================
	// Identification
	//============================================================================

	std::string _tag{ "INVALID" };

	unsigned int _id{ 0 };

};

using Scene = SceneNode;