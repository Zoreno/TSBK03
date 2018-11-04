#include "Timer.h"

Timer::Timer()
{
	_time = static_cast<float>(glfwGetTime());
}

float Timer::restart()
{
	float oldTime = _time;

	_time = static_cast<float>(glfwGetTime());

	return _time - oldTime;
}
