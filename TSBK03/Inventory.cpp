#include "Inventory.h"
#include "Item.h"
#include "ItemDatabase.h"
#include <iostream>

Inventory::Inventory(
	unsigned int size,
	ItemDatabase *itemDatabase)
	:_size{ size },
	_itemDatabase{ itemDatabase }
{
	_items.resize(size);

	std::fill(_items.begin(), _items.end(), ItemInstance{ INVALID_ID, 0 });
}

ItemInstance & Inventory::getItemAt(
	unsigned int pos)
{
	return _items.at(pos);
}

bool Inventory::addItem(
	const ItemInstance &itemInstance)
{
	Item *item = _itemDatabase->getItemById(itemInstance.getID());

	if(itemInstance.getStackSize() > item->getMaxStackSize())
	{
		throw std::invalid_argument{ "Stack larger than stack size" };
	}

	// TODO: Handle stacks of items
	for(auto& it : _items)
	{
		if(it.getID() == INVALID_ID)
		{
			it = itemInstance;
			return true;
		}
	}

	return false;
}

unsigned int Inventory::getItemCount(
	unsigned int id) const
{
	unsigned int count = 0;

	for(auto& it : _items)
	{
		if(it.getID() == id)
		{
			count += it.getStackSize();
		}
	}

	return count;
}

void Inventory::removeItem(
	unsigned int id,
	unsigned int count)
{
	for(auto& it : _items)
	{
		if(it.getID() == id)
		{
			unsigned int stackSize = it.getStackSize();

			if(stackSize > count)
			{
				it.setStackSize(stackSize - count);
				return;
			}
			
			if(stackSize <= count)
			{
				count -= stackSize;
				it.setID(INVALID_ID);
			}

			if(count == 0)
			{
				return;
			}
		}
	}
}

void Inventory::removeItemAt(
	unsigned int pos)
{
	_items.at(pos).setID(INVALID_ID);
}

void Inventory::swapItems(
	unsigned int pos1,
	unsigned int pos2)
{
	ItemInstance ii = _items.at(pos1);
	_items.at(pos1) = _items.at(pos2);
	_items.at(pos2) = ii;
}

unsigned int Inventory::getSize() const
{
	return _size;
}
