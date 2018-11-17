#pragma once

#include "Item.h"
#include <map>

#define INVALID_ID 0xFFFFFFFFU

class ItemDatabase
{
public:

	ItemDatabase();

	Item *getItemById(unsigned int id);

	bool invokeOnUseFunction(const std::string& onUseFunction, Game *game);
private:

	void loadItems();
	void loadOnUseFunctions();

	std::map<unsigned int, Item *> _items;

	std::map<std::string, std::function<bool(Game *)>> _onUseFunctions;
};