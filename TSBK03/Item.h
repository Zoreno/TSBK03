#pragma once

#include <map>
#include <string>
#include <functional>
#include "ItemRarity.h"
#include "ItemType.h"

class Game;

typedef bool(*OnUseFunction)(Game *game);

class Item
{
public:
	Item(
		unsigned int id,
		const std::string& name,
		ItemType type,
		ItemRarity rarity,
		const std::string& description,
		unsigned int maxStackSize = 1,
		unsigned int iconX = 0,
		unsigned int iconY = 0);

	unsigned int getID() const;
	const std::string& getName() const;
	ItemType getType() const;
	ItemRarity getRarity() const;
	const std::string& getDescription() const;
	unsigned int getMaxStackSize() const;
	unsigned int getIconX() const;
	unsigned int getIconY() const;

protected:
	unsigned int _id;

	std::string _name;
	unsigned int _iconX;
	unsigned int _iconY;

	ItemType _type;
	ItemRarity _rarity;

	std::string _description;

	unsigned int _maxStackSize;
};