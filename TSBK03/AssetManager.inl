template <typename T, typename ... Args>
T *AssetManager::load(
	const std::string &assetID,
	Args... args)
{
	AssetPool<T> *pool = getPool<T>();

	return pool->loadAsset(assetID, std::forward<Args>(args)...);
}

template <typename T>
void AssetManager::store(
	const std::string &assetID,
	T *assetPtr)
{
	AssetPool<T> *pool = getPool<T>();

	pool->storeAsset(assetID, assetPtr);
}

template <typename T>
void AssetManager::store(
	const std::string &assetID,
	T &assetRef)
{
	AssetPool<T> *pool = getPool<T>();

	pool->storeAsset(assetID, assetRef);
}

template <typename T>
T * AssetManager::fetch(
	const std::string &assetID) const
{
	AssetPool<T>* pool = getPool<T>();

	return pool->fetchAsset(assetID);
}

template <typename T>
void AssetManager::dispose(
	const std::string &assetID)
{
	AssetPool<T>* pool = getPool<T>();

	pool->removeAsset(assetID);
}

template <typename T>
void AssetManager::registerAsset()
{
	size_t id = std::type_index{ typeid(T) }.hash_code();

	auto it = _assets.find(id);

	if (it == _assets.end())
	{
		_assets.emplace(id, new AssetPool<T>{});
	}
}

template <typename T>
AssetPool<T> * AssetManager::getPool() const
{
	size_t id = std::type_index{ typeid(T) }.hash_code();

	auto it = _assets.find(id);

	if (it == _assets.end())
	{
		return nullptr;
	}

	AssetPool<T> *pool = dynamic_cast<AssetPool<T> *>(it->second);

	return pool;
}
