#pragma once

#include "Event.h"

class MouseMovedEvent : public Event
{
public:
	float x;
	float y;
};