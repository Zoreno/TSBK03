#pragma once
#include <string>
#include "ItemInstance.h"
#include <vector>
#include "LootGeneratorTableEntry.h"

class LootGenerator;

class LootGeneratorTableEntryTable : public LootGeneratorTableEntry
{
public:
	LootGeneratorTableEntryTable(const std::string& tableTag, int probability, int minCount = 1, int maxCount = 1);

	void generateLoot(LootGenerator *generator, std::vector<ItemInstance>& result) override;

	~LootGeneratorTableEntryTable() override;

	std::string getTableTag() const;
private:

	std::string _tableTag;
};
