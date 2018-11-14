/**
* @file		Color.cpp
* @Author	Joakim Bertils
* @date		2018-10-13
* @brief	Class representing a color.
*/

#include "Color.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

//=============================================================================
// Constructor
//=============================================================================

Color::Color()
	:_r{ MIN },
	_g{ MIN },
	_b{ MIN },
	_a{ MAX }
{

}

Color::Color(
	float grayScale,
	float a)
	:_r{ clamp(grayScale) },
	_g{ clamp(grayScale) },
	_b{ clamp(grayScale) },
	_a{ clamp(a) }
{

}

Color::Color(
	int grayScale,
	int a)
	:_r{ static_cast<float>(clamp(grayScale)) / static_cast<float>(INTMAX) },
	_g{ static_cast<float>(clamp(grayScale)) / static_cast<float>(INTMAX) },
	_b{ static_cast<float>(clamp(grayScale)) / static_cast<float>(INTMAX) },
	_a{ static_cast<float>(clamp(a)) / static_cast<float>(INTMAX) }
{
}

Color::Color(
	float r,
	float g,
	float b,
	float a)
	:_r{ clamp(r) },
	_g{ clamp(g) },
	_b{ clamp(b) },
	_a{ clamp(a) }
{

}

Color::Color(
	int r,
	int g,
	int b,
	int a)
	:_r{ static_cast<float>(clamp(r)) / static_cast<float>(INTMAX) },
	_g{ static_cast<float>(clamp(g)) / static_cast<float>(INTMAX) },
	_b{ static_cast<float>(clamp(b)) / static_cast<float>(INTMAX) },
	_a{ static_cast<float>(clamp(a)) / static_cast<float>(INTMAX) }
{

}

Color::Color(
	int *vec,
	bool hsl)
{
	if (!hsl)
	{
		_r = static_cast<float>(clamp(vec[0])) / static_cast<float>(INTMAX);
		_g = static_cast<float>(clamp(vec[1])) / static_cast<float>(INTMAX);
		_b = static_cast<float>(clamp(vec[2])) / static_cast<float>(INTMAX);
		_a = static_cast<float>(clamp(vec[3])) / static_cast<float>(INTMAX);
	}
	else
	{
		float h = static_cast<float>(clamp(vec[0])) / static_cast<float>(INTMAX);
		float s = static_cast<float>(clamp(vec[1])) / static_cast<float>(INTMAX);
		float l = static_cast<float>(clamp(vec[2])) / static_cast<float>(INTMAX);
		float a = static_cast<float>(clamp(vec[3])) / static_cast<float>(INTMAX);

		if (s == 0.f)
		{
			_r = l;
			_g = l;
			_b = l;
			_a = a;
		}
		else
		{
			float q = 0.f;

			if (l < 0.5f)
			{
				q = l * (1.f + s);
			}
			else
			{
				q = l + s - l * s;
			}

			float p = 2.f * l - q;

			_r = hueToRgb(p, q, h + 1.f / 3.f);
			_b = hueToRgb(p, q, h);
			_g = hueToRgb(p, q, h - 1.f / 3.f);
			_a = a;
		}
	}
}

Color::Color(
	float *vec,
	bool hsl)
{
	if (!hsl)
	{
		_r = clamp(vec[0]);
		_g = clamp(vec[1]);
		_b = clamp(vec[2]);
		_a = clamp(vec[3]);
	}
	else
	{
		float h = clamp(vec[0]);
		float s = clamp(vec[1]);
		float l = clamp(vec[2]);
		float a = clamp(vec[3]);

		if (s == 0.f)
		{
			_r = l;
			_g = l;
			_b = l;
			_a = a;
		}
		else
		{
			float q = 0.f;

			if (l < 0.5f)
			{
				q = l * (1.f + s);
			}
			else
			{
				q = l + s - l * s;
			}

			float p = 2.f * l - q;

			_r = hueToRgb(p, q, h + 1.f / 3.f);
			_b = hueToRgb(p, q, h);
			_g = hueToRgb(p, q, h - 1.f / 3.f);
			_a = a;
		}
	}
}

