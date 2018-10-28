/**
* @file	TGA.h
* @Author	Joakim Bertils
* @date	2017-02-09
* @brief	Targa File Format (TGA)
*/

#pragma once

#include "TextureFile.h"

#define COLOR_MAP_NOT_INCLUDED			0
#define COLOR_MAP_INCLUDED				1

#define IS_TYPE_NO_DATA					0
#define IS_TYPE_UNCOMPRESSED_COLOR_MAP	1
#define IS_TYPE_UNCOMPRESSED_TRUE_COLOR	2
#define IS_TYPE_UNCOMPRESSED_BW			3
#define IS_TYPE_RUN_LENGTH_COLOR_MAP	9
#define IS_TYPE_RUN_LENGTH_TRUE_COLOR	10
#define IS_TYPE_RUN_LENGTH_BW			11

#define PD_BITCOUNT_1					1
#define PD_BITCOUNT_4					4
#define PD_BITCOUNT_8					8
#define PD_BITCOUNT_16					16
#define PD_BITCOUNT_24					24
#define PD_BITCOUNT_32					32

#pragma pack(push, 1)

// File Format
// 
//	---------------------
//  | File Header       |
//	---------------------
//  | Image ID          |
//	---------------------
//  | Color Map Data    |
//	---------------------
//  | Image Data        |
//	---------------------
//  | Developer Area    |
//	---------------------
//  | Extended Area     |
//	---------------------
//  | File Footer       |
//	---------------------

struct TGAColorMapSpecification
{
	uint16_t firstEntryIndex;
	uint16_t colorMapLength;
	uint8_t colorMapEntrySize;
};

struct TGAImageSpecification
{
	uint16_t xOrigin;
	uint16_t yOrigin;
	uint16_t imageWidth;
	uint16_t imageHeight;
	uint8_t pixelDepth;
	uint8_t imageDescriptor;
};

struct TGAFileHeader
{
	uint8_t IDLength;
	uint8_t colorMapType;
	uint8_t imageType;
	TGAColorMapSpecification colorMapSpecification;
	TGAImageSpecification imageSpecification;
};

struct TGADateTimeEntry
{
	uint16_t month;
	uint16_t day;
	uint16_t year;
	uint16_t hour;
	uint16_t minute;
	uint16_t second;
};

struct TGAJobTimeEntry
{
	uint16_t hours;
	uint16_t minutes;
	uint16_t seconds;
};

struct TGASoftwareVersion
{
	uint16_t versionNumber;
	char versionLetter;
};

struct TGAPixelAspectRatio
{
	uint16_t numerator;
	uint16_t denominator;
};

struct TGAGammaValue
{
	uint16_t numerator;
	uint16_t denominator;
};

struct TGAExtensionArea
{
	uint16_t extensionSize;
	char author[41];
	char authorComments[324];
	TGADateTimeEntry dateTimeEntry;
	char jobID[41];
	TGAJobTimeEntry jobTimeEntry;
	char softwareID[41];
	TGASoftwareVersion softwareVersion;
	uint32_t keyColor;
	TGAPixelAspectRatio aspectRatio;
	TGAGammaValue gammaValue;
	uint32_t colorCorrectionOffset;
	uint32_t postageStampOffset;
	uint32_t scanLineOffset;
	uint8_t attributesType;
};

struct TGAFileFooter
{
	uint32_t extensionAreaFooter;
	uint32_t developerDirectoryOffset;
	char signature[16];
	char reserved;
	char zeroTerminator;
};

#pragma pack(pop)

/**
* @brief TGA file representation.
*/
class TGA : public TextureFile
{
public:

	/**
	* @brief Constructor
	* @param filePath Path to the file.
	*/
	explicit TGA(const char* filePath);

	/**
	* @brief Gets the pixels in the file.
	* @return Vector containing pixel data.
	*/
	const std::vector<uint8_t>& getPixels() const override;

	/**
	* @brief Gets the width of the texture
	* @return Width of texture
	*/
	uint32_t getWidth() const override;

	/**
	* @brief Gets the height of the texture
	* @return Height of texture
	*/
	uint32_t getHeight() const override;

	/**
	* @brief Checks whether the file has alpha channel information.
	* @return True if file has alpha information.
	*/
	bool hasAlpha() const override;

private:

	/**
	* @brief Vector containing the pixels.
	*/
	std::vector<uint8_t> _pixels;

	/**
	* @brief True if the source file is compressed.
	*/
	bool _isCompressed;

	/**
	* @brief The Width of the image.
	*/
	uint32_t _width;

	/**
	* @brief The Height of the image.
	*/
	uint32_t _height;

	/**
	* @brief The Size in bytes of the image.
	*/
	uint32_t _size;

	/**
	* @brief The number of bits per pixel.
	*/
	uint32_t _bitsPerPixel;

	/**
	 * @brief Read an uncompressed byte stream from the file
	 * @param hFile File stream handle
	 * @param fileHeader TGA file header
	 * @param filePath Path to file
	 */
	void readUncompressedTrueColor(
		std::fstream &hFile,
		TGAFileHeader &fileHeader,
		const char *filePath);

	/**
	 * @brief Read a run-length compressed byte stream from the file 
	 * @param hFile File stream handle
	 * @param fileHeader TGA file header
	 * @param filePath Path to file
	 */
	void readRunLengthTrueColor(
		std::fstream &hFile,
		TGAFileHeader &fileHeader,
		const char *filePath);
};