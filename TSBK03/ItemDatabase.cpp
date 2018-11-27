#include "ItemDatabase.h"
#include <rapidxml/rapidxml.hpp>
#include <iostream>
#include "Utils.h"
#include "ConsumableItem.h"

#include "Game.h"
#include "Player.h"
#include "ItemSlot.h"
#include "EquippableItem.h"

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
		else if (strcmp(typestring, "EQUIPPABLE") == 0)
		{
			type = ItemType::EQUIPPABLE;

			// Parse stats
			Stats stats{};
			xml_node<> *statsNode = itemnode->first_node("stats");

			if(statsNode->first_node("agility") != nullptr)
			{
				stats.agility = std::strtol(statsNode->first_node("agility")->value(), nullptr, 10);
			}

			if (statsNode->first_node("strength") != nullptr)
			{
				stats.strength = std::strtol(statsNode->first_node("strength")->value(), nullptr, 10);
			}

			if (statsNode->first_node("stamina") != nullptr)
			{
				stats.stamina = std::strtol(statsNode->first_node("stamina")->value(), nullptr, 10);
			}

			if (statsNode->first_node("intellect") != nullptr)
			{
				stats.intellect = std::strtol(statsNode->first_node("intellect")->value(), nullptr, 10);
			}

			if (statsNode->first_node("spirit") != nullptr)
			{
				stats.spirit = std::strtol(statsNode->first_node("spirit")->value(), nullptr, 10);
			}

			// Parse item slot

			const char *itemSlotString = itemnode->first_node("itemslot")->value();

			ItemSlot slot;

			if(strcmp(itemSlotString, "HEAD") == 0)
			{
				slot = ItemSlot::HEAD;
			}
			else if (strcmp(itemSlotString, "NECK") == 0)
			{
				slot = ItemSlot::NECK;
			}
			else if (strcmp(itemSlotString, "SHOULDER") == 0)
			{
				slot = ItemSlot::SHOULDER;
			}
			else if (strcmp(itemSlotString, "BACK") == 0)
			{
				slot = ItemSlot::BACK;
			}
			else if (strcmp(itemSlotString, "CHEST") == 0)
			{
				slot = ItemSlot::CHEST;
			}
			else if (strcmp(itemSlotString, "WRIST") == 0)
			{
				slot = ItemSlot::WRIST;
			}
			else if (strcmp(itemSlotString, "GLOVES") == 0)
			{
				slot = ItemSlot::GLOVES;
			}
			else if (strcmp(itemSlotString, "WAIST") == 0)
			{
				slot = ItemSlot::WAIST;
			}
			else if (strcmp(itemSlotString, "LEGS") == 0)
			{
				slot = ItemSlot::LEGS;
			}
			else if (strcmp(itemSlotString, "FEET") == 0)
			{
				slot = ItemSlot::FEET;
			}
			else if (strcmp(itemSlotString, "RING") == 0)
			{
				slot = ItemSlot::RING;
			}
			else if (strcmp(itemSlotString, "WEAPON") == 0)
			{
				slot = ItemSlot::WEAPON;
			}
			else if (strcmp(itemSlotString, "SHILED") == 0)
			{
				slot = ItemSlot::SHIELD;
			}
			else if (strcmp(itemSlotString, "RANGEDWEAPON") == 0)
			{
				slot = ItemSlot::RANGEDWEAPON;
			}
			else
			{
				throw std::invalid_argument(std::string{ "Invalid item slot for item " } +std::to_string(id));
			}

			int requiredLevel = 1;

			xml_node<> *reqlevelNode = itemnode->first_node("requiredlevel");

			if(reqlevelNode != nullptr)
			{
				requiredLevel = std::strtol(reqlevelNode->value(), nullptr, 10);
			}

			_items.emplace(
				id,
				new EquippableItem(
					id,
					name,
					type,
					rarity,
					description,
					maxstacksize,
					iconX,
					iconY,
					stats,
					slot,
					requiredLevel));
		}
		else
		{
			throw std::invalid_argument(std::string{ "Invalid type descriptor for item " } +std::to_string(id));
		}

		++items_read;

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