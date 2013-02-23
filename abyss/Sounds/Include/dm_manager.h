#ifndef DM_MANAGER_HPP
#define DM_MANAGER_HPP

#include "dm_loader.h"
#include <windows.h>
#include <dmusici.h>

#include <map>
#include <vector>

#ifndef RELEASE
#define RELEASE(x) { if(x) (x)->Release(); x = NULL; }
#endif



#define MAX_APATH 16

class CDMManager
{
public:
	CDMManager():
	  m_pPerformance(NULL)
	   , m_Loader()
	{
		m_Channel = 0;
		m_Volume = 100;
	}

	~CDMManager()
	{
		std::map<long ,IDirectMusicSegment8* >::iterator it = m_Segments.begin();
		while (it != m_Segments.end())
		{
			it->second->Release();
			it++;
		}
		
		RELEASE(m_Channel);
	};

public:
	void Init(HWND hwd);
	void CreateWaveSegment(long handle, const std::vector<unsigned char>& data);
	void RemoveSegment(long handle);
	void PlayWaveSegment(long handle, unsigned short channel);
	void SetVolume(long volume) 
	{
		m_pPerformance->SetGlobalParam( GUID_PerfMasterVolume, 
                        (void*)&volume, sizeof(long) );                    
		m_Channel->SetVolume(volume,0);
		m_Volume  = volume;
	}
	void SetVolume(long handle,long volume) 
	{
		// multiple audiopath not implemented.
		
	}
	IDirectMusicPerformance8 * GetPerformance() const 
	{
		return m_pPerformance;
	}

private:
	IDirectMusicSegment8* getSegment(long handle)
	{
		std::map<long , IDirectMusicSegment8* >::iterator& pit = m_Segments.find(handle);
		if (pit != m_Segments.end())
		{
			return pit->second;
		}
		return NULL;
	}




	DMLoader m_Loader;
	IDirectMusicPerformance8 *m_pPerformance;
	std::map<long , IDirectMusicSegment8* > m_Segments;
	
	std::map<short , long> m_ActiveChannels;
	IDirectMusicAudioPath* m_Channel; 
	long m_Volume;

};

#endif