#pragma once

#include "Event.h"

class MouseButtonEvent : public Event
{
public:
	int button;
	int action;
	int mods;
};