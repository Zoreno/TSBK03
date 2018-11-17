#include "ConsumableItem.h"

ConsumableItem::ConsumableItem(
	unsigned int id,
	const std::string &name,
	ItemType type,
	ItemRarity rarity,
	const std::string &description,
	const std::string& onUseFunction,
	const std::string& onUseText,
	unsigned int maxStackSize,
	unsigned int iconX,
	unsigned int iconY)
	:Item(id, name, type, rarity, description, maxStackSize, iconX, iconY),
	_onUseFunction{ onUseFunction }, _onUseText{onUseText}
{

}

const std::string & ConsumableItem::getUseFunction() const
{
	return _onUseFunction;
}

const std::string & ConsumableItem::getUseText() const
{
	return _onUseText;
}
