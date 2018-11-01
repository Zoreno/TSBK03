#pragma once

#include <string>
#include <map>
#include <typeindex>

#include "AssetPool.h"

// TODO: Load asset from file

class AssetManager
{
public:
	AssetManager();
	AssetManager(const AssetManager& other) = delete;
	AssetManager(AssetManager&& other) = delete;

	AssetManager& operator=(const AssetManager& other) = delete;
	AssetManager& operator=(AssetManager&& other) = delete;
	
	~AssetManager();

	template <typename T, typename ... Args>
	T *load(const std::string& assetID, Args ... args);

	template <typename T>
	void store(const std::string& assetID, T *assetPtr);

	template <typename T>
	void store(const std::string& assetID, T& assetRef);

	template <typename T>
	T *fetch(const std::string& assetID) const;

	template <typename T>
	void dispose(const std::string& assetID);

	template <typename T>
	void registerAsset();

	template <typename T>
	AssetPool<T> *getPool() const;
private:

	std::map<size_t, AssetBase *> _assets{};
};

#include "AssetManager.inl"