#include "EquipmentManager.h"

#include "Player.h"
#include "Game.h"
#include "Inventory.h"
#include <numeric>

EquipmentManager::EquipmentManager(Game *game)
	:_game{ game }
{
	for (unsigned int i = 0; i < static_cast<unsigned int>(ItemSlot::COUNT); ++i)
	{
		_items[i] = ItemInstance{ INVALID_ID, 0 };
	}
}

EquipmentManager::~EquipmentManager()
{

}

bool EquipmentManager::equip(
	ItemInstance itemInstance,
	unsigned int invPos)
{
	EquippableItem *item = static_cast<EquippableItem *>(
		_game->getItemDatabase()->getItemById(itemInstance.getID()));

	if(item->getRequiredLevel() > _game->getPlayer()->getLevel())
	{
		return false;
	}

	ItemSlot slot = item->getItemSlot();

	Inventory *inv = _game->getPlayer()->getInventory();

	inv->removeItemAt(invPos);

	ItemInstance prevEquippedItem = _items[static_cast<unsigned int>(slot)];

	if (prevEquippedItem.getID() != INVALID_ID)
	{
		inv->addItem(prevEquippedItem);
	}

	_items[static_cast<unsigned int>(slot)] = itemInstance;

	_game->getPlayer()->recalculateStats();

	return true;
}

bool EquipmentManager::unequip(
	ItemSlot slot)
{
	Inventory *inv = _game->getPlayer()->getInventory();

	ItemInstance prevEquippedItem = _items[static_cast<unsigned int>(slot)];

	if(inv->addItem(prevEquippedItem) == 0)
	{
		_items[static_cast<unsigned int>(slot)] = ItemInstance(INVALID_ID, 0);

		_game->getPlayer()->recalculateStats();

		return true;
	}

	return false;
}

const ItemInstance & EquipmentManager::getItemAt(
	ItemSlot slot) const
{
	return _items[static_cast<unsigned int>(slot)];
}

Stats EquipmentManager::getStats() const
{
	ItemDatabase *database = _game->getItemDatabase();

	return std::accumulate(std::begin(_items), std::end(_items), Stats{},
		[database](const Stats& a, const ItemInstance& b)
	{
		if (b.getID() != INVALID_ID)
		{
			EquippableItem *item = static_cast<EquippableItem *>(database->getItemById(b.getID()));

			return a + item->getStats();
		}
		return a;
	});
}
