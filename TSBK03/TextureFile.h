#pragma once

#include <vector>

#include "PixelInfo.h"

class TextureFile
{
public:
	virtual ~TextureFile() = default;
	virtual const std::vector<unsigned char>& getPixels() const = 0;
	virtual unsigned int getWidth() const = 0;
	virtual unsigned int getHeight() const = 0;
	virtual bool hasAlpha() const = 0;
};