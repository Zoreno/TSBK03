#include "LootGenerator.h"

#include <rapidxml/rapidxml.hpp>
#include <iostream>
#include "Utils.h"

LootGenerator::LootGenerator()
	:_randomDevice{ time(NULL) }
{
	using namespace rapidxml;
	xml_document<> doc;

	std::string lootTableFilePath = "loottables.xml";

	std::cout << "Parsing " + lootTableFilePath << "..." << std::endl;

	std::string file = getStringFromFile(lootTableFilePath);

	doc.parse<0>(const_cast<char *>(file.c_str()));

	xml_node<> *tablenode = doc.first_node("table");

	int tablesRead = 0;

	while (tablenode != nullptr)
	{
		// TODO: Handle error
		const char *tag = tablenode->first_attribute("tag")->value();

		LootGeneratorTable *table = new LootGeneratorTable(tablenode);

		_tables.emplace(tag, table);

		++tablesRead;

		tablenode = tablenode->next_sibling("table");
	}

	std::cout << "Read " << tablesRead << " tables" << std::endl;
}

void LootGenerator::generateLoot(
	const std::string &tableTag,
	int count,
	std::vector<ItemInstance> &result)
{
	LootGeneratorTable *table = getTable(tableTag);

	table->generateLoot(this, count, result);
}

LootGenerator::~LootGenerator()
{
	for (auto it : _tables)
	{
		delete it.second;
	}
}

RandomGenerator<MersenneDevice> * LootGenerator::getRandomGenerator()
{
	return &_randomDevice;
}

LootGeneratorTable * LootGenerator::getTable(
	const std::string &tableTag)
{
	auto it = _tables.find(tableTag);

	if (it != _tables.end())
	{
		return it->second;
	}

	return nullptr;
}