Color::Color(
	const std::string &str)
{
	std::string s{ str };

	// Check if the passed in string is empty.
	if (s.empty())
	{
		throw std::invalid_argument{ "String is empty." };
	}

	// Rempve leading '#' if present.
	if (s.at(0) == '#')
	{
		s = s.substr(1);
	}

	// Check if the string is valid.
	// Valid strings are 2, 4, 6 or 8 chars long.
	if (s.empty() || s.size() > 8 || (s.size() % 2) != 0 ||
		s.find_first_not_of("0123456789ABCDEF") != std::string::npos)
	{
		throw std::invalid_argument{ "String is malformed." };
	}

	int numComponents = static_cast<int>(s.size() / 2);
	int r_int = 0;
	int g_int = 0;
	int b_int = 0;
	int a_int = 0;

	switch (numComponents)
	{
	case 1:
		r_int = strtol(s.substr(0, 2).c_str(), NULL, 16);
		g_int = r_int;
		b_int = r_int;
		a_int = INTMAX;
		break;
	case 2:
		r_int = strtol(s.substr(0, 2).c_str(), NULL, 16);
		g_int = r_int;
		b_int = r_int;
		a_int = strtol(s.substr(2, 2).c_str(), NULL, 16);
		break;
	case 3:
		r_int = strtol(s.substr(0, 2).c_str(), NULL, 16);
		g_int = strtol(s.substr(2, 2).c_str(), NULL, 16);
		b_int = strtol(s.substr(4, 2).c_str(), NULL, 16);
		a_int = INTMAX;
		break;
	case 4:
		r_int = strtol(s.substr(0, 2).c_str(), NULL, 16);
		g_int = strtol(s.substr(2, 2).c_str(), NULL, 16);
		b_int = strtol(s.substr(4, 2).c_str(), NULL, 16);
		a_int = strtol(s.substr(6, 2).c_str(), NULL, 16);
		break;
	default:
		// This should not happen
		break;
	}

	_r = static_cast<float>(clamp(r_int)) / static_cast<float>(INTMAX);
	_g = static_cast<float>(clamp(g_int)) / static_cast<float>(INTMAX);
	_b = static_cast<float>(clamp(b_int)) / static_cast<float>(INTMAX);
	_a = static_cast<float>(clamp(a_int)) / static_cast<float>(INTMAX);
}

Color::Color(
	const Color &other)
	:_r{ other._r },
	_g{ other._g },
	_b{ other._b },
	_a{ other._a }
{

}

Color::Color(
	Color &&other) noexcept
{
	swap(*this, other);
}

//=============================================================================
// Destructor
//=============================================================================

Color::~Color()
{

}

//=============================================================================
// Assignment operators
//=============================================================================

Color & Color::operator=(
	const Color &other) &
{
	_r = other._r;
	_g = other._g;
	_b = other._b;
	_a = other._a;

	return *this;
}

Color & Color::operator=(
	Color &&other) & noexcept
{
	swap(*this, other);

	return *this;
}

//=============================================================================
// Arithmetic operators
//=============================================================================

Color Color::operator+(
	const Color &rhs) const
{
	return Color(
		clamp(_r + rhs._r),
		clamp(_g + rhs._g),
		clamp(_b + rhs._b),
		clamp(_a + rhs._a));
}

Color operator+(
	const Color &lhs,
	float rhs)
{
	return lhs + Color{ rhs , Color::MIN };
}

Color operator+(
	float lhs,
	const Color &rhs)
{
	return Color{ lhs, Color::MIN } +rhs;
}

Color Color::operator-(
	const Color &rhs) const
{
	return Color(
		clamp(_r - rhs._r),
		clamp(_g - rhs._g),
		clamp(_b - rhs._b),
		clamp(_a - rhs._a));
}


Color operator-(
	const Color &lhs,
	float rhs)
{
	return lhs - Color{ rhs, Color::MIN };
}

Color operator-(
	float lhs,
	const Color &rhs)
{
	const float rhsA = rhs.getA();

	Color result = Color{ lhs } -rhs;

	result.setA(rhsA);

	return result;
}

