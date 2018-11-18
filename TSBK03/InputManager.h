#pragma once

#include "Subscriber.h"
#include "MouseMovedEvent.h"
#include "MouseButtonEvent.h"
#include "KeyEvent.h"

enum Keys
{
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,

	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,

	KEY_TAB,

	KEY_COUNT
};

class InputManager :
	public Subscriber<MouseMovedEvent>,
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

	bool keys[KEY_COUNT];

	bool wKey;
	bool aKey;
	bool sKey;
	bool dKey;

	bool eKey;

	bool dragging{ false };
};
