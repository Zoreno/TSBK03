#include "LootGeneratorTableEntry.h"

LootGeneratorTableEntry::LootGeneratorTableEntry(
	int probability,
	int minCount,
	int maxCount)
	:_probability{ probability },
	_minCount{ minCount },
	_maxCount{ maxCount }
{

}

LootGeneratorTableEntry::~LootGeneratorTableEntry()
{

}

int LootGeneratorTableEntry::getProbability() const
{
	return _probability;
}

int LootGeneratorTableEntry::getMinCount() const
{
	return _minCount;
}

int LootGeneratorTableEntry::getMaxCount() const
{
	return _maxCount;
}
