#ifndef VORBIS_DECODER_HPP


#define VORBIS_DECODER_HPP

#include "ogg.h"
#include <vector>
#include <pshpack1.h>
typedef struct
{ char id[4];         //="fmt "
  unsigned long size; //=16
  short wFormatTag;   //=WAVE_FORMAT_PCM=1
  unsigned short wChannels;       //=1 or 2 for mono or stereo
  unsigned long dwSamplesPerSec;  //=11025 or 22050 or 44100
  unsigned long dwAvgBytesPerSec; //=wBlockAlign * dwSamplesPerSec
  unsigned short wBlockAlign;     //=wChannels * (wBitsPerSample==8?1:2)
  unsigned short wBitsPerSample;  //=8 or 16, for bits per sample
} FmtChunk;

typedef struct
{ char id[4];            //="data"
  unsigned long size;    //=datsize, size of the following array
 ///char data[1];          //=the raw data goes here 
} DataChunk;

typedef struct
{ char id[4];         //="RIFF"
  unsigned long size; //=datsize+8+16+4
  char type[4];       //="WAVE"
  FmtChunk fmt;
  DataChunk dat;
} WavChunk;
#include <poppack.h>


/** 
	A wrapper around vorbis decoder to feed to directmusic loader
*/
class VorbisFile
{
private:
	long m_Len;
	long m_Freq;
	long m_Channels;
	std::vector<char> m_Header;
public:
	VorbisFile()
	{
	}
	~VorbisFile()
	{
		ov_clear(&vf);
	}
	long GetLength();
	long GetFreq();
	long GetChannels();
	int Open(const std::vector<unsigned char>& data);
	const std::vector<char>& GetHeader();
	int Read(void* pBuf , int size );
	void Seek(int pos );
private:
	OggVorbis_File vf;
	std::vector<unsigned char> m_Data;
};

#endif