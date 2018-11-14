#pragma once

#include "Event.h"

class KeyEvent : public Event
{
public:
	int key;
	int scancode;
	int action;
	int mods;
};