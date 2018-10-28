#pragma once

#include "SceneNode.h"

class TerrainSceneNode : public SceneNode
{
public:
	TerrainSceneNode(
		const std::string& terrain, 
		const std::string& tag);

	const std::string& getTerrain() const;
private:
	std::string _terrain;
};