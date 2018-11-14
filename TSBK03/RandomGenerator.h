#pragma once

#include <cstdint>
#include <ctime>

template <typename Device>
class RandomGenerator
{
public:
	explicit RandomGenerator(uint32_t seed);
	RandomGenerator();

	RandomGenerator(const RandomGenerator& other) = delete;
	RandomGenerator(RandomGenerator&& other) = delete;

	RandomGenerator& operator=(const RandomGenerator& other) = delete;
	RandomGenerator& operator=(RandomGenerator&& other) = delete;

	~RandomGenerator();

	uint64_t randUint64();
	uint64_t randUint64Range(uint64_t min, uint64_t max);

	uint32_t randUint32();
	uint32_t randUint32Range(uint32_t min, uint32_t max);

	uint32_t randInt64();
	uint32_t randInt64Range(int64_t min, int64_t max);

	int32_t randInt32();
	int32_t randInt32Range(int32_t min, int32_t max);

	double randDouble();
	double randDoubleRange(double min, double max);
	double randDoubleSigned();

	float randFloat();
	float randFloatRange(float min, float max);
	float randFloatSigned();
private:
	Device device;

	uint32_t extract();
};

template <typename Device>
RandomGenerator<Device>::RandomGenerator(
	uint32_t seed)
{
	device.initialize(seed);
}

template <typename Device>
RandomGenerator<Device>::RandomGenerator()
{
	device.initialize(static_cast<uint32_t>(time(nullptr)));
}

template <typename Device>
RandomGenerator<Device>::~RandomGenerator()
{
	device.destroy();
}

template <typename Device>
uint64_t RandomGenerator<Device>::randUint64()
{
	uint32_t lower = extract();
	uint32_t upper = extract();

	return lower | (static_cast<uint64_t>(upper) << 32);
}

template <typename Device>
uint64_t RandomGenerator<Device>::randUint64Range(
	uint64_t min,
	uint64_t max)
{
	uint64_t range = max - min;

	return min + randUint64() % range;
}

template <typename Device>
uint32_t RandomGenerator<Device>::randUint32()
{
	return extract();
}

template <typename Device>
uint32_t RandomGenerator<Device>::randUint32Range(
	uint32_t min,
	uint32_t max)
{
	uint32_t range = max - min;

	return min + randUint32() % range;
}

template <typename Device>
uint32_t RandomGenerator<Device>::randInt64()
{
	uint64_t num = randUint64();

	uint64_t sign = num & 0x8000000000000000ULL;

	if(sign)
	{
		return -static_cast<int64_t>(num & 0x7FFFFFFFFFFFFFFFULL);
	}

	return static_cast<int64_t>(num & 0x7FFFFFFFFFFFFFFFULL);
}

template <typename Device>
uint32_t RandomGenerator<Device>::randInt64Range(
	int64_t min,
	int64_t max)
{
	int64_t range = max > min ? max - min : min - max;

	return min + randInt64() % range;
}

template <typename Device>
int32_t RandomGenerator<Device>::randInt32()
{
	uint32_t num = extract();

	uint32_t sign = num & 0x80000000;

	if (sign)
	{
		return -static_cast<int32_t>(num & 0x7FFFFFFF);
	}

	return static_cast<int32_t>(num & 0x7FFFFFFF);
}

template <typename Device>
int32_t RandomGenerator<Device>::randInt32Range(
	int32_t min,
	int32_t max)
{
	int32_t range = max > min ? max - min : min - max;

	return min + randInt32() % range;
}

template <typename Device>
double RandomGenerator<Device>::randDouble()
{
	uint32_t a = randUint32() >> 5;
	uint32_t b = randUint32() >> 6;

	return(a*67108864.0 + b)*(1.0 / 9007199254740992.0);
}

template <typename Device>
double RandomGenerator<Device>::randDoubleRange(
	double min,
	double max)
{
	double range = max - min;

	return min + randDouble() * range;
}

template <typename Device>
double RandomGenerator<Device>::randDoubleSigned()
{
	return randDoubleRange(-1.0, 1.0);
}

template <typename Device>
float RandomGenerator<Device>::randFloat()
{
	return static_cast<float>(randDouble());
}

template <typename Device>
float RandomGenerator<Device>::randFloatRange(
	float min,
	float max)
{
	float range = max - min;

	return min + randFloat() * range;
}

template <typename Device>
float RandomGenerator<Device>::randFloatSigned()
{
	return randFloatRange(-1.f, 1.f);
}

template <typename Device>
uint32_t RandomGenerator<Device>::extract()
{
	return device.extract();
}
