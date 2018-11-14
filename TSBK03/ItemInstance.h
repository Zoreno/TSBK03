#pragma once
#include <utility>

#define INVALID_ID 0xFFFFFFFFU

class ItemInstance
{
public:
	explicit ItemInstance(
		unsigned int id = INVALID_ID,
		unsigned int stackSize = 1);

	ItemInstance(const ItemInstance& other);
	ItemInstance(ItemInstance&& other) noexcept;

	ItemInstance& operator=(const ItemInstance& other);
	ItemInstance& operator=(ItemInstance&& other) noexcept;

	unsigned int getID() const;
	unsigned int getStackSize() const;

	void setID(unsigned int newID);
	void setStackSize(unsigned int newStackSize);

	friend void swap(ItemInstance& first, ItemInstance& second) noexcept;
private:
	unsigned int _id;
	unsigned int _stackSize;
};
