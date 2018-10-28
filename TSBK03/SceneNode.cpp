#include "SceneNode.h"

static unsigned int nextid = 1;

SceneNode::SceneNode()
{
	_id = nextid++;
}

SceneNode::SceneNode(
	SceneNodeType type,
	const std::string& tag)
	:_type{ type }, _tag{ tag }
{
	_id = nextid++;
}

SceneNode::~SceneNode()
{
	while (!_children.empty())
	{
		SceneNode *it = _children.back();

		delete it;

		_children.pop_back();

	}
}

SceneNodeType SceneNode::getSceneNodeType() const
{
	return _type;
}

glm::vec3 SceneNode::getPosition() const
{
	return _position;
}

void SceneNode::setPosition(
	const glm::vec3 &newPos)
{
	_position = newPos;
}

glm::vec3 SceneNode::getRotation() const
{
	return _rotation;
}

void SceneNode::setRotation(
	const glm::vec3 &newRotation)
{
	_rotation = newRotation;
}

glm::vec3 SceneNode::getScale() const
{
	return _scale;
}

void SceneNode::setScale(
	const glm::vec3 &newScale)
{
	_scale = newScale;
}

glm::mat4 SceneNode::getLocalTransformationMatrix() const
{
	glm::mat4 result{ 1.f };

	result = glm::translate(result, _position);

	result = glm::rotate(result, glm::radians(_rotation.x), glm::vec3{ 1.f, 0.f, 0.f });
	result = glm::rotate(result, glm::radians(_rotation.y), glm::vec3{ 0.f, 1.f, 0.f });
	result = glm::rotate(result, glm::radians(_rotation.z), glm::vec3{ 0.f, 0.f, 1.f });

	result = glm::scale(result, _scale);

	return result;
}

glm::mat4 SceneNode::getTransformationMatrix() const
{
	return getLocalTransformationMatrix() * (_parent != nullptr ? _parent->getTransformationMatrix() : glm::mat4{ 1.f });
}

SceneNode * SceneNode::getParent() const
{
	return _parent;
}

void SceneNode::setParent(
	SceneNode *newParent)
{
	_parent = newParent;
}

const std::string & SceneNode::getTag() const
{
	return _tag;
}

void SceneNode::setTag(
	const std::string &newTag)
{
	_tag = newTag;
}

const std::vector<SceneNode *> & SceneNode::getChildren() const
{
	return _children;
}

std::vector<SceneNode *> & SceneNode::getChildren()
{
	return _children;
}

unsigned int SceneNode::getChildrenCount() const
{
	return _children.size();
}

void SceneNode::addChild(
	SceneNode *newChild)
{
	// TODO: Make sure that the tag is unique.

	_children.push_back(newChild);
	newChild->setParent(this);
}

SceneNode * SceneNode::findChildrenByTag(
	const std::string &tag) const
{
	for (auto it : _children)
	{
		if (it->_tag == tag)
		{
			return it;
		}
	}

	return nullptr;
}

unsigned int SceneNode::getID() const
{
	return _id;
}

SceneNode * SceneNode::findNodeByID(unsigned int id) const
{
	if (id == _id)
	{
		return const_cast<SceneNode *>(this);
	}

	for (auto it : _children)
	{
		SceneNode *ret = it->findNodeByID(id);

		if (ret != nullptr)
		{
			return ret;
		}
	}

	return nullptr;
}
