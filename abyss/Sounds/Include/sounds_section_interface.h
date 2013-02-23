#ifndef SOUNDS_SECTION_INTERFACE_HPP
#define SOUNDS_SECTION_INTERFACE_HPP


#define CSoundsSection_CLSID 120119662

#include "../../MT_Core/Include/command.h"
#include <string>

#define SND_OGG 1
#define SND_WAVE 2

struct CSoundsSection_InitSoundsRequest 
	: public CCommand
{
	CSoundsSection_InitSoundsRequest( HWND wnd , 
				unsigned long coop = 2, 
				unsigned long primaryChannels = 2, // stereo
				unsigned long primaryFreq = 22050, // 22 kbit
				unsigned long primaryBitRate = 16) 
				: hWnd(wnd) , dwCoopLevel(coop), dwPrimaryChannels(primaryChannels), 
				dwPrimaryFreq(primaryFreq), dwPrimaryBitRate(primaryBitRate)
				
	{
	}
	HWND hWnd;
	
	unsigned long dwCoopLevel;
	unsigned long dwPrimaryChannels;
	unsigned long dwPrimaryFreq;
	unsigned long dwPrimaryBitRate;

};

struct CSoundsSection_InitSoundsResponse
	:public CCommand
{
	CSoundsSection_InitSoundsResponse(bool success)
		: bSuccess(success)
	{

	}
	bool bSuccess;
};


/** 
	Those commands are intended for playing short effect sounds saved in wav files. 
*/
struct CSoundsSection_OpenSoundRequest
	: public CCommand
{
	CSoundsSection_OpenSoundRequest(long type=0 , const std::string& req = "", long vfs=0, long magic=0) : m_WaveFile(req) , m_VFS(vfs), m_Magic(magic),m_Type(type) {};


	long isWave()
	{
		return 1;
	}
	long isOgg()
	{
		return 0;
	}
	std::string m_WaveFile;
	long m_VFS;
	long m_Magic;
	long m_Type;

};

struct CSoundsSection_OpenSoundResponse
	: public CCommand
{
	CSoundsSection_OpenSoundResponse(long handle,long magic=0) : m_Handle(handle) ,m_Magic(magic) {};
	long m_Handle;
	long m_Magic;
};

/** 
	play request 
*/
struct CSoundsSection_PlaySoundRequest
	: public CCommand
{
	CSoundsSection_PlaySoundRequest(long handle, short channel = 0, long magic  =0) 
		: m_Handle(handle), m_Channel(channel), m_Magic(magic)
	
	{
		
	}
	long m_Handle;
	long m_Channel;
	long m_Magic;
};


struct CSoundsSection_PlaySoundResponse
	:public CCommand
{
	CSoundsSection_PlaySoundResponse(long result=0, long magic=0)
		: m_Result(result), m_Magic(magic)
	{

	}
	long m_Result;
	long m_Magic;
};



/**
	General purpose mute command
*/
struct CSoundsSection_SetVolumeRequest : public CCommand
{
	CSoundsSection_SetVolumeRequest(long volume, long handle = 0)
		: m_Volume(volume), m_Handle(handle)
	{
	}
	long m_Volume;
	long m_Handle;
};



#endif 
