#ifndef SOUNDS_HPP_INCLUDED
#define SOUNDS_HPP_INCLUDED

#include <map>


#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../MT_Core/Include/command_sender.h"

#include "../../App_Win32/Include/window_section_interface.h"
#include "../../VFS/Include/section_vfs_interface.h"

#include "ds_manager.h"
#include "sounds_section_interface.h"
//#include "vorbis_decoder.h"



class CSoundsSection :
	public CSection,
	CTCommandSender<CSoundsSection_InitSoundsResponse>
	,CTCommandSender<CSoundsSection_OpenSoundResponse>
	,CTCommandSender<CSoundsSection_PlaySoundResponse>
	// system
	,public CTCommandSender<CCommand_SetTimer_Request>
	,public CTCommandSender<CVFSSection_ReadRequest>
	,public CTCommandSender<CVFSSection_CloseRequest>
	,public CTCommandSender<CVFSSection_OpenRequest>

{
public:
	CSoundsSection( long in_SectionID );
	~CSoundsSection(){};
	std::string GetName()
	{
		return "CSoundsSection";
	}

	// system
	void Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand );
	void Reaction( long in_SrcSectionID, const CCommand_SetTimer_Response& req);
	void Reaction( long in_SrcSectionID, const CCommand_Timer_Event& req);
	void Reaction( long in_SrcSectionID, const CWindowSection_Activated& in_rStartupCommand );

	void Reaction(long in_SrcSectionID,const CVFSSection_ReadResponse& in_rBufferResponse);
	void Reaction(long in_SrcSectionID,const CVFSSection_CloseResponse& in_rCloseResponse){};
	void Reaction(long in_SrcSectionID,const CVFSSection_OpenResponse& in_rCloseResponse);



	void Reaction( long in_SrcSectionID, const CSoundsSection_InitSoundsRequest& in_rStartupCommand );
	void Reaction( long in_SrcSectionID, const CSoundsSection_OpenSoundRequest&  );
	void Reaction( long in_SrcSectionID, const CSoundsSection_PlaySoundRequest&  );
	void Reaction( long in_SrcSectionID, const CSoundsSection_SetVolumeRequest& in_rCommand );
	
	

private:
	void OpenSound(long requester, long magic, const CVFSSection_ReadResponse& req);
	CDSManager m_SoundManager;
	std::map<long, long > m_Sounds; // main sound buffer
	
	std::map<std::string, long> m_OpeningSounds;
	std::map<long, long> m_OpenedSounds;
	std::map<long, long> m_Requesters;


	std::map<long, CSoundsSection_OpenSoundRequest > m_Requests;

//	std::map<long, VorbisStream> m_VorbisStreams;

};

#endif


















