#pragma once

#include "Event.h"

class KeyEvent : public Event
{
public:
	int key;

	int action;
};