#include "LootGeneratorTableEntryItem.h"

#include "LootGenerator.h"

LootGeneratorTableEntryItem::LootGeneratorTableEntryItem(
	int itemid,
	int probability,
	int minCount,
	int maxCount)
	:LootGeneratorTableEntry{ probability, minCount, maxCount },
	_itemId{ itemid }
{

}

void LootGeneratorTableEntryItem::generateLoot(
	LootGenerator *generator,
	std::vector<ItemInstance> &result)
{
	int itemcount;

	if (_minCount != _maxCount)
	{
		itemcount = generator->getRandomGenerator()->randInt32Range(_minCount, _maxCount + 1);
	}
	else
	{
		itemcount = _minCount;
	}

	result.emplace_back(_itemId, itemcount);
}

LootGeneratorTableEntryItem::~LootGeneratorTableEntryItem()
{

}

int LootGeneratorTableEntryItem::getItemId() const
{
	return _itemId;
}
