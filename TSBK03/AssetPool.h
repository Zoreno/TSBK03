#pragma once

#include "AssetBase.h"

#include <string>
#include <map>
#include <typeindex>

template <typename T>
class AssetPool : public AssetBase
{
public:
	AssetPool() = default;

	~AssetPool() override;

	AssetPool(const AssetPool& other) = delete;
	AssetPool(AssetPool&& other) noexcept = delete;

	AssetPool& operator=(const AssetPool& other) = delete;
	AssetPool& operator=(AssetPool&& other) noexcept = delete;

	void removeAsset(const std::string& assetID) override;

	T *fetchAsset(const std::string& assetID) const;

	template <typename ... Args>
	T *loadAsset(const std::string& assetID, Args ... args);

	void storeAsset(const std::string& assetID, T *assetPtr);
	void storeAsset(const std::string& assetID, T& assetRef);

	const std::map<std::string, T *>& getAssets() const;

private:
	std::map<std::string, T *> _assets{};
};

#include "AssetPool.inl"