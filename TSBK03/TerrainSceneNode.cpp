#include "TerrainSceneNode.h"

TerrainSceneNode::TerrainSceneNode(
	const std::string &terrain,
	const std::string &tag)
	:SceneNode(SceneNodeType::TERRAIN, tag),
	_terrain{ terrain }
{

}

const std::string & TerrainSceneNode::getTerrain() const
{
	return _terrain;
}
