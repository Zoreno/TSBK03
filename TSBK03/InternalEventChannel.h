#pragma once

#include "InternalEventChannelBase.h"

#include <type_traits>
#include "Event.h"
#include "Subscriber.h"

#include <vector>
#include <queue>

template <typename T>
class InternalEventChannel : public InternalEventChannelBase
{
public:

	InternalEventChannel();

	~InternalEventChannel() override;

	typename std::enable_if<std::is_base_of<Event, T>::value>::type
		addSubscriber(Subscriber<T> *sub);

	typename std::enable_if<std::is_base_of<Event, T>::value>::type
		removeSubscriber(Subscriber<T> *sub);

	typename std::enable_if<std::is_base_of<Event, T>::value>::type
		postEvent(const T& ev);

	void handleEvents() override;

	unsigned int getEventCount() const override;
private:

	std::vector<Subscriber<T> *> _subscribers{};

	std::queue<T> _eventQueue{};
};

template <typename T>
InternalEventChannel<T>::InternalEventChannel()
{
	
}

template <typename T>
InternalEventChannel<T>::~InternalEventChannel()
{
	
}

template <typename T>
typename std::enable_if<std::is_base_of<Event, T>::value>::type
InternalEventChannel<T>::addSubscriber(
	Subscriber<T> *sub)
{
	_subscribers.push_back(sub);
}

template <typename T>
typename std::enable_if<std::is_base_of<Event, T>::value>::type
InternalEventChannel<T>::removeSubscriber(
	Subscriber<T> *sub)
{
	for(auto it = _subscribers.begin(); it != _subscribers.end(); ++it)
	{
		if(*it == sub)
		{
			_subscribers.erase(it);
			break;
		}
	}
}

template <typename T>
typename std::enable_if<std::is_base_of<Event, T>::value>::type
InternalEventChannel<T>::postEvent(
	const T &ev)
{
	_eventQueue.push(ev);
}

template <typename T>
void InternalEventChannel<T>::handleEvents()
{
	while (!_eventQueue.empty())
	{
		T ev = _eventQueue.front();
		_eventQueue.pop();
		for (auto sub : _subscribers)
		{
			sub->handleEvent(ev);
		}
	}
}

template <typename T>
unsigned int InternalEventChannel<T>::getEventCount() const
{
	return static_cast<unsigned int>(_eventQueue.size());
}
