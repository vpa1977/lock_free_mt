#include "../include/dm_manager.h"
#include "../include/vfs_stream.h"
#include "../include/ogg.h"

// The WAV file format.
// The format is a single RIFF WAVE chunk, groupid="RIFF", rifftype="WAVE"
// This contains two chunks, first the format chunk ckID="fmt "
// and next the sound data chunk ckID="data"
// Note: all RIFF blocks contain 4byte identifier, then 4byte 'sizeofrest', then the rest of the chunk.
// The following structures represent the structure of a WAV file.
// NB. the use of pshpack1.h, to ensure that the structures don't get padded by the compiler.




void CDMManager::Init(HWND hWnd)
{
//	    USES_CONVERSION;
	    HRESULT hr;
	
	CoInitialize(NULL);

    hr = CoCreateInstance(CLSID_DirectMusicPerformance, 
		NULL, CLSCTX_INPROC_SERVER, 
		IID_IDirectMusicPerformance8, (void**)&m_pPerformance);

    if (SUCCEEDED(hr))
    {
        hr = m_pPerformance->InitAudio(
            NULL,
            NULL,
            hWnd,
            DMUS_APATH_DYNAMIC_STEREO,
            16,
            0,
            NULL);
    }

    if (SUCCEEDED(hr))
    {
        hr = m_Loader.Init();
    }

	hr = m_pPerformance->CreateStandardAudioPath(DMUS_APATH_DYNAMIC_3D,
			16,true,&m_Channel);
		
		m_Channel->SetVolume(100,0);
	
}


void CDMManager::CreateWaveSegment(long handle, const std::vector<unsigned char>& data)
{
		
		std::vector<unsigned char> wav =data;

		CVFSStream stream(wav);
	    DMUS_OBJECTDESC dmod;
		memset(&dmod, 0, sizeof(dmod));
		dmod.dwSize = sizeof(dmod);
		dmod.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_STREAM;
		dmod.pStream = &stream;
		dmod.guidClass = CLSID_DirectMusicSegment;
		//wcscpy(dmod.wszFileName, L"1.wav");
		IDirectMusicSegment8* pSegment = NULL;
		IDirectMusicSegmentState *pState = NULL;

	    HRESULT hr = m_Loader.GetObject(&dmod, IID_IDirectMusicSegment8, (void**)&pSegment);
	    if (SUCCEEDED(hr))
		{  
			hr = pSegment->Download(m_pPerformance);
		}

		m_Segments[handle] = pSegment;
}

void CDMManager::RemoveSegment(long handle)
{
	
	if (m_Segments.find(handle) != m_Segments.end())
	{
		IDirectMusicSegment8* pSegment = m_Segments.find(handle)->second;
		HRESULT hr = pSegment->Unload(m_pPerformance);
		RELEASE(pSegment);
	}
	m_Segments.erase(handle);
}

void CDMManager::PlayWaveSegment(long handle, unsigned short channel)
{
	IDirectMusicSegment8* pSegment= getSegment(handle);
	
	std::map<short,long  >::iterator& sit= m_ActiveChannels.find(channel);
	if (sit != m_ActiveChannels.end())
	{
		long playingSegment = sit->second;
		// stop this segment from playing
		IDirectMusicSegment8* curr = getSegment(playingSegment);
		if (m_pPerformance->IsPlaying(curr,NULL) == S_OK)
		{
			m_pPerformance->Stop(curr, 0,0,0);
		}
	}
	//m_Channel->Activate(true);
	m_Channel->SetVolume(m_Volume,0);
	
	HRESULT hr = m_pPerformance->PlaySegmentEx(pSegment, 
		NULL, NULL,
		DMUS_SEGF_QUEUE | DMUS_SEGF_SECONDARY,
		0, NULL,
		NULL,  // Segment to cue against.
		m_Channel);
	if (!FAILED(hr))
	{
		m_ActiveChannels[channel] = handle;
	}
}
	

