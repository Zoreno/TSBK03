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
