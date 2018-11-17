#pragma once
#include "ItemInstance.h"
#include <vector>
#include "LootGeneratorTableEntry.h"

class LootGenerator;

class LootGeneratorTableEntryItem : public LootGeneratorTableEntry
{
public:
	LootGeneratorTableEntryItem(int itemid, int probability, int minCount = 1, int maxCount = 1);

	void generateLoot(LootGenerator *generator, std::vector<ItemInstance>& result) override;

	~LootGeneratorTableEntryItem() override;

	int getItemId() const;
private:
	int _itemId;
};