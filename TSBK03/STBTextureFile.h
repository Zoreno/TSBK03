#pragma once

#include "TextureFile.h"

class STBTextureFile : public TextureFile
{
public:
	STBTextureFile(const std::string& filePath);
	~STBTextureFile() override;
public:
	const std::vector<unsigned char> & getPixels() const override;

	unsigned getWidth() const override;

	unsigned getHeight() const override;

	bool hasAlpha() const override;

	int _width;
	int _height;
	int _numChannels;

	std::vector<unsigned char> _data;
};