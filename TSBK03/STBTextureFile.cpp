#include "STBTextureFile.h"

#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

STBTextureFile::STBTextureFile(
	const std::string &filePath)
{
	// 
	stbi_set_flip_vertically_on_load(1);

	unsigned char *data = stbi_load(filePath.c_str(), &_width, &_height, &_numChannels, 0);

	_data.resize(_width * _height * _numChannels);

	memcpy(&_data[0], data, _width * _height * _numChannels);

	stbi_image_free(data);
}

STBTextureFile::~STBTextureFile()
{
	
}

const std::vector<unsigned char> & STBTextureFile::getPixels() const
{
	return _data;
}

unsigned STBTextureFile::getWidth() const
{
	return _width;
}

unsigned STBTextureFile::getHeight() const
{
	return _height;
}

bool STBTextureFile::hasAlpha() const
{
	return _numChannels == 4;
}