//=============================================================================
// Relational Operators
//=============================================================================

bool Color::operator==(
	const Color &other) const
{
	// TODO: Use epsilon instead of equals
	return _r == other._r &&
		_g == other._g &&
		_b == other._b &&
		_a == other._a;
}

bool Color::operator!=(
	const Color &other) const
{
	return !(*this == other);
}

//=============================================================================
// Compound operators
//=============================================================================

Color & Color::operator+=(
	const Color &rhs) &
{
	*this = *this + rhs;

	return *this;
}

Color & Color::operator+=(
	float rhs) &
{
	*this = *this + rhs;

	return *this;
}

Color & Color::operator-=(
	const Color &rhs) &
{
	*this = *this - rhs;

	return *this;
}

Color & Color::operator-=(
	float rhs) &
{
	*this = *this - rhs;

	return *this;
}

//=============================================================================
// Member access
//=============================================================================

float Color::getR() const
{
	return _r;
}

float Color::getG() const
{
	return _g;
}

float Color::getB() const
{
	return _b;
}

float Color::getA() const
{
	return _a;
}

int Color::getRint() const
{
	return static_cast<int>(_r * INTMAX);
}

int Color::getGint() const
{
	return static_cast<int>(_g * INTMAX);
}

int Color::getBint() const
{
	return static_cast<int>(_b * INTMAX);
}

int Color::getAint() const
{
	return static_cast<int>(_a * INTMAX);
}

void Color::setR(
	float r)
{
	_r = clamp(r);
}

void Color::setG(
	float g)
{
	_g = clamp(g);
}

void Color::setB(
	float b)
{
	_b = clamp(b);
}

void Color::setA(
	float a)
{
	_a = clamp(a);
}

void Color::setRint(
	int r)
{
	_r = static_cast<float>(clamp(r)) / static_cast<float>(INTMAX);
}

void Color::setGint(
	int g)
{
	_g = static_cast<float>(clamp(g)) / static_cast<float>(INTMAX);
}

void Color::setBint(
	int b)
{
	_b = static_cast<float>(clamp(b)) / static_cast<float>(INTMAX);
}

void Color::setAint(
	int a)
{
	_a = static_cast<float>(clamp(a)) / static_cast<float>(INTMAX);
}

void Color::getRGB(
	float *vec) const
{
	vec[0] = _r;
	vec[1] = _g;
	vec[2] = _b;
}

void Color::getBGR(
	float *vec) const
{
	vec[0] = _b;
	vec[1] = _g;
	vec[2] = _r;
}

void Color::getHSL(
	float *vec) const
{
	float max = _r;
	float min = _r;
	int maxi = 0;

	if (_g > max)
	{
		max = _g;
		maxi = 1;
	}

	if (_b > max)
	{
		max = _b;
		maxi = 2;
	}

	if (_g < min)
	{
		min = _g;
	}

	if (_b < min)
	{
		min = _b;
	}


	float h;
	float s;
	float l = (max + min) / 2.f;

	if (max == min)
	{
		h = 0.f;
		s = 0.f;
	}
	else
	{
		float d = max - min;

		if (l > 0.5f)
		{
			s = d / (2 - max - min);
		}
		else
		{
			s = d / (max + min);
		}

		switch (maxi)
		{
		case 0:
			h = (_g - _b) / d + (_g < _b ? 6.f : 0.f);
			break;
		case 1:
			h = (_b - _r) / d + 2.f;
			break;
		case 2:
			h = (_r - _g) / d + 4.f;
			break;
		default:
			h = 0.f;
			break;
		}

		h /= 6.f;
	}

	vec[0] = h;
	vec[1] = s;
	vec[2] = l;
}

void Color::getRGBA(
	float *vec) const
{
	vec[0] = _r;
	vec[1] = _g;
	vec[2] = _b;
	vec[3] = _a;
}

void Color::getBGRA(
	float *vec) const
{
	vec[0] = _b;
	vec[1] = _g;
	vec[2] = _r;
	vec[3] = _a;
}

