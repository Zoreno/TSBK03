#pragma once

#include "Item.h"
#include <map>

#define INVALID_ID 0xFFFFFFFFU

class ItemDatabase
{
public:
	ItemDatabase();

	Item *getItemById(unsigned int id);
private:
	std::map<unsigned int, Item *> _items;
};