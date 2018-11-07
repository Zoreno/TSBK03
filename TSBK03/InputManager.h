#pragma once

#include "Subscriber.h"
#include "MouseMovedEvent.h"
#include "MouseButtonEvent.h"
#include "KeyEvent.h"

class InputManager : public Subscriber<MouseMovedEvent>,
                     public Subscriber<MouseButtonEvent>,
                     public Subscriber<KeyEvent>
{
public:

	void handleEvent(const MouseMovedEvent& ev) override;
	void handleEvent(const MouseButtonEvent &ev) override;
	void handleEvent(const KeyEvent &ev) override;

	bool rightClick;
	bool leftClick;

	float mouseX;
	float mouseY;

	bool wKey;
	bool aKey;
	bool sKey;
	bool dKey;
};
