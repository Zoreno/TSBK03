#pragma once

#include "Item.h"

class ConsumableItem : public Item
{
public:
	ConsumableItem(
		unsigned int id,
		const std::string& name,
		ItemType type,
		ItemRarity rarity,
		const std::string& description,
		const std::string& onUseFunction,
		const std::string& onUseText,
		unsigned int maxStackSize = 1,
		unsigned int iconX = 0,
		unsigned int iconY = 0);

	const std::string& getUseFunction() const;
	const std::string& getUseText() const;
protected:

	std::string _onUseFunction;
	std::string _onUseText;
};