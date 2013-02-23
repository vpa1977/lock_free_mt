#include "../include/ds_manager.h"

#define BUFFER_SIZE 32000

////////// implementation class 


void CDSManager::Init(HWND hwd)
{
    HRESULT             hr;

    RELEASE( m_pDS );

    // Create IDirectSound using the primary sound device
    if( FAILED( hr = DirectSoundCreate8( NULL, &m_pDS, NULL ) ) )
        DXTRACE_ERR( TEXT("DirectSoundCreate8"), hr );

	DWORD dwCoopLevel = DSSCL_PRIORITY;
    // Set DirectSound coop level 
    if( FAILED( hr = m_pDS->SetCooperativeLevel( hwd, dwCoopLevel ) ) )
        DXTRACE_ERR( TEXT("SetCooperativeLevel"), hr );
    
	DWORD dwPrimaryChannels = 2;
	DWORD dwPrimaryFreq = 22050;
	DWORD dwPrimaryBitRate = 16;
    // Set primary buffer format
    if( FAILED( hr = SetPrimaryBufferFormat( dwPrimaryChannels, dwPrimaryFreq, dwPrimaryBitRate ) ) )
        DXTRACE_ERR( TEXT("SetPrimaryBufferFormat"), hr );

    

}
void CDSManager::CreateWaveSegment(long handle, const std::vector<unsigned char>& data)
{
	CDSound* s = new CDSound(data, m_pDS);
	assert(s->isOK());
	if (s->isOK()) 
	{
		m_SoundMap[handle] = PSound(s);
	}
	else
	{
		delete s;
		assert(0);
	}
}
void CDSManager::RemoveSegment(long handle)
{
	SoundMap::iterator it = m_SoundMap.find(handle);
	if (it != m_SoundMap.end() ) 
	{
		m_SoundMap.erase(handle);
	}
}

void CDSManager::Update()
{
	SoundMap::iterator it = m_SoundMap.begin();
	while (it != m_SoundMap.end() ) 
	{
		it->second->Update();
		it++;
	}
}
void CDSManager::PlayWaveSegment(long handle, unsigned short channel)
{
	SoundMap::iterator it = m_SoundMap.find(handle);
	if (it != m_SoundMap.end() ) 
	{
		it->second->Play();
	}
	else
	{
		// some error here
	}
}
void CDSManager::SetVolume(long volume)
{
	SoundMap::iterator it = m_SoundMap.begin();
	while (it != m_SoundMap.end() ) 
	{
		it->second->SetVolume(volume);
		it++;
	}
	
}
void CDSManager::SetVolume(long handle,long volume)
{
	SoundMap::iterator it = m_SoundMap.find(handle);
	if (it != m_SoundMap.end() ) 
	{
		it->second->SetVolume(volume);
	}
	else
	{
		// some error here
	}

}



HRESULT CDSManager::SetPrimaryBufferFormat( DWORD dwPrimaryChannels, 
                                               DWORD dwPrimaryFreq, 
                                               DWORD dwPrimaryBitRate )
{
    HRESULT             hr;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    if( m_pDS == NULL )
        return CO_E_NOTINITIALIZED;

    // Get the primary buffer 
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;
       
    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) )
        return DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );

    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = WAVE_FORMAT_PCM; 
    wfx.nChannels       = (WORD) dwPrimaryChannels; 
    wfx.nSamplesPerSec  = dwPrimaryFreq; 
    wfx.wBitsPerSample  = (WORD) dwPrimaryBitRate; 
    wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
        return DXTRACE_ERR( TEXT("SetFormat"), hr );

    RELEASE( pDSBPrimary );

    return S_OK;
}

