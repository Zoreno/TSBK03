/**
* @file	WaveFile.cpp
* @Author	Joakim Bertils
* @date	2017-02-17
* @brief	WaveFile(.wav) format parser
*/

#include "WaveFile.h"

#include <fstream>
#include <iostream>

WaveFile::WaveFile(const std::string& path, ALboolean loop)
	:_loop{ loop }
{
	std::ifstream fStream{ path , std::ios::binary };

	if (!fStream.is_open())
	{
		std::cerr << "Could not open file" << std::endl;
		return;
	}

	WAVHeader header;

	fStream.read(reinterpret_cast<char*>(&header), sizeof(header));

	// Check that file is in a RIFF chunk

	if (strncmp(header.chunkID, "RIFF", 4) != 0)
	{
		std::cerr << "File not RIFF format" << std::endl;
		return;
	}

	// Check that file is in WAVE format

	if (strncmp(header.format, "WAVE", 4) != 0)
	{
		std::cerr << "File Not WAVE Format" << std::endl;
		return;
	}

	if (strncmp(header.subChunk1ID, "fmt ", 4) != 0)
	{
		std::cerr << "Format sub-chunk expected" << std::endl;
		return;
	}

#if 0

	std::cout << "Successully read file " << path << " of size " << header.chunkSize << " bytes." << std::endl;

	std::cout << "Format Header Data:" << std::endl;

	std::cout << "Audio Format: " << header.audioFormat << std::endl;
	std::cout << "Number of Channels: " << header.numChannels << std::endl;
	std::cout << "Sample Rate: " << header.sampleRate << std::endl;
	std::cout << "Byte Rate: " << header.byteRate << std::endl;
	std::cout << "Block Align: " << header.blockAlign << std::endl;
	std::cout << "Bits Per Sample: " << header.bitsPerSample << std::endl;

#endif

	if (header.bitsPerSample == 16)
	{
		if (header.numChannels == 2)
		{
			_format = AL_FORMAT_STEREO16;
		}
		else
		{
			_format = AL_FORMAT_MONO16;
		}
	}
	else if (header.bitsPerSample == 8)
	{
		if (header.numChannels == 2)
		{
			_format = AL_FORMAT_STEREO8;
		}
		else
		{
			_format = AL_FORMAT_MONO8;
		}
	}

	_frequency = header.sampleRate;

	if (strncmp(header.subChunk2ID, "data", 4) != 0)
	{
		std::cerr << "Data sub-chunk expected" << std::endl;
		return;
	}

	_size = header.subChunk2Size;

	// Allocate memory for data
	_data = malloc(_size);

	char* dataPtr = static_cast<char*>(_data);

	for (uint32_t i{ 0 }; i < _size; ++i)
	{

		fStream.read(dataPtr + i, sizeof(char));

		if (fStream.eof())
		{
			std::cout << "Unexpected eof" << std::endl;
			std::cout << i << std::endl;
			break;
		}

		if (!fStream.good())
		{
			std::cerr << "Error reading file" << std::endl;
			break;
		}
	}
	fStream.close();
}

WaveFile::~WaveFile()
{
	free(_data);
}

void WaveFile::setLoop(ALboolean value)
{
	_loop = value;
}

ALenum WaveFile::getFormat() const
{
	return _format;
}

ALsizei WaveFile::getSize() const
{
	return _size;
}

ALsizei WaveFile::getFrequency() const
{
	return _frequency;
}

ALboolean WaveFile::getLoop() const
{
	return _loop;
}

ALvoid* WaveFile::getData() const
{
	return _data;
}