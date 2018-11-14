#include "EventManager.h"

EventManager::EventManager()
{
	
}

EventManager::~EventManager()
{
	while(!_channels.empty())
	{
		auto it = _channels.begin();

		delete it->second;
		_channels.erase(it);
	}
}

void EventManager::handleEvents()
{
	bool empty;

	do
	{
		empty = true;

		for (auto it : _channels)
		{
			if (it.second->getEventCount() > 0)
			{
				it.second->handleEvents();

				empty = false;
			}
		}
	} while (!empty);
}
