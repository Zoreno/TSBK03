#pragma once
#include "Item.h"
#include "Stats.h"
#include "ItemSlot.h"

class EquippableItem : public Item
{
public:
	EquippableItem(
		unsigned int id,
		const std::string& name,
		ItemType type,
		ItemRarity rarity,
		const std::string& description,
		unsigned int maxStackSize,
		unsigned int iconX,
		unsigned int iconY,
		Stats stats,
		ItemSlot itemSlot,
		unsigned int requiredLevel);

	const Stats& getStats() const;
	ItemSlot getItemSlot() const;
	unsigned int getRequiredLevel() const;

private:

	Stats _stats;
	ItemSlot _itemSlot;
	unsigned int _requiredLevel;
};
