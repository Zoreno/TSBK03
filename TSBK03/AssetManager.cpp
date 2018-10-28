#include "AssetManager.h"
#include "Model.h"
#include "Texture.h"
#include "Terrain.h"

AssetManager::AssetManager()
{
	registerAsset<Model>();
	registerAsset<Texture2D>();
	registerAsset<TextureCubeMap>();
	registerAsset<Terrain>();
}

AssetManager::~AssetManager()
{
	for(auto it : _assets)
	{
		delete it.second;
	}
}
