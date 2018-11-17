#include "LootGeneratorTable.h"

#include "LootGenerator.h"

#include <iostream>
#include "LootGeneratorTableEntryItem.h"
#include "LootGeneratorTableEntryTable.h"
#include <numeric>

LootGeneratorTable::LootGeneratorTable(
	rapidxml::xml_node<> *tableNode)
{
	rapidxml::xml_node<> *entry = tableNode->first_node("entry");

	const char *tag = tableNode->first_attribute("tag")->value();

	std::cout << "Parsing table " << tag << std::endl;

	int entriesRead = 0;

	while (entry != nullptr)
	{
		bool isTable = false;
		bool isItem = false;

		bool always = false;

		int probablility{ 1 };
		int minCount{ 1 };
		int maxCount{ 1 };

		int itemid{ 0 };
		std::string grouptag{};

		rapidxml::xml_attribute<> *itemidattr = entry->first_attribute("itemid");
		rapidxml::xml_attribute<> *grouptagattr = entry->first_attribute("grouptag");
		rapidxml::xml_attribute<> *alwaysattr = entry->first_attribute("always");
		rapidxml::xml_attribute<> *probabilityattr = entry->first_attribute("probability");
		rapidxml::xml_attribute<> *mincountattr = entry->first_attribute("mincount");
		rapidxml::xml_attribute<> *maxcountattr = entry->first_attribute("maxcount");

		if ((itemidattr == nullptr) && (grouptagattr == nullptr))
		{
			// TODO: A more precise error message
			throw std::invalid_argument("Invalid entry. Entry must be either item or table");
		}

		if (itemidattr != nullptr)
		{
			itemid = std::strtol(itemidattr->value(), nullptr, 10);
			isItem = true;
		}

		if (grouptagattr != nullptr)
		{
			grouptag = grouptagattr->value();

			isTable = true;
		}

		if (alwaysattr != nullptr)
		{
			always = strcmp(alwaysattr->value(), "true") == 0;
		}

		if (probabilityattr != nullptr)
		{
			probablility = std::strtol(probabilityattr->value(), nullptr, 10);
		}

		if (mincountattr != nullptr)
		{
			minCount = std::strtol(mincountattr->value(), nullptr, 10);
		}

		if (maxcountattr != nullptr)
		{
			maxCount = std::strtol(maxcountattr->value(), nullptr, 10);
		}

		if (isItem)
		{
			if (always)
			{
				_alwaysTable.push_back(new LootGeneratorTableEntryItem(itemid, probablility, minCount, maxCount));
			}
			else
			{
				_table.push_back(new LootGeneratorTableEntryItem(itemid, probablility, minCount, maxCount));
			}
		}
		else if(isTable)
		{
			if (always)
			{
				_alwaysTable.push_back(new LootGeneratorTableEntryTable(grouptag, probablility, minCount, maxCount));
			}
			else
			{
				_table.push_back(new LootGeneratorTableEntryTable(grouptag, probablility, minCount, maxCount));
			}
		}

		++entriesRead;

		entry = entry->next_sibling("entry");
	}


	_sum = std::accumulate(_table.begin(), _table.end(), 0, [](const auto& init, const auto& elem) -> int { return init + elem->getProbability();});

	std::cout << _sum << std::endl;
	std::cout << "Read " << entriesRead << " entries" << std::endl;
}

void LootGeneratorTable::generateLoot(
	LootGenerator *generator,
	int count,
	std::vector<ItemInstance> &result)
{

	for (auto& it : _alwaysTable)
	{
		it->generateLoot(generator, result);

		--count;
	}

	for (int i = 0; i < count; ++i)
	{
		int accumulator = 0;

		int hitValue = generator->getRandomGenerator()->randInt32Range(0, _sum);

		for (auto& it : _table)
		{
			accumulator += it->getProbability();

			if (accumulator > hitValue)
			{
				it->generateLoot(generator, result);

				break;
			}
		}
	}
}

LootGeneratorTable::~LootGeneratorTable()
{
	for (auto it : _alwaysTable)
	{
		delete it;
	}

	for (auto it : _table)
	{
		delete it;
	}
}