#include "Stats.h"

Stats Stats::operator+(
	const Stats &other) const
{
	Stats stats;

	stats.agility = agility + other.agility;
	stats.strength = strength + other.strength;
	stats.stamina = stamina + other.stamina;
	stats.intellect = intellect + other.intellect;
	stats.spirit = spirit + other.spirit;

	return stats;
}
