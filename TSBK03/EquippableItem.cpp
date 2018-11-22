#include "EquippableItem.h"

EquippableItem::EquippableItem(
	unsigned int id,
	const std::string &name,
	ItemType type,
	ItemRarity rarity,
	const std::string &description,
	unsigned int maxStackSize,
	unsigned int iconX,
	unsigned int iconY,
	Stats stats,
	ItemSlot itemSlot,
	unsigned int requiredLevel)
	:Item(id, name, type, rarity, description, maxStackSize, iconX, iconY),
	_stats{ stats },
	_itemSlot{ itemSlot },
	_requiredLevel{ requiredLevel }
{

}

const Stats & EquippableItem::getStats() const
{
	return _stats;
}

ItemSlot EquippableItem::getItemSlot() const
{
	return _itemSlot;
}

unsigned int EquippableItem::getRequiredLevel() const
{
	return _requiredLevel;
}
