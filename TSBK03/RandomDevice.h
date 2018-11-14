#pragma once

#include <cstdint>

class RandomDevice
{
public:
	virtual ~RandomDevice();

	virtual void initialize(uint32_t seed) = 0;
	virtual void destroy() = 0;
	virtual uint32_t extract() = 0;
};

inline RandomDevice::~RandomDevice() {}
