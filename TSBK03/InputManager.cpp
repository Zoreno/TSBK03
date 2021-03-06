#include "InputManager.h"
#include <GLFW/glfw3.h>

void InputManager::handleEvent(
	const MouseMovedEvent &ev)
{
	mouseX = ev.x;
	mouseY = ev.y;
}

void InputManager::handleEvent(
	const MouseButtonEvent &ev)
{
	if (ev.action == GLFW_PRESS)
	{
		switch (ev.button)
		{
		case GLFW_MOUSE_BUTTON_1:
			leftClick = true;
			break;
		case GLFW_MOUSE_BUTTON_2:
			rightClick = true;
			break;
		default:
			// Unhandled key
			break;
		}
	}
	else if (ev.action == GLFW_RELEASE)
	{
		switch (ev.button)
		{
		case GLFW_MOUSE_BUTTON_1:
			leftClick = false;
			break;
		case GLFW_MOUSE_BUTTON_2:
			rightClick = false;
			break;
		default:
			// Unhandled key
			break;
		}
	}
}

#define KEY_ENTRY_CASE_PRESSED(key) \
case GLFW_##key: \
	keys[key] = true;\
	break;

#define KEY_ENTRY_CASE_RELEASED(key) \
case GLFW_##key: \
	keys[key] = false;\
	break;


