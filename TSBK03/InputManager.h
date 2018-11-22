#pragma once

#include "Subscriber.h"
#include "MouseMovedEvent.h"
#include "MouseButtonEvent.h"
#include "KeyEvent.h"

enum Keys
{
	KEY_A = 0,
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

	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,

	KEY_SPACE,
	KEY_ESCAPE,
	KEY_ENTER,
	KEY_TAB,

	KEY_UP,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_LEFT,

	KEY_INSERT,
	KEY_DELETE,
	KEY_HOME,
	KEY_END,
	KEY_PAGE_UP,
	KEY_PAGE_DOWN,

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

	bool shift;
	bool ctrl;
	bool alt;

	bool dragging{ false };
};
