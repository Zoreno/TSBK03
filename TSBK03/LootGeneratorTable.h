#pragma once
#include "LootGeneratorTableEntry.h"
#include <rapidxml/rapidxml.hpp>

class LootGenerator;

class LootGeneratorTable
{
public:
	explicit LootGeneratorTable(rapidxml::xml_node<> *tableNode);

	void generateLoot(LootGenerator *generator, int count, std::vector<ItemInstance>& result);

	~LootGeneratorTable();
private:
	std::vector<LootGeneratorTableEntry *> _alwaysTable;

	std::vector<LootGeneratorTableEntry *> _table;

	int _sum{ 0 };
};