void InputManager::handleEvent(
	const KeyEvent &ev)
{
	if (ev.action == GLFW_PRESS)
	{
		switch (ev.key)
		{
			KEY_ENTRY_CASE_PRESSED(KEY_A);
			KEY_ENTRY_CASE_PRESSED(KEY_B);
			KEY_ENTRY_CASE_PRESSED(KEY_C);
			KEY_ENTRY_CASE_PRESSED(KEY_D);
			KEY_ENTRY_CASE_PRESSED(KEY_E);
			KEY_ENTRY_CASE_PRESSED(KEY_F);
			KEY_ENTRY_CASE_PRESSED(KEY_G);
			KEY_ENTRY_CASE_PRESSED(KEY_H);
			KEY_ENTRY_CASE_PRESSED(KEY_I);
			KEY_ENTRY_CASE_PRESSED(KEY_J);
			KEY_ENTRY_CASE_PRESSED(KEY_K);
			KEY_ENTRY_CASE_PRESSED(KEY_L);
			KEY_ENTRY_CASE_PRESSED(KEY_M);
			KEY_ENTRY_CASE_PRESSED(KEY_N);
			KEY_ENTRY_CASE_PRESSED(KEY_O);
			KEY_ENTRY_CASE_PRESSED(KEY_P);
			KEY_ENTRY_CASE_PRESSED(KEY_Q);
			KEY_ENTRY_CASE_PRESSED(KEY_R);
			KEY_ENTRY_CASE_PRESSED(KEY_S);
			KEY_ENTRY_CASE_PRESSED(KEY_T);
			KEY_ENTRY_CASE_PRESSED(KEY_U);
			KEY_ENTRY_CASE_PRESSED(KEY_V);
			KEY_ENTRY_CASE_PRESSED(KEY_W);
			KEY_ENTRY_CASE_PRESSED(KEY_X);
			KEY_ENTRY_CASE_PRESSED(KEY_Y);
			KEY_ENTRY_CASE_PRESSED(KEY_Z);
			KEY_ENTRY_CASE_PRESSED(KEY_0);
			KEY_ENTRY_CASE_PRESSED(KEY_1);
			KEY_ENTRY_CASE_PRESSED(KEY_2);
			KEY_ENTRY_CASE_PRESSED(KEY_3);
			KEY_ENTRY_CASE_PRESSED(KEY_4);
			KEY_ENTRY_CASE_PRESSED(KEY_5);
			KEY_ENTRY_CASE_PRESSED(KEY_6);
			KEY_ENTRY_CASE_PRESSED(KEY_7);
			KEY_ENTRY_CASE_PRESSED(KEY_8);
			KEY_ENTRY_CASE_PRESSED(KEY_9);
			KEY_ENTRY_CASE_PRESSED(KEY_F1);
			KEY_ENTRY_CASE_PRESSED(KEY_F2);
			KEY_ENTRY_CASE_PRESSED(KEY_F3);
			KEY_ENTRY_CASE_PRESSED(KEY_F4);
			KEY_ENTRY_CASE_PRESSED(KEY_F5);
			KEY_ENTRY_CASE_PRESSED(KEY_F6);
			KEY_ENTRY_CASE_PRESSED(KEY_F7);
			KEY_ENTRY_CASE_PRESSED(KEY_F8);
			KEY_ENTRY_CASE_PRESSED(KEY_F9);
			KEY_ENTRY_CASE_PRESSED(KEY_F10);
			KEY_ENTRY_CASE_PRESSED(KEY_F11);
			KEY_ENTRY_CASE_PRESSED(KEY_F12);
			KEY_ENTRY_CASE_PRESSED(KEY_SPACE);
			KEY_ENTRY_CASE_PRESSED(KEY_ESCAPE);
			KEY_ENTRY_CASE_PRESSED(KEY_ENTER);
			KEY_ENTRY_CASE_PRESSED(KEY_TAB);
			KEY_ENTRY_CASE_PRESSED(KEY_UP);
			KEY_ENTRY_CASE_PRESSED(KEY_DOWN);
			KEY_ENTRY_CASE_PRESSED(KEY_RIGHT);
			KEY_ENTRY_CASE_PRESSED(KEY_LEFT);
			KEY_ENTRY_CASE_PRESSED(KEY_INSERT);
			KEY_ENTRY_CASE_PRESSED(KEY_DELETE);
			KEY_ENTRY_CASE_PRESSED(KEY_HOME);
			KEY_ENTRY_CASE_PRESSED(KEY_END);
			KEY_ENTRY_CASE_PRESSED(KEY_PAGE_UP);
			KEY_ENTRY_CASE_PRESSED(KEY_PAGE_DOWN);
		default:
			break;
		}
	}
	else if (ev.action == GLFW_RELEASE)
	{
		switch (ev.key)
		{
			KEY_ENTRY_CASE_RELEASED(KEY_A);
			KEY_ENTRY_CASE_RELEASED(KEY_B);
			KEY_ENTRY_CASE_RELEASED(KEY_C);
			KEY_ENTRY_CASE_RELEASED(KEY_D);
			KEY_ENTRY_CASE_RELEASED(KEY_E);
			KEY_ENTRY_CASE_RELEASED(KEY_F);
			KEY_ENTRY_CASE_RELEASED(KEY_G);
			KEY_ENTRY_CASE_RELEASED(KEY_H);
			KEY_ENTRY_CASE_RELEASED(KEY_I);
			KEY_ENTRY_CASE_RELEASED(KEY_J);
			KEY_ENTRY_CASE_RELEASED(KEY_K);
			KEY_ENTRY_CASE_RELEASED(KEY_L);
			KEY_ENTRY_CASE_RELEASED(KEY_M);
			KEY_ENTRY_CASE_RELEASED(KEY_N);
			KEY_ENTRY_CASE_RELEASED(KEY_O);
			KEY_ENTRY_CASE_RELEASED(KEY_P);
			KEY_ENTRY_CASE_RELEASED(KEY_Q);
			KEY_ENTRY_CASE_RELEASED(KEY_R);
			KEY_ENTRY_CASE_RELEASED(KEY_S);
			KEY_ENTRY_CASE_RELEASED(KEY_T);
			KEY_ENTRY_CASE_RELEASED(KEY_U);
			KEY_ENTRY_CASE_RELEASED(KEY_V);
			KEY_ENTRY_CASE_RELEASED(KEY_W);
			KEY_ENTRY_CASE_RELEASED(KEY_X);
			KEY_ENTRY_CASE_RELEASED(KEY_Y);
			KEY_ENTRY_CASE_RELEASED(KEY_Z);
			KEY_ENTRY_CASE_RELEASED(KEY_0);
			KEY_ENTRY_CASE_RELEASED(KEY_1);
			KEY_ENTRY_CASE_RELEASED(KEY_2);
			KEY_ENTRY_CASE_RELEASED(KEY_3);
			KEY_ENTRY_CASE_RELEASED(KEY_4);
			KEY_ENTRY_CASE_RELEASED(KEY_5);
			KEY_ENTRY_CASE_RELEASED(KEY_6);
			KEY_ENTRY_CASE_RELEASED(KEY_7);
			KEY_ENTRY_CASE_RELEASED(KEY_8);
			KEY_ENTRY_CASE_RELEASED(KEY_9);
			KEY_ENTRY_CASE_RELEASED(KEY_F1);
			KEY_ENTRY_CASE_RELEASED(KEY_F2);
			KEY_ENTRY_CASE_RELEASED(KEY_F3);
			KEY_ENTRY_CASE_RELEASED(KEY_F4);
			KEY_ENTRY_CASE_RELEASED(KEY_F5);
			KEY_ENTRY_CASE_RELEASED(KEY_F6);
			KEY_ENTRY_CASE_RELEASED(KEY_F7);
			KEY_ENTRY_CASE_RELEASED(KEY_F8);
			KEY_ENTRY_CASE_RELEASED(KEY_F9);
			KEY_ENTRY_CASE_RELEASED(KEY_F10);
			KEY_ENTRY_CASE_RELEASED(KEY_F11);
			KEY_ENTRY_CASE_RELEASED(KEY_F12);
			KEY_ENTRY_CASE_RELEASED(KEY_SPACE);
			KEY_ENTRY_CASE_RELEASED(KEY_ESCAPE);
			KEY_ENTRY_CASE_RELEASED(KEY_ENTER);
			KEY_ENTRY_CASE_RELEASED(KEY_TAB);
			KEY_ENTRY_CASE_RELEASED(KEY_UP);
			KEY_ENTRY_CASE_RELEASED(KEY_DOWN);
			KEY_ENTRY_CASE_RELEASED(KEY_RIGHT);
			KEY_ENTRY_CASE_RELEASED(KEY_LEFT);
			KEY_ENTRY_CASE_RELEASED(KEY_INSERT);
			KEY_ENTRY_CASE_RELEASED(KEY_DELETE);
			KEY_ENTRY_CASE_RELEASED(KEY_HOME);
			KEY_ENTRY_CASE_RELEASED(KEY_END);
			KEY_ENTRY_CASE_RELEASED(KEY_PAGE_UP);
			KEY_ENTRY_CASE_RELEASED(KEY_PAGE_DOWN);
		default:
			break;
		}
	}
}
