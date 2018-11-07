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
	if(ev.action == GLFW_PRESS)
	{
		switch(ev.button)
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

void InputManager::handleEvent(
	const KeyEvent &ev)
{
	if (ev.action == GLFW_PRESS)
	{
		switch (ev.key)
		{
		case GLFW_KEY_W:
			wKey = true;
			break;
		case GLFW_KEY_A:
			aKey = true;
			break;
		case GLFW_KEY_S:
			sKey = true;
			break;
		case GLFW_KEY_D:
			dKey = true;
			break;
		default:
			break;
		}
	}
	else if (ev.action == GLFW_RELEASE)
	{
		switch (ev.key)
		{
		case GLFW_KEY_W:
			wKey = false;
			break;
		case GLFW_KEY_A:
			aKey = false;
			break;
		case GLFW_KEY_S:
			sKey = false;
			break;
		case GLFW_KEY_D:
			dKey = false;
			break;
		default:
			break;
		}
	}
}
