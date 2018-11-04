#pragma once

#include <type_traits>

#include "Event.h"

template <typename T>
class Subscriber
{
public:

	Subscriber() = default;

	virtual ~Subscriber();

	virtual
		typename std::enable_if<std::is_base_of<Event, T>::value>::type
		handleEvent(const T& ev) = 0;
};

template <typename T>
Subscriber<T>::~Subscriber()
{
	
}
