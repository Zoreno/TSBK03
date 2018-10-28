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

template <typename T>
AssetPool<T>::~AssetPool()
{
	for (auto& it : _assets)
	{
		delete it.second;
	}
}

template <typename T>
void AssetPool<T>::removeAsset(
	const std::string &assetID)
{
	auto it = _assets.find(assetID);

	if (it == _assets.end())
	{
		return;
	}

	delete it->second;
	_assets.erase(it);
}

template <typename T>
T * AssetPool<T>::fetchAsset(
	const std::string &assetID) const
{
	auto it = _assets.find(assetID);

	if (it == _assets.end())
	{
		return nullptr;
	}

	return it->second;
}

template <typename T>
template <typename ... Args>
T *AssetPool<T>::loadAsset(
	const std::string &assetID,
	Args... args)
{
	T *ret = new T(std::forward<Args>(args)...);

	_assets.emplace(assetID, ret);

	return ret;
}

template <typename T>
void AssetPool<T>::storeAsset(
	const std::string &assetID,
	T *assetPtr)
{
	_assets.emplace(assetID, assetPtr);
}

template <typename T>
void AssetPool<T>::storeAsset(
	const std::string &assetID,
	T &assetRef)
{
	_assets.emplace(assetID, &assetRef);
}

template <typename T>
const std::map<std::string, T *> & AssetPool<T>::getAssets() const
{
	return _assets;
}
