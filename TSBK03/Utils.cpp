#include "Utils.h"

std::string getStringFromFile(
	const std::string &file)
{
	return std::string
	{
		std::istreambuf_iterator<char>(std::ifstream{file}),
		std::istreambuf_iterator<char>()
	};
}

std::ostream & operator<<(
	std::ostream &stream,
	const glm::vec2 &rhs)
{
	stream << rhs.x << " " << rhs.y;

	return stream;
}

std::ostream & operator<<(
	std::ostream &stream,
	const glm::vec3 &rhs)
{
	stream << rhs.x << " " << rhs.y << " " << rhs.z;

	return stream;
}


std::ostream & operator<<(
	std::ostream &stream,
	const glm::vec4 &rhs)
{
	stream << rhs.x << " " << rhs.y << " " << rhs.z << " " << rhs.w;

	return stream;
}

