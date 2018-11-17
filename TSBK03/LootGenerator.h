#pragma once

#include <map>
#include <string>
#include <vector>
#include "ItemInstance.h"

#include <rapidxml/rapidxml.hpp>

#include "LootGeneratorTableEntry.h"
#include "LootGeneratorTable.h"
#include "MersenneDevice.h"
#include "RandomGenerator.h"

class LootGenerator
{
public:
	LootGenerator();

	void generateLoot(const std::string& tableTag, int count, std::vector<ItemInstance>& result);

	~LootGenerator();

	RandomGenerator<MersenneDevice> *getRandomGenerator();

	LootGeneratorTable *getTable(const std::string& tableTag);
private: 

	RandomGenerator<MersenneDevice> _randomDevice;

	std::map<std::string, LootGeneratorTable *> _tables;
};
