#pragma once
#include <string>

class AssetBase
{
public:

	virtual ~AssetBase() {}

	virtual void removeAsset(const std::string& assetID) = 0;
};
