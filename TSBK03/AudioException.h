#pragma once

#include <stdexcept>

class AudioException : public std::logic_error
{
public:
	using std::logic_error::logic_error;
};