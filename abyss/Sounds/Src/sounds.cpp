#include "../Include/sounds.h"
#include "../../MT_Core/Include/section_registrator.h"

static CTSectionRegistrator<CSoundsSection_CLSID,CSoundsSection> g_CSoundsSectionRegistrator(SINGLE_INSTANCE_ONLY);

void Touch_SoundsSection()
{
}


CSoundsSection::CSoundsSection(long in_SectionID ) : 
CSection(in_SectionID)

{
	RegisterReaction( GetID((CCommand_SectionStartup*)NULL), CTConform<CSoundsSection,CCommand_SectionStartup>::Conform );
	RegisterReaction( GetID((CCommand_SetTimer_Response*)NULL), CTConform<CSoundsSection,CCommand_SetTimer_Response>::Conform );
	RegisterReaction( GetID((CCommand_Timer_Event*)NULL), CTConform<CSoundsSection,CCommand_Timer_Event>::Conform );
	RegisterReaction( GetID((CWindowSection_Activated*)NULL),  CTConform<CSoundsSection,CWindowSection_Activated>::Conform );


	RegisterReaction( GetID((CVFSSection_OpenResponse*)NULL),  CTConform<CSoundsSection,CVFSSection_OpenResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_ReadResponse*)NULL),  CTConform<CSoundsSection,CVFSSection_ReadResponse>::Conform );
	

	// sounds specfic reactions
	RegisterReaction( GetID((CSoundsSection_InitSoundsRequest*)NULL),  CTConform<CSoundsSection,CSoundsSection_InitSoundsRequest>::Conform );
	RegisterReaction( GetID((CSoundsSection_OpenSoundRequest*)NULL),  CTConform<CSoundsSection,CSoundsSection_OpenSoundRequest>::Conform );
	RegisterReaction( GetID((CSoundsSection_PlaySoundRequest*)NULL),  CTConform<CSoundsSection,CSoundsSection_PlaySoundRequest>::Conform );
	RegisterReaction( GetID((CSoundsSection_SetVolumeRequest*)NULL),  CTConform<CSoundsSection,CSoundsSection_SetVolumeRequest>::Conform );
}
void CSoundsSection::Reaction( long in_SrcSectionID, const CSoundsSection_SetVolumeRequest& in_rCommand )

{
	if (!in_rCommand.m_Handle)
	{
		m_SoundManager.SetVolume(in_rCommand.m_Volume);
	}
	else
	{
		m_SoundManager.SetVolume(in_rCommand.m_Handle,in_rCommand.m_Volume);
	}
}
// system
void CSoundsSection::Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand)
{
	CCommand_SetTimer_Request request(this->GetThisID(), 200);
		CTCommandSender<CCommand_SetTimer_Request>::SendCommand(
			SECTION_ID_SYSTEM,
			request
	);
	
}
void CSoundsSection::Reaction( long in_SrcSectionID, const CCommand_SetTimer_Response& req)
{
}
void CSoundsSection::Reaction( long in_SrcSectionID, const CCommand_Timer_Event& req)
{
/*
	
	std::map<long, VorbisStream>::iterator& it = m_VorbisStreams.begin();
	std::vector<int> done;

	while (it != m_VorbisStreams.end())
	{
		it->second.Update();
		if (it->second.isStopped())
		{
			done.push_back(it->first);
		}
	}

	for (int i = 0 ; i < done.size() ; i ++ ) 
	{
		m_VorbisStreams.erase(done[i]);
	}
*/
	m_SoundManager.Update();
	// update self every 200 ms.
	CCommand_SetTimer_Request request(this->GetThisID(), 200);
		CTCommandSender<CCommand_SetTimer_Request>::SendCommand(
			SECTION_ID_SYSTEM,
			request
	);
	
}

void CSoundsSection::Reaction( long in_SrcSectionID, const CWindowSection_Activated& in_rStartupCommand )
{
}

