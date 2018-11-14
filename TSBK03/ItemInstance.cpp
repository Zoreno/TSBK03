#include "ItemInstance.h"

ItemInstance::ItemInstance(
	unsigned int id,
	unsigned int stackSize)
	: _id{id},
	_stackSize{stackSize}
{
}

ItemInstance::ItemInstance(
	const ItemInstance &other)
	: _id{ other._id },
	_stackSize{ other._stackSize }
{
}

ItemInstance::ItemInstance(
	ItemInstance &&other) noexcept
{
	swap(*this, other);
}

ItemInstance & ItemInstance::operator=(
	const ItemInstance &other)
{
	_id = other._id;
	_stackSize = other._stackSize;

	return *this;
}

ItemInstance & ItemInstance::operator=(
	ItemInstance &&other) noexcept
{
	swap(*this, other);

	return *this;
}

unsigned int ItemInstance::getID() const
{
	return _id;
}

unsigned int ItemInstance::getStackSize() const
{
	return _stackSize;
}

void ItemInstance::setID(
	unsigned int newID)
{
	_id = newID;
}

void ItemInstance::setStackSize(
	unsigned int newStackSize)
{
	_stackSize = newStackSize;
}

void swap(
	ItemInstance &first,
	ItemInstance &second) noexcept
{
	std::swap(first._id, second._id);
	std::swap(first._stackSize, second._stackSize);
}
