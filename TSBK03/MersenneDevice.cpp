#include "MersenneDevice.h"

void MersenneDevice::initialize(
	uint32_t seed)
{
	_mt[0] = seed;

	for (uint32_t i = 1; i < N; ++i)
	{
		_mt[i] = (F * (_mt[i - 1] ^ (_mt[i - 1] >> 30)) + i);
	}

	_index = N;
}

void MersenneDevice::destroy()
{

}

uint32_t MersenneDevice::extract()
{
	int i = _index;

	if (_index >= N)
	{
		twist();
		i = _index;
	}

	uint32_t y = _mt[i];

	_index = i + 1;

	y ^= (_mt[i] >> U);
	y ^= (y << S) & B;
	y ^= (y << T) & C;
	y ^= (y >> L);

	return y;
}

void MersenneDevice::twist()
{
	for (uint32_t i = 0; i < N; ++i)
	{
		uint32_t x = (_mt[i] & MASK_UPPER) + (_mt[(i + 1) % N] & MASK_LOWER);

		uint32_t xA = x >> 1;

		if (x & 0x1)
		{
			xA ^= A;
		}

		_mt[i] = _mt[(i + M) % N] ^ xA;
	}

	_index = 0;
}
