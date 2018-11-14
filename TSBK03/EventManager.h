#pragma once

#include "InternalEventChannel.h"
#include "Subscriber.h"
#include "Event.h"

#include <map>
#include <typeindex>

class EventManager
{
public:
	EventManager();

	~EventManager();

	template <typename T>
	typename std::enable_if<std::is_base_of<Event, T>::value>::type
		addSubscriber(Subscriber<T>* sub);

	template <typename T>
	typename std::enable_if<std::is_base_of<Event, T>::value>::type
		removeSubscriber(Subscriber<T>* sub);

	template <typename T>
	typename std::enable_if<std::is_base_of<Event, T>::value>::type
		postEvent(const T& ev);

	void handleEvents();

private:

	typedef size_t EventHash;

	using Entry = std::pair<EventHash, InternalEventChannelBase *>;
	using Table = std::map<EventHash, InternalEventChannelBase *>;

	template <typename T>
	typename std::enable_if<std::is_base_of<Event, T>::value, EventHash>::type
		hash() const;

	template <typename T>
	typename std::enable_if<std::is_base_of<Event, T>::value, InternalEventChannel<T>*>::type
		getInternalChannel();

	Table _channels;
};

template <typename T>
typename std::enable_if<std::is_base_of<Event, T>::value>::type
EventManager::addSubscriber(
	Subscriber<T> *sub)
{
	getInternalChannel<T>()->addSubscriber(sub);
}

template <typename T>
typename std::enable_if<std::is_base_of<Event, T>::value>::type
EventManager::removeSubscriber(
	Subscriber<T> *sub)
{
	getInternalChannel<T>()->removeSubscriber(sub);
}

template <typename T>
typename std::enable_if<std::is_base_of<Event, T>::value>::type
EventManager::postEvent(
	const T &ev)
{
	getInternalChannel<T>()->postEvent(ev);
}

template <typename T>
typename std::enable_if<std::is_base_of<Event, T>::value, EventManager::EventHash>::type
EventManager::hash() const
{
	return std::type_index{ typeid(T) }.hash_code();
}

template <typename T>
typename std::enable_if<std::is_base_of<Event, T>::value, InternalEventChannel<T> *>::type
EventManager::getInternalChannel()
{
	Table::iterator it = _channels.find(hash<T>());

	if (it == _channels.end())
	{
		it = _channels.insert(Entry(hash<T>(), new InternalEventChannel<T>())).first;
	}

	return dynamic_cast<InternalEventChannel<T> *>(it->second);
}