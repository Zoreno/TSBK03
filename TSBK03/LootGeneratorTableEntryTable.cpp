#include "LootGeneratorTableEntryTable.h"

#include "LootGenerator.h"

LootGeneratorTableEntryTable::LootGeneratorTableEntryTable(
	const std::string &tableTag,
	int probability,
	int minCount,
	int maxCount)
	:LootGeneratorTableEntry(probability, minCount, maxCount),
	_tableTag(tableTag)
{

}

void LootGeneratorTableEntryTable::generateLoot(
	LootGenerator *generator,
	std::vector<ItemInstance> &result)
{
	LootGeneratorTable *table = generator->getTable(_tableTag);

	int itemcount;

	if (_minCount != _maxCount)
	{
		itemcount = generator->getRandomGenerator()->randInt32Range(_minCount, _maxCount + 1);
	}
	else
	{
		itemcount = _minCount;
	}

	table->generateLoot(generator, itemcount, result);
}

LootGeneratorTableEntryTable::~LootGeneratorTableEntryTable()
{

}

std::string LootGeneratorTableEntryTable::getTableTag() const
{
	return _tableTag;
}
