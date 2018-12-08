#pragma once

#include <vector>
#include "ItemInstance.h"

class ItemDatabase;

class Inventory
{
public:
	Inventory(unsigned int size, ItemDatabase *itemDatabase);

	unsigned int getSize() const;

	ItemInstance& getItemAt(unsigned int pos);

	unsigned int addItem(const ItemInstance& itemInstance);
	unsigned int addItemCount(unsigned int id, unsigned int count);
	unsigned int getItemCount(unsigned int id) const;

	void removeItem(unsigned int id, unsigned int count);
	void removeItemAt(unsigned int pos);
	void removeItemCountAt(unsigned int pos, unsigned int count);

	void swapItems(unsigned int pos1, unsigned int pos2);
private:

	unsigned int _size;

	std::vector<ItemInstance> _items;

	ItemDatabase *_itemDatabase;
};
