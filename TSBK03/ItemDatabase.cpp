#include "ItemDatabase.h"

ItemDatabase::ItemDatabase()
{
	_items.emplace(0, new Item(0, "Fisk", ItemType::GENERIC, ItemRarity::LEGENDARY, "\"This is a fish\"", 1, 0, 0));
	_items.emplace(1, new Item(1, "Sten", ItemType::GENERIC, ItemRarity::SUPERIOR, "\"This is a sten\"", 1, 1, 0));
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
