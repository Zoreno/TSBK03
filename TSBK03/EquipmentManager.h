#pragma once
#include "ItemInstance.h"
#include "ItemSlot.h"
#include "EquippableItem.h"

class Game;

class EquipmentManager
{
public:
	explicit EquipmentManager(Game *game);

	~EquipmentManager();

	bool equip(ItemInstance itemInstance, unsigned int invPos);
	bool unequip(ItemSlot slot);

	const ItemInstance& getItemAt(ItemSlot slot) const;

	Stats getStats() const;
private:
	ItemInstance _items[static_cast<unsigned int>(ItemSlot::COUNT)];

	Game *_game;
};
