#include "ItemDatabase.h"

ItemDatabase::ItemDatabase()
{
	_items.emplace(0, new Item(0, "Fisk", ItemType::GENERIC, ItemRarity::EPIC, "This is a fish"));
}

Item * ItemDatabase::getItemById(unsigned int id)
{
	auto it = _items.find(id);

	if(it != _items.end())
	{
		return it->second;
	}

	return nullptr;
}
