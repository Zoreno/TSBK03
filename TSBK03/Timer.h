#pragma once

#include <GLFW/glfw3.h>

class Timer
{
public:
	Timer();

	float restart();
private:

	float _time;
};