/**
* @file	TGA.cpp
* @Author	Joakim Bertils
* @date	2017-05-21
* @brief	TGA Texture file Implementation
*/

#include "TGA.h"

#include <fstream>
#include <iostream>

TGA::TGA(const char* filePath)
{
	std::fstream hFile(filePath, std::ios::in | std::ios::binary);

	if (!hFile.is_open())
	{
		throw std::invalid_argument(std::string("File (") + filePath + ") could not be opened.");
	}

	// Allocate header
	TGAFileHeader fileHeader;
	memset(&fileHeader, 0, sizeof(TGAFileHeader));

	// Read header into struct
	hFile.read(reinterpret_cast<char*>(&fileHeader), sizeof(TGAFileHeader));


	if (fileHeader.imageType == IS_TYPE_UNCOMPRESSED_TRUE_COLOR) // File is not compressed and in true color.
	{
		readUncompressedTrueColor(hFile, fileHeader, filePath);
	}
	else if (fileHeader.imageType == IS_TYPE_RUN_LENGTH_TRUE_COLOR) // File is compressed and in true color
	{
		readRunLengthTrueColor(hFile, fileHeader, filePath);
	}
	// Add more imageTypes here as needed.
	else
	{
		hFile.close();
		throw std::invalid_argument(std::string("Invalid file format in file (") + filePath + "). Expected compressed or uncompressed true-color format.");
	}

	hFile.close();
}

const std::vector<uint8_t>& TGA::getPixels() const
{
	return _pixels;
}

uint32_t TGA::getWidth() const
{
	return _width;
}

uint32_t TGA::getHeight() const
{
	return _height;
}

bool TGA::hasAlpha() const
{
	return _bitsPerPixel == PD_BITCOUNT_32;
}

void TGA::readUncompressedTrueColor(
	std::fstream &hFile,
	TGAFileHeader& fileHeader,
	const char *filePath)
{
	// Read file meta-data
	_bitsPerPixel = fileHeader.imageSpecification.pixelDepth;
	_width = fileHeader.imageSpecification.imageWidth;
	_height = fileHeader.imageSpecification.imageHeight;
	_size = (_width * _bitsPerPixel + 31) / 8 * _height;

	// Check that it is the correct format.
	if (_bitsPerPixel != PD_BITCOUNT_24 && _bitsPerPixel != PD_BITCOUNT_32)
	{
		hFile.close();
		throw std::invalid_argument(std::string("Invalid file format in file (") + filePath + "). Expected 24 or 32 bit image.");
	}

	// Prepare pixel buffer.
	_pixels.resize(_size);
	_isCompressed = false;

	// Read file content.
	hFile.read(reinterpret_cast<char*>(_pixels.data()), _size);
}

void TGA::readRunLengthTrueColor(
	std::fstream &hFile,
	TGAFileHeader &fileHeader,
	const char *filePath)
{
	// Read file meta-data
	_bitsPerPixel = fileHeader.imageSpecification.pixelDepth;
	_width = fileHeader.imageSpecification.imageWidth;
	_height = fileHeader.imageSpecification.imageHeight;
	_size = (_width * _bitsPerPixel + 31) / 8 * _height;

	// Check that it is the correct format.
	if (_bitsPerPixel != PD_BITCOUNT_24 && _bitsPerPixel != PD_BITCOUNT_32)
	{
		hFile.close();
		throw std::invalid_argument(std::string("Invalid file format in file (") + filePath + "). Expected 24 or 32 bit image.");
	}

	// Allocate a pixel-struct.
	PixelInfo pixel;
	memset(&pixel, 0, sizeof(PixelInfo));

	int currentByte = 0;
	std::size_t currentPixel = 0;
	_isCompressed = true;
	uint8_t chunkHeader = 0;
	const int bytesPerPixel = (_bitsPerPixel) / 8;
	_pixels.resize(_width * _height * sizeof(PixelInfo));

	// Loop through each pixel in memory array.
	do
	{
		// Read chunk header
		hFile.read(reinterpret_cast<char*>(&chunkHeader), sizeof(chunkHeader));

		// If chunk header bit 7 is 0, add one to value and read that many pixels from file.
		if (chunkHeader < 128)
		{
			++chunkHeader;
			for (int i{ 0 }; i < chunkHeader; ++i, ++currentPixel)
			{
				hFile.read(reinterpret_cast<char*>(&pixel), bytesPerPixel);

				_pixels[currentByte++] = pixel.B;
				_pixels[currentByte++] = pixel.G;
				_pixels[currentByte++] = pixel.R;

				// Take alpha channel into account if present.
				if (_bitsPerPixel == PD_BITCOUNT_32)
				{
					_pixels[currentByte++] = pixel.A;
				}
			}
		}
		// If chunk header bit 7 is 1, subtract 127 to header and read the next value.
		// Repeat that value chunk header times.
		else
		{
			chunkHeader -= 127;
			hFile.read(reinterpret_cast<char*>(&pixel), bytesPerPixel);

			for (int i{ 0 }; i < chunkHeader; ++i, ++currentPixel)
			{
				_pixels[currentByte++] = pixel.B;
				_pixels[currentByte++] = pixel.G;
				_pixels[currentByte++] = pixel.R;

				// Take alpha channel into account if present.
				if (_bitsPerPixel == PD_BITCOUNT_32)
				{
					_pixels[currentByte++] = pixel.A;
				}
			}
		}
	} while (currentPixel < (_height * _width));
}
