/**
* @file	WaveFile.h
* @Author	Joakim Bertils
* @date	2017-02-17
* @brief	WaveFile(.wav) format parser.
*/

#pragma once

#include <stdint.h>

#include <string>
#include <AL/al.h>
#include <AL/alc.h>

/**
* @brief WAV File structure header.
*/
struct WAVHeader
{
	//=========================================================================
	//  RIFF chunk Descriptor
	//=========================================================================

	/**
	* @brief RIFF chunk ID. Should be "RIFF"
	*/
	char chunkID[4];

	/**
	* @brief RIFF chunk size.
	*/
	uint32_t chunkSize;

	/**
	* @brief Format description. Should be "WAVE"
	*/
	char format[4];

	//=========================================================================
	// "fmt " sub-chunk
	//=========================================================================

	/**
	* @brief Format sub-chunk ID. Should be "fmt "
	*/
	char subChunk1ID[4];

	/**
	* @brief Format sub-chunk size;
	*/
	uint32_t subChunk1Size;

	/**
	* @brief Audio Format
	*
	*  Allowed values are:
	*
	*   1 - PCM
	*   6 - mulaw
	*   7 - alaw
	*   257 - IBM Mu-Law
	*   258 - IBM A-Law
	*   259 - ADPCM
	*/
	uint16_t audioFormat;

	/**
	* @brief Number of audio channels
	*/
	uint16_t numChannels;

	/**
	* @brief Sampling frequency in Hz
	*/
	uint32_t sampleRate;

	/**
	* @brief Bytes per Second
	*/
	uint32_t byteRate;

	/**
	* @brief Alignment for each block.
	*/
	uint16_t blockAlign;

	/**
	* @brief Number of bits per sample
	*/
	uint16_t bitsPerSample;

	//=========================================================================
	// "data" sub-chunk
	//=========================================================================

	/**
	* @brief Data sub-chunk ID. Should be "data"
	*/
	char subChunk2ID[4];

	/**
	* @brief Data sub-chunk size;
	*/
	uint32_t subChunk2Size;
};

/**
* @brief Internal representation of a Wave file.
*/
class WaveFile
{
public:
	/**
	* @brief Constructor
	* @param path Path to .wav file.
	* @param loop True if playback should loop the file.
	*/
	explicit WaveFile(const std::string& path, ALboolean loop = false);

	/**
	* @brief Destructor
	*/
	~WaveFile();

	/**
	* @brief Setter for the loop value
	* @param value New value
	*/
	void setLoop(ALboolean value);

	/**
	* @brief Gets the entry format.
	* @return Format
	*/
	ALenum getFormat() const;

	/**
	* @brief Gets the size of the data.
	* @return Size
	*/
	ALsizei getSize() const;

	/**
	* @brief Gets the sampling frequency.
	* @return Frequency
	*/
	ALsizei getFrequency() const;

	/**
	* @brief Gets the loop value.
	* @return Loop value
	*/
	ALboolean getLoop() const;

	/**
	* @brief Gets a pointer to the stored data.
	* @return Data pointer.
	*/
	ALvoid* getData() const;
private:

	/**
	* @brief Entry format
	*/
	ALenum _format;

	/**
	* @brief Size of data.
	*/
	ALsizei _size{ 0 };

	/**
	* @brief Sampling Frequency.
	*/
	ALsizei _frequency{ 0 };

	/**
	* @brief Loop value.
	*/
	ALboolean _loop{ false };

	/**
	* @brief Sound data.
	*/
	ALvoid* _data{ nullptr };
};