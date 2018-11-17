#pragma once

#include <vector>
#include "ItemInstance.h"

class LootGenerator;

class LootGeneratorTableEntry
{
public:
	explicit LootGeneratorTableEntry(int probability, int minCount = 1, int maxCount = 1);

	virtual void generateLoot(LootGenerator *generator, std::vector<ItemInstance>& result) = 0;

	virtual ~LootGeneratorTableEntry();

	int getProbability() const;
	int getMinCount() const;
	int getMaxCount() const;
protected:
	int _probability{ 1 };

	int _minCount{ 1 };
	int _maxCount{ 1 };
};