#pragma once

#include "RandomDevice.h"

class MersenneDevice : public RandomDevice
{
public:
	MersenneDevice() = default;

	void initialize(uint32_t seed) override;
	void destroy() override;
	uint32_t extract() override;

private:

	void twist();

	static constexpr uint32_t N{ 624 };
	static constexpr uint32_t M{ 397 };
	static constexpr uint32_t R{ 31 };

	static constexpr uint32_t A{ 0x9908B0DF };

	static constexpr uint32_t F{ 1812433253 };

	static constexpr uint32_t U{ 11 };

	static constexpr uint32_t S{ 7 };
	static constexpr uint32_t B{ 0x9D2C5680 };

	static constexpr uint32_t T{ 15 };
	static constexpr uint32_t C{ 0xEFC60000 };

	static constexpr uint32_t L{ 18 };

	static constexpr uint64_t MASK_LOWER = (1ull << R) - 1;
	static constexpr uint64_t MASK_UPPER = (1ull << R);

	uint32_t _mt[N];
	uint16_t _index;
};