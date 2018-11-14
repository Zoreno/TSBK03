#include "Item.h"

Item::Item(
	unsigned int id,
	const std::string &name,
	ItemType type,
	ItemRarity rarity,
	const std::string &description,
	unsigned int maxStackSize,
	unsigned int iconX,
	unsigned int iconY)
	:_id{ id },
	_name{ name },
	_iconX{ iconX },
	_iconY{ iconY },
	_type{ type },
	_rarity{ rarity },
	_description{ description },
	_maxStackSize{ maxStackSize }
{

}

unsigned int Item::getID() const
{
	return _id;
}

const std::string & Item::getName() const
{
	return _name;
}

ItemType Item::getType() const
{
	return _type;
}

ItemRarity Item::getRarity() const
{
	return _rarity;
}

unsigned Item::getMaxStackSize() const
{
	return _maxStackSize;
}

unsigned Item::getIconX() const
{
	return _iconX;
}

unsigned Item::getIconY() const
{
	return _iconY;
}

const std::string & Item::getDescription() const
{
	return _description;
}