//////////////////////////////////////////////////////////
CDSound::CDSound(const std::vector<unsigned char>& data , IDirectSound8* m_pDS)
{

	m_bOK = false;
	m_pStream = new CVFSStream(data);
	m_pdsBuffer = NULL;
	WavChunk chunk;
	DWORD dw;
	m_pStream->Read(&chunk, sizeof(WavChunk),&dw);
	// rest is pure data
	
	m_bIsActive = false;

    WAVEFORMATEX waveFormat;
    DSBUFFERDESC bufferDesc;
    ZeroMemory(&waveFormat, sizeof(WAVEFORMATEX));
    ZeroMemory(&bufferDesc, sizeof(DSBUFFERDESC));
    
    waveFormat.cbSize           = sizeof(WAVEFORMATEX);
	waveFormat.wFormatTag       = chunk.fmt.wFormatTag;
	waveFormat.nChannels        = chunk.fmt.wChannels;
	waveFormat.nSamplesPerSec   = chunk.fmt.dwSamplesPerSec;
	waveFormat.wBitsPerSample   = chunk.fmt.wBitsPerSample;
	waveFormat.nBlockAlign      = chunk.fmt.wBlockAlign;
	waveFormat.nAvgBytesPerSec  = chunk.fmt.dwAvgBytesPerSec;
	
    bufferDesc.dwSize           = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags          = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC | DSBCAPS_CTRLFREQUENCY;
	bufferDesc.dwBufferBytes    = BUFFER_SIZE;
    bufferDesc.lpwfxFormat      = &waveFormat;
    
	HRESULT hr;
    IDirectSoundBuffer *pdsBuffer;
    if (SUCCEEDED(hr = m_pDS->CreateSoundBuffer(&bufferDesc, &pdsBuffer, 0)))
    {
        if (SUCCEEDED(hr = pdsBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&m_pdsBuffer)))
        {
            void *pData;
            DWORD dwBytes;
            if (SUCCEEDED(hr = m_pdsBuffer->Lock(0, BUFFER_SIZE, &pData, &dwBytes, 0, 0, 0)))
            {
				DWORD res;
				m_pStream->Read(pData,dwBytes,&res);
				m_dwWritePos = BUFFER_SIZE;
                m_pdsBuffer->Unlock(pData, dwBytes, 0, 0);
                pdsBuffer->Release();
				m_bLooped = false;
				m_bOK = true;
                return;
            }
        }
        pdsBuffer->Release();
    }
	m_pdsBuffer = NULL;
	
}

void CDSound::Update() 
{
	if (!m_bIsActive)
	{
		return;
	}
	if(!m_bOK)
	{
		return;
	}

    void *pPtr1, *pPtr2;
    DWORD dwSize1, dwSize2;
    DWORD dwReadPos, dwSize;
	
    HRESULT hr;
    if (SUCCEEDED(hr = m_pdsBuffer->GetCurrentPosition(&dwReadPos, 0)))
    {
        if (dwReadPos > m_dwWritePos)
        {
            dwSize = dwReadPos - m_dwWritePos;
        }
        else
        {
            dwSize = BUFFER_SIZE  - m_dwWritePos + dwReadPos;
        }
        if (!dwSize)
        {
            return;
        }
        hr = m_pdsBuffer->Lock(m_dwWritePos, dwSize, &pPtr1, &dwSize1, &pPtr2, &dwSize2, 0);
        if (DSERR_BUFFERLOST == hr)
        {
            m_pdsBuffer->Restore();
            hr = m_pdsBuffer->Lock(m_dwWritePos, dwSize, &pPtr1, &dwSize1, &pPtr2, &dwSize2, 0);
        }
        m_dwWritePos = dwReadPos;
        if (SUCCEEDED(hr))
        {
            
			DWORD nDecoded;
			m_pStream->Read(pPtr1,dwSize1, &nDecoded);
			
            if (nDecoded != dwSize1 && nDecoded != 0)
            {
                FillMemory((char*)pPtr1 + nDecoded, dwSize1 - nDecoded, 0);
                if (m_bLooped)
                {
					ULARGE_INTEGER res;
					LARGE_INTEGER off;
					off.LowPart = sizeof(WavChunk);

					m_pStream->Seek(off,FILE_BEGIN,&res);
                }
                else
                {
                    Stop();
                }
            }
            if (pPtr2)
            {
                m_pStream->Read(pPtr2, dwSize2,&nDecoded);
                if (nDecoded != dwSize2 && nDecoded != 0)
                {
                    FillMemory((char*)pPtr1 + nDecoded, dwSize2 - nDecoded, 0);
                    if (m_bLooped)
                    {
                        ULARGE_INTEGER res;
						LARGE_INTEGER off;
						off.LowPart = sizeof(WavChunk);
						m_pStream->Seek(off,FILE_BEGIN,&res);
                    }
                    else
                    {
                        Stop();
                    }
                }
            }
            m_pdsBuffer->Unlock(pPtr1, dwSize1, pPtr2, dwSize2);
        }
    }
}


void CDSound::Play()
{
	//if (!isOK()) return;
if (m_bIsActive) 
	{
		Stop();
	}
	
	 m_pdsBuffer->Play(0, 0, DSBPLAY_LOOPING);
	 m_bIsActive = true;
}
void CDSound::Stop()
{
	m_bIsActive = false;
	m_pdsBuffer->Stop();
    m_pdsBuffer->SetCurrentPosition(0);
	ULARGE_INTEGER res;
	LARGE_INTEGER off;
	off.LowPart = sizeof(WavChunk);
	m_pStream->Seek(off,FILE_BEGIN,&res);
}
