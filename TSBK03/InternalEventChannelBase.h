#pragma once

class InternalEventChannelBase
{
public:

	virtual void handleEvents() = 0;

	virtual unsigned int getEventCount() const = 0;

	virtual ~InternalEventChannelBase();
};