void CSoundsSection::Reaction( long in_SrcSectionID, const CSoundsSection_InitSoundsRequest& in_rReq )
{
	HRESULT hr = S_OK;
	m_SoundManager.Init(in_rReq.hWnd);
	bool bSuccess = ! FAILED(hr);
	CTCommandSender<CSoundsSection_InitSoundsResponse>::SendCommand(in_SrcSectionID, CSoundsSection_InitSoundsResponse(bSuccess));
}


void CSoundsSection::Reaction( long in_SrcSectionID, const CSoundsSection_OpenSoundRequest& req )
{
	CTCommandSender<CVFSSection_OpenRequest>::SendCommand( req.m_VFS , 
		CVFSSection_OpenRequest(req.m_WaveFile));
	

	m_OpeningSounds[req.m_WaveFile] =req.m_Magic;
	m_Requesters[req.m_Magic] = in_SrcSectionID;
	m_Requests[in_SrcSectionID] = req;
}

void CSoundsSection::Reaction( long in_SrcSectionID, const CSoundsSection_PlaySoundRequest& req )
{
	if (m_Sounds.find(req.m_Handle) != m_Sounds.end() ) 
	{
		m_SoundManager.PlayWaveSegment(req.m_Handle, req.m_Channel);
	}
/*(	else
	if (m_VorbisStreams.find(req.m_Handle) != m_VorbisStreams.end()) 
	{
		m_VorbisStreams.find(req.m_Handle)->second.Play();
	}
	*/
	else
	assert(0);
}



void CSoundsSection::Reaction(long in_SrcSectionID,const CVFSSection_ReadResponse& in_rBufferResponse)
{
	CTCommandSender<CVFSSection_CloseRequest>::SendCommand(in_SrcSectionID, CVFSSection_CloseRequest(in_rBufferResponse.m_Handle));
	std::map<long, long>::iterator it =  m_OpenedSounds.find(in_rBufferResponse.m_Handle);
	if (it != m_OpenedSounds.end() ) 
	{
		long requester = m_Requesters.find(it->second)->second;
		
		OpenSound(requester,it->second ,in_rBufferResponse);
		m_OpenedSounds.erase(in_rBufferResponse.m_Handle);
		
	}
	else
	{
		assert(0);
	}
	
}


void CSoundsSection::Reaction(long in_SrcSectionID,const CVFSSection_OpenResponse& in_rOpenResponse)
{

	std::map<std::string, long>::iterator& it =  m_OpeningSounds.find(in_rOpenResponse.m_Url);
	if (it != m_OpeningSounds.end() ) 
	{
		m_OpenedSounds[in_rOpenResponse.m_Handle] = it->second;
		m_OpeningSounds.erase(in_rOpenResponse.m_Url);
		
		CTCommandSender<CVFSSection_ReadRequest>::SendCommand( in_SrcSectionID, 
			CVFSSection_ReadRequest(in_rOpenResponse.m_Handle));
	}
	else
	{ 
		assert(0);
	}
}



void CSoundsSection::OpenSound(long requester, long magic ,const CVFSSection_ReadResponse& in_rBufferResponse)
{
		
		if (m_Requests.find(requester) == m_Requests.end())
		{
			assert(0);
		}

		CSoundsSection_OpenSoundRequest req = m_Requests.find(requester)->second;
		


		// this is wave and we create wave sound
		if (req.isWave() ) 
		{
			m_Sounds[magic] =magic;
			m_SoundManager.CreateWaveSegment(magic, in_rBufferResponse.m_pData );
		}
		else
		/*if (req.isOgg() )
		{
			VorbisStream vs;
			m_VorbisStreams[magic] = vs;
			m_VorbisStreams.find(magic)->second.Create(m_SoundManager.GetPerformance(), in_rBufferResponse.m_pData);
		}
		else*/
		{
			assert(0);
		}
		CTCommandSender<CSoundsSection_OpenSoundResponse>::SendCommand(requester, 
				CSoundsSection_OpenSoundResponse(magic,magic ));
		return;
}
