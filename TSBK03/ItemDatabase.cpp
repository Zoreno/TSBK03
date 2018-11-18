#include "ItemDatabase.h"
#include <rapidxml/rapidxml.hpp>
#include <iostream>
#include "Utils.h"
#include "ConsumableItem.h"

#include "Game.h"
#include "Player.h"

ItemDatabase::ItemDatabase()
{
	loadItems();
	loadOnUseFunctions();
}

Item * ItemDatabase::getItemById(unsigned int id)
{
	auto it = _items.find(id);

	if (it != _items.end())
	{
		return it->second;
	}

	return nullptr;
}

bool ItemDatabase::invokeOnUseFunction(
	const std::string &onUseFunction,
	Game *game)
{
	auto it = _onUseFunctions.find(onUseFunction);

	if (it != _onUseFunctions.end())
	{
		return it->second(game);
	}

	throw std::invalid_argument{ "Invalid onUseFunction" };
}

void ItemDatabase::loadItems()
{
	using namespace rapidxml;
	xml_document<> doc;

	std::string itemFilePath = "items.xml";

	std::cout << "Parsing " + itemFilePath << "..." << std::endl;

	std::string file = getStringFromFile(itemFilePath);

	doc.parse<0>(const_cast<char *>(file.c_str()));

	xml_node<> *itemsnode = doc.first_node("items");

	xml_node<> *itemnode = itemsnode->first_node("item");

	unsigned int items_read = 0;

	while (itemnode != nullptr)
	{
		// TODO: Handle default cases and such for each of the data points
		unsigned int id = std::strtoul(itemnode->first_node("id")->value(), nullptr, 10);
		const char *name = itemnode->first_node("name")->value();
		const char *typestring = itemnode->first_node("type")->value();
		const char *raritystring = itemnode->first_node("rarity")->value();
		const char *description = itemnode->first_node("description")->value();
		unsigned int maxstacksize = std::strtoul(itemnode->first_node("maxstacksize")->value(), nullptr, 10);
		unsigned int iconX = std::strtoul(itemnode->first_node("iconX")->value(), nullptr, 10);
		unsigned int iconY = std::strtoul(itemnode->first_node("iconY")->value(), nullptr, 10);

		ItemType type;
		ItemRarity rarity;

		if (strcmp(raritystring, "COMMON") == 0)
		{
			rarity = ItemRarity::COMMON;
		}
		else if (strcmp(raritystring, "UNCOMMON") == 0)
		{
			rarity = ItemRarity::UNCOMMON;
		}
		else if (strcmp(raritystring, "SUPERIOR") == 0)
		{
			rarity = ItemRarity::SUPERIOR;
		}
		else if (strcmp(raritystring, "EPIC") == 0)
		{
			rarity = ItemRarity::EPIC;
		}
		else if (strcmp(raritystring, "LEGENDARY") == 0)
		{
			rarity = ItemRarity::LEGENDARY;
		}
		else
		{
			throw std::invalid_argument(std::string{ "Invalid rarity descriptor for item " } +std::to_string(id));
		}

		if (strcmp(typestring, "GENERIC") == 0)
		{
			type = ItemType::GENERIC;

			_items.emplace(
				id,
				new Item(
					id,
					name,
					type,
					rarity,
					description,
					maxstacksize, 
					iconX, 
					iconY));

			++items_read;
		}
		else if (strcmp(typestring, "CONSUMABLE") == 0)
		{
			type = ItemType::CONSUMABLE;

			const char *onUseFunction = itemnode->first_node("usefunction")->value();
			const char *onUseText = itemnode->first_node("usetext")->value();

			_items.emplace(
				id,
				new ConsumableItem(
					id,
					name,
					type,
					rarity,
					description,
					onUseFunction,
					onUseText,
					maxstacksize,
					iconX,
					iconY));
		}
		else
		{
			throw std::invalid_argument(std::string{ "Invalid type descriptor for item " } +std::to_string(id));
		}

		itemnode = itemnode->next_sibling("item");
	}

	std::cout << "Parsed " << items_read << " items" << std::endl;
}

void ItemDatabase::loadOnUseFunctions()
{
	// TODO: This may really benefit from some sort of scripting language for 
	// programming the callbacks, but in the meantime this will have to 
	// suffice.
	_onUseFunctions.emplace("greaterhealthpotion",
		[](Game *game) -> bool
	{
		if (game->getPlayer()->getMissingHealth() > 0)
		{
			game->getPlayer()->heal(20);

			return true;
		}

		return false;
	});
}