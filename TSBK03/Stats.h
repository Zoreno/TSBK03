#pragma once

struct Stats
{
	Stats() = default;

	Stats operator+(const Stats& other) const;

	int agility;
	int strength;
	int stamina;
	int intellect;
	int spirit;
};