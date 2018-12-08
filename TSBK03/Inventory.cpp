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

unsigned int Inventory::addItem(
	const ItemInstance &itemInstance)
{
	Item *item = _itemDatabase->getItemById(itemInstance.getID());

	if(itemInstance.getStackSize() > item->getMaxStackSize())
	{
		throw std::invalid_argument{ "Stack larger than stack size" };
	}

	unsigned int countLeft = itemInstance.getStackSize();

	// Find all non-empty stacks of this item and try to fill those
	for(auto& it : _items)
	{
		if(it.getID() == itemInstance.getID())
		{
			unsigned int freeSpots = item->getMaxStackSize() - it.getStackSize();

			if(freeSpots >= countLeft)
			{
				it.setStackSize(it.getStackSize() + countLeft);
				return 0;
			}

			it.setStackSize(item->getMaxStackSize());
			countLeft -= freeSpots;
		}
	}

	// Else, find an empty slot and place the item there.
	for(auto& it : _items)
	{
		if(it.getID() == INVALID_ID)
		{
			it = ItemInstance(itemInstance.getID(), countLeft);
			return 0;
		}
#if 0
		if(it.getID() == itemInstance.getID())
		{
			unsigned int freeSpots = item->getMaxStackSize() - it.getStackSize();

			if(freeSpots >= countLeft)
			{
				it.setStackSize(it.getStackSize() + countLeft);
				return 0;
			}

			it.setStackSize(item->getMaxStackSize());

			countLeft -= freeSpots;
		}
#endif
	}

	return countLeft;
}

unsigned int Inventory::addItemCount(
	unsigned int id,
	unsigned int count)
{
	return addItem(ItemInstance{ id, count });
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

void Inventory::removeItemCountAt(
	unsigned int pos,
	unsigned int count)
{
	ItemInstance ii = _items.at(pos);

	unsigned int stackSize = ii.getStackSize();

	if(count > stackSize)
	{
		throw std::invalid_argument("Removing more items from stack than exists in stack");
	}

	if(count == stackSize)
	{
		_items.at(pos) = ItemInstance(INVALID_ID, 0);
	}
	else
	{
		ii.setStackSize(stackSize - count);
		_items.at(pos) = ii;
	}
}

void Inventory::swapItems(
	unsigned int pos1,
	unsigned int pos2)
{
	ItemInstance ii1 = _items.at(pos1);
	ItemInstance ii2 = _items.at(pos2);

	// If the target and the source share the same item id, we fill the target
	// target stack with items from source instead of swapping
	if(ii1.getID() == ii2.getID())
	{
		Item *item = _itemDatabase->getItemById(ii1.getID());

		// If the target stack has open item slots
		if(ii1.getStackSize() < item->getMaxStackSize())
		{
			// Calculate the number of free slots
			unsigned int freeslots = item->getMaxStackSize() - ii1.getStackSize();

			// If the items in source fit in target
			if(ii2.getStackSize() <= freeslots)
			{
				// Set the stack size of the target to the sum of the target's
				// and source's stack sizes
				ii1.setStackSize(ii1.getStackSize() + ii2.getStackSize());

				// Place that item in the target slot
				_items.at(pos1) = ii1;

				// Clear the source slot.
				_items.at(pos2) = ItemInstance(INVALID_ID, 0);
			}
			else
			{
				// Fill the target slot completely
				ii1.setStackSize(item->getMaxStackSize());

				// Remove the number of free slots from the source
				ii2.setStackSize(ii2.getStackSize() - freeslots);

				// Update the target
				_items.at(pos1) = ii1;

				// Update the source
				_items.at(pos2) = ii2;
			}
		}
	}
	else
	{
		// Ordinary swapping of the slots
		_items.at(pos1) = ii2;
		_items.at(pos2) = ii1;
	}
}

unsigned int Inventory::getSize() const
{
	return _size;
}
