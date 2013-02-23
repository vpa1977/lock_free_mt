#ifndef DS_MANAGER_HPP

#define DS_MANAGER_HPP 

#include <windows.h>
#include <mmsystem.h>
#include <dxerr8.h>
#include <dsound.h>
#include <vector>
#include <map>
#include "vorbis_decoder.h"
#include "vfs_stream.h"
#include <sys/types.h>
#include <boost/shared_ptr.hpp>


#ifndef RELEASE
#define RELEASE(x) { if(x) (x)->Release(); x = NULL; }
#endif




class CDSound 
{
	public:
		CDSound(const std::vector<unsigned char>& data,IDirectSound8* pDS);
		~CDSound()
		{
			RELEASE(m_pStream);
			RELEASE(m_pdsBuffer);
			m_pdsBuffer = NULL;
			m_pStream = NULL;
		}

	public:
		void Play(); 
		void Update();
		void Stop();
		void SetVolume(long volume)
		{
			volume =  DSBVOLUME_MIN +(DSBVOLUME_MAX - DSBVOLUME_MIN) * volume / 100;
			if (m_pdsBuffer) m_pdsBuffer->SetVolume(volume);
		}
		bool isOK() {return m_bOK; };
	private:
		bool m_bIsActive;
		CVFSStream* m_pStream;
		IDirectSoundBuffer8* m_pdsBuffer;
		DWORD m_dwWritePos;
		bool m_bLooped;
		bool m_bOK;
};


class CDSManager
{
public:
	CDSManager()
		:m_pDS(0)
	{
	}

	~CDSManager()
	{
		
		RELEASE(m_pDS);
	};

public:
	void Update();
	void Init(HWND hwd);
	void CreateWaveSegment(long handle, const std::vector<unsigned char>& data);
	void RemoveSegment(long handle);
	void PlayWaveSegment(long handle, unsigned short channel);
	void SetVolume(long volume);
	void SetVolume(long handle,long volume);
	
private:
HRESULT SetPrimaryBufferFormat( DWORD dwPrimaryChannels, 
                                               DWORD dwPrimaryFreq, 
                                               DWORD dwPrimaryBitRate );



	IDirectSound8* m_pDS;
	typedef boost::shared_ptr< CDSound > PSound;
	typedef std::map<long , PSound> SoundMap;
	SoundMap m_SoundMap;
};

#endif 