void Color::getHSLA(
	float *vec) const
{
	getHSL(vec);
	vec[3] = _a;
}

void Color::getRGBint(
	int *vec) const
{
	float v[3];
	getRGB(v);

	for (int i = 0; i < 3; ++i)
	{
		vec[i] = static_cast<int>(v[i] * INTMAX);
	}
}

void Color::getBGRint(
	int *vec) const
{
	float v[3];
	getBGR(v);

	for (int i = 0; i < 3; ++i)
	{
		vec[i] = static_cast<int>(v[i] * INTMAX);
	}
}

void Color::getHSLint(
	int *vec) const
{
	float v[3];
	getHSL(v);

	for (int i = 0; i < 3; ++i)
	{
		vec[i] = static_cast<int>(v[i] * INTMAX);
	}
}

void Color::getRGBAint(
	int *vec) const
{
	float v[4];
	getRGBA(v);

	for (int i = 0; i < 4; ++i)
	{
		vec[i] = static_cast<int>(v[i] * INTMAX);
	}
}

void Color::getBGRAint(
	int *vec) const
{
	float v[4];
	getBGRA(v);

	for (int i = 0; i < 4; ++i)
	{
		vec[i] = static_cast<int>(v[i] * INTMAX);
	}
}

void Color::getHSLAint(
	int *vec) const
{
	float v[4];
	getHSLA(v);

	for (int i = 0; i < 4; ++i)
	{
		vec[i] = static_cast<int>(v[i] * INTMAX);
	}
}

std::string Color::toString() const
{
	int ivec[4];
	getRGBAint(ivec);

	std::stringstream ss{};

	ss << '#' <<
		std::setbase(16) << std::setw(2) << std::setfill('0') << ivec[0] <<
		std::setbase(16) << std::setw(2) << std::setfill('0') << ivec[1] <<
		std::setbase(16) << std::setw(2) << std::setfill('0') << ivec[2] <<
		std::setbase(16) << std::setw(2) << std::setfill('0') << ivec[3];

	return ss.str();
}

//=============================================================================
// Helper Functions
//=============================================================================

float Color::hueToRgb(
	float p,
	float q,
	float t)
{
	if (t < 0)
	{
		t += 1.f;
	}
	else if (t > 1.f)
	{
		t -= 1.f;
	}

	if (t < 1.f / 6.f)
	{
		return p + (q - p) * 6.f * t;
	}

	if (t < 1.f / 2.f)
	{
		return q;
	}

	if (t < 2.f / 3.f)
	{
		return p + (q - p) * (2.f / 3.f - t) * 6.f;
	}

	return p;
}

float Color::clamp(
	float v,
	float min,
	float max)
{
	if (v < min)
	{
		return min;
	}

	if (v > max)
	{
		return max;
	}

	return v;
}

int Color::clamp(
	int v,
	int min,
	int max)
{
	if (v < min)
	{
		return min;
	}

	if (v > max)
	{
		return max;
	}

	return v;
}

Color interpolate(
	const Color &a,
	const Color &b,
	float t)
{
	float aHsl[4];
	float bHsl[4];
	float resHsl[4];

	a.getHSLA(aHsl);
	b.getHSLA(bHsl);

	for (unsigned int i = 0; i < 4; ++i)
	{
		resHsl[i] = aHsl[i] * (1.f - t) + bHsl[i] * t;
	}

	return Color{ resHsl, true };
}

std::ostream & operator<<(
	std::ostream &stream,
	const Color &c)
{
	stream << c._r << " " << c._g << " " << c._b << " " << c._a;

	return stream;
}

std::istream & operator>>(
	std::istream &stream,
	Color &c) 
{
	stream >> c._r >> c._g >> c._b >> c._a;

	return stream;
}

void swap(
	Color &lhs,
	Color &rhs) noexcept
{
	std::swap(lhs._r, rhs._r);
	std::swap(lhs._g, rhs._g);
	std::swap(lhs._b, rhs._b);
	std::swap(lhs._a, rhs._a);
}

//=============================================================================
// End of file.
//=============================================================================