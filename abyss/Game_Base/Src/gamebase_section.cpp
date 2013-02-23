#include "../Include/gamebase_section.h"
#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

extern void Touch_InputConsoleSection();
extern void Touch_VFSSection();
extern void Touch_WindowSection();
extern void Touch_RenderSection();
extern void Touch_PreloaderSection();
extern void Touch_SoundsSection();

#define QUERY_MT_INFO_MAGIC					1
#define LAUNCH_THREADS_MAGIC				2
#define LAUNCH_VFS_SECTION_MAGIC			3
#define LAUNCH_WINDOW_SECTION_MAGIC			4
#define CREATE_WINDOW_MAGIC					5
#define LAUNCH_RENDER_SECTION_MAGIC			6
#define LAUNCH_PRELOADER_SECTION_MAGIC	    7
#define LAUNCH_INPUTCONSOLE_SECTION_MAGIC   8
#define LAUNCH_SOUNDS_SECTION_MAGIC			9 

CGameBaseSection::CGameBaseSection( long in_SectionID ) :
	CSection( in_SectionID )
	,m_VFSSectionID(SECTION_ID_ERROR)
	,m_WindowSectionID(SECTION_ID_ERROR)
	,m_RenderSectionID(SECTION_ID_ERROR)
	,m_InputConsoleSectionID(SECTION_ID_ERROR)
	,m_SoundsSectionID(SECTION_ID_ERROR)
	,m_HWnd(NULL)
	//,m_bClosing(false)
	,m_GameWindowWasClosed(false)
	,m_CanExitNow(false)
{
	CLog::Print("CGameBaseSection::CGameBaseSection()\n");
	//
	RegisterReaction( GetID((CCommand_SectionStartup*)NULL),			CTConform<CGameBaseSection,CCommand_SectionStartup>::Conform );
	RegisterReaction( GetID((CCommand_MTInfo*)NULL),					CTConform<CGameBaseSection,CCommand_MTInfo>::Conform );
	RegisterReaction( GetID((CCommand_LaunchThreadsResult*)NULL),		CTConform<CGameBaseSection,CCommand_LaunchThreadsResult>::Conform );
	RegisterReaction( GetID((CCommand_LaunchSectionResult*)NULL),		CTConform<CGameBaseSection,CCommand_LaunchSectionResult>::Conform );
	//
	RegisterReaction( GetID((CWindowSection_CreateWindowResult*)NULL),	CTConform<CGameBaseSection,CWindowSection_CreateWindowResult>::Conform );
	RegisterReaction( GetID((CWindowSection_WindowClosed*)NULL),		CTConform<CGameBaseSection,CWindowSection_WindowClosed>::Conform );
	RegisterReaction( GetID((CWindowSection_Activated*)NULL),			CTConform<CGameBaseSection,CWindowSection_Activated>::Conform );
	RegisterReaction( GetID((CWindowSection_WindowSize*)NULL),			CTConform<CGameBaseSection,CWindowSection_WindowSize>::Conform );
	//
	RegisterReaction( GetID((CRenderSection_BoundToWindow*)NULL),		CTConform<CGameBaseSection,CRenderSection_BoundToWindow>::Conform );
	RegisterReaction( GetID((CRenderSection_WindowClosedCleanupComplete*)NULL),		CTConform<CGameBaseSection,CRenderSection_WindowClosedCleanupComplete>::Conform );
	RegisterReaction( GetID((CRenderSection_3DEnvironmentInfo*)NULL),	CTConform<CGameBaseSection,CRenderSection_3DEnvironmentInfo>::Conform );
	RegisterReaction( GetID((CRenderSection_InitRenderResult*)NULL),	CTConform<CGameBaseSection,CRenderSection_InitRenderResult>::Conform );
	RegisterReaction( GetID((CRenderSection_CloseRenderResult*)NULL),	CTConform<CGameBaseSection,CRenderSection_CloseRenderResult>::Conform );
	//
	Touch_VFSSection();
	Touch_WindowSection();
	Touch_RenderSection();
	Touch_PreloaderSection();
	Touch_InputConsoleSection();
	Touch_SoundsSection();
}

CGameBaseSection::~CGameBaseSection()
{
	//CLog::Print("CGameBaseSection::~CGameBaseSection()\n");
}

void
CGameBaseSection::Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )
{
	CLog::Print("CGameBaseSection::Reaction( const CCommand_SectionStartup& )\n");
	//CTCommandSender<CCommand_QueryMTInfo>::SendCommand( SECTION_ID_SYSTEM, CCommand_QueryMTInfo(QUERY_MT_INFO_MAGIC) );
	CLog::Print("CGameBaseSection::Reaction( const CCommand_LaunchThreadsResult& )\n");
//	assert(in_rResult.m_Magic==LAUNCH_THREADS_MAGIC);
//	assert(in_rResult.m_Result==TLR_OK);
	if( GetNeedVFS() )
	{
		CTCommandSender<CCommand_LaunchSection>::SendCommand(
			SECTION_ID_SYSTEM,
			CCommand_LaunchSection(
				CVFSSection_CLSID,
				LAUNCH_VFS_SECTION_MAGIC
				//,SF_BOUND|SF_EXCLUSIVE
			)
		);
	}
	else
	{
		LaunchWindowSection();
	}

}

void
CGameBaseSection::Reaction( long in_SrcSectionID, const CCommand_MTInfo& in_rMTInfoCommand )
{
/*	CLog::Print("CGameBaseSection::Reaction( const CCommand_MTInfo& )\n");
	Received_MTInfo(in_rMTInfoCommand);
	long NThreadsToRun = GetNThreadsToRun();
	CLog::Print("  launching %d thread(s)\n",NThreadsToRun);
	CTCommandSender<CCommand_LaunchThreads>::SendCommand(
		SECTION_ID_SYSTEM,
		CCommand_LaunchThreads(NThreadsToRun,LAUNCH_THREADS_MAGIC)
	);*/
}

void
CGameBaseSection::Reaction( long in_SrcSectionID, const CCommand_LaunchThreadsResult& in_rResult )
{
	CLog::Print("CGameBaseSection::Reaction( const CCommand_LaunchThreadsResult& )\n");
	assert(in_rResult.m_Magic==LAUNCH_THREADS_MAGIC);
	assert(in_rResult.m_Result==TLR_OK);
	if( GetNeedVFS() )
	{
		CTCommandSender<CCommand_LaunchSection>::SendCommand(
			SECTION_ID_SYSTEM,
			CCommand_LaunchSection(
				CVFSSection_CLSID,
				LAUNCH_VFS_SECTION_MAGIC
				//,SF_BOUND|SF_EXCLUSIVE
			)
		);
	}
	else
	{
		LaunchWindowSection();
	}
}

void
CGameBaseSection::Reaction( long in_SrcSectionID, const CCommand_LaunchSectionResult& in_rResult )
{
	CLog::Print("CGameBaseSection::Reaction( const CCommand_LaunchSectionResult& )\n");
	if( (in_rResult.m_Magic==LAUNCH_PRELOADER_SECTION_MAGIC) // preloader section
		||(in_rResult.m_Magic==LAUNCH_VFS_SECTION_MAGIC) // vfs section
		|| (in_rResult.m_Magic==LAUNCH_WINDOW_SECTION_MAGIC) // window section
		|| (in_rResult.m_Magic==LAUNCH_RENDER_SECTION_MAGIC) // render section
		|| (in_rResult.m_Magic==LAUNCH_INPUTCONSOLE_SECTION_MAGIC ) // input section
		||  (in_rResult.m_Magic==LAUNCH_SOUNDS_SECTION_MAGIC)
	)
	{
		assert(in_rResult.m_ErrorCode==SLR_OK);
		CLog::Print("  in_rResult.m_NewSectionID = %ld\n",in_rResult.m_NewSectionID);
		if(in_rResult.m_Magic==LAUNCH_VFS_SECTION_MAGIC)
		{
			CLog::Print("  vfs section launched\n");
			m_VFSSectionID = in_rResult.m_NewSectionID;
			assert(m_VFSSectionID>=SECTION_ID_FIRST_AVAILABLE);
			if (GetNeedPreloader() ) 
			{
				LaunchPreloaderSection();
			}
			else 
			{
				LaunchWindowSection();
			}
		}
		else if (in_rResult.m_Magic == LAUNCH_SOUNDS_SECTION_MAGIC ) 
		{
			CLog::Print("  sounds section launched\n");
			m_SoundsSectionID = in_rResult.m_NewSectionID;
			assert(m_SoundsSectionID>=SECTION_ID_FIRST_AVAILABLE);
			CTCommandSender<CSoundsSection_InitSoundsRequest>::SendCommand(m_SoundsSectionID,
				CSoundsSection_InitSoundsRequest(m_HWnd));
			
		}
		else if (in_rResult.m_Magic== LAUNCH_PRELOADER_SECTION_MAGIC ) 
		{
			CLog::Print("  preloader section launched\n");
			m_PreloaderSectionID = in_rResult.m_NewSectionID;
			assert(m_PreloaderSectionID>=SECTION_ID_FIRST_AVAILABLE);
			LaunchWindowSection();
		}
		else if(in_rResult.m_Magic==LAUNCH_WINDOW_SECTION_MAGIC)
		{
			CLog::Print("  window section launched\n");
			m_WindowSectionID = in_rResult.m_NewSectionID;
			assert(m_WindowSectionID>=SECTION_ID_FIRST_AVAILABLE);
			CWindowParam GameWindowParams;
			GetGameWindowParams( GameWindowParams );
			CTCommandSender<CWindowSection_CreateWindow>::SendCommand(
				m_WindowSectionID,
				CWindowSection_CreateWindow(
					GameWindowParams.m_Style,
					GameWindowParams.m_ClientWidth,
					GameWindowParams.m_ClientHeight,
					GameWindowParams.m_Caption,
					CREATE_WINDOW_MAGIC
				)
			);
		}
		else if (in_rResult.m_Magic==LAUNCH_INPUTCONSOLE_SECTION_MAGIC)
		{
			CLog::Print("  input section launched\n");
			m_InputConsoleSectionID = in_rResult.m_NewSectionID;
			assert(m_RenderSectionID>=SECTION_ID_FIRST_AVAILABLE);
			CTCommandSender<CInputConsoleSection_InitInput>::SendCommand(
				m_InputConsoleSectionID, // no joystick but that's ok ?
				CInputConsoleSection_InitInput(m_HWnd, true, true, false)
			);

		}
		else if(in_rResult.m_Magic==LAUNCH_RENDER_SECTION_MAGIC)
		{
			CLog::Print("  render section launched\n");
			m_RenderSectionID = in_rResult.m_NewSectionID;
			assert(m_RenderSectionID>=SECTION_ID_FIRST_AVAILABLE);
			CTCommandSender<CRenderSection_BindToWindow>::SendCommand(
				m_RenderSectionID,
				CRenderSection_BindToWindow( m_WindowSectionID, m_HWnd )
			);
			LaunchSoundsSection();
			// send start to our console.
			CTCommandSender<CCommand_LaunchSection>::SendCommand(
				SECTION_ID_SYSTEM,
				CCommand_LaunchSection(
				CInputConsoleSection_CLSID,
					LAUNCH_INPUTCONSOLE_SECTION_MAGIC,
					SF_BOUND|SF_EXCLUSIVE // requesting separate thread for input as it is in lock mode most of the time
				)
			);

		}
	}
	else
	{
		Received_SectionLaunchResult( in_rResult );
	}
}



// ====================================================================================================================

void
CGameBaseSection::Reaction( long in_SrcSectionID, const CWindowSection_CreateWindowResult& in_rResult )
{
	CLog::Print("CGameBaseSection::Reaction( const CWindowSection_CreateWindowResult& )\n");
	assert(in_SrcSectionID==m_WindowSectionID);
	assert(in_rResult.m_MagicID==CREATE_WINDOW_MAGIC);
	assert(in_rResult.m_Result==CWR_OK);
	assert(in_rResult.m_HWnd!=NULL);
	m_HWnd = in_rResult.m_HWnd;
	CTCommandSender<CCommand_LaunchSection>::SendCommand(
		SECTION_ID_SYSTEM,
		CCommand_LaunchSection(
			CRenderSection_CLSID,
			LAUNCH_RENDER_SECTION_MAGIC
		)
	);
}

void
CGameBaseSection::Reaction( long in_SrcSectionID, const CWindowSection_WindowClosed& in_rCommand )
{
	//Recieved_WindowClosing();
	CLog::Print("CGameBaseSection::Reaction( const CWindowSection_WindowClosed& )\n");
	assert(m_HWnd!=NULL);
	assert(in_rCommand.m_HWnd==m_HWnd);
	assert( !m_GameWindowWasClosed );
	assert( !m_CanExitNow );
	m_GameWindowWasClosed = true;
	Event_WindowWasClosedByUser();
}

void
CGameBaseSection::CloseAllSubsystems() {
	assert( m_GameWindowWasClosed );
	assert( !m_CanExitNow );
	if( m_RenderSectionID != SECTION_ID_ERROR )
	{
		CTCommandSender<CRenderSection_WindowClosed>::SendCommand(
			m_RenderSectionID,
			CRenderSection_WindowClosed(m_HWnd)
		);
	}
	if( m_InputConsoleSectionID != SECTION_ID_ERROR )
	{
		CTCommandSender<CInputConsoleSection_StopInput>::SendCommand(
			m_InputConsoleSectionID , 
			CInputConsoleSection_StopInput()
		);
	}
}

void
CGameBaseSection::Reaction( long in_SrcSectionID, const CRenderSection_WindowClosedCleanupComplete& in_rCmd )
{
	assert( in_rCmd.m_HWnd==m_HWnd );
	assert( m_GameWindowWasClosed );
	assert( !m_CanExitNow );
	m_HWnd = NULL;
	m_CanExitNow = true;
	//Received_WindowClosed();
	Event_OkToExitNow();
}

void
CGameBaseSection::Reaction( long in_SrcSectionID, const CWindowSection_Activated& in_rCommand )
{
	CLog::Print("CGameBaseSection::Reaction( const CWindowSection_Activated& )\n");
}

void
CGameBaseSection::Reaction( long in_SrcSectionID, const CWindowSection_WindowSize& in_rCommand )
{
	CLog::Print("CGameBaseSection::Reaction( const CWindowSection_WindowSize& )\n");
}

// ====================================================================================================================

void
CGameBaseSection::Reaction( long in_SrcSectionID, const CRenderSection_BoundToWindow& in_rBound )
{
	CLog::Print("CGameBaseSection::Reaction( const CRenderSection_BoundToWindow& )\n");
	assert( in_rBound.m_HWnd == m_HWnd );
	CTCommandSender<CRenderSection_Get3DEnvironmentInfo>::SendCommand(
		m_RenderSectionID,
		CRenderSection_Get3DEnvironmentInfo()
	);
}

void
CGameBaseSection::Reaction( long in_SrcSectionID, const CRenderSection_3DEnvironmentInfo& in_rInfo )
{
	CLog::Print("CGameBaseSection::Reaction( const CRenderSection_3DEnvironmentInfo& )\n");
	assert(!in_rInfo.m_bError);
	CRenderSection_InitRender CmdInitRender;
	GetRenderInitParams( CmdInitRender );
	CTCommandSender<CRenderSection_InitRender>::SendCommand(
		m_RenderSectionID,
		CmdInitRender
	);
}

void
CGameBaseSection::Reaction( long in_SrcSectionID, const CRenderSection_InitRenderResult& in_rResult )
{
	CLog::Print("CGameBaseSection::Reaction( const CRenderSection_InitRenderResult& )\n");
	Received_InitRenderResult( in_rResult.m_Result );
	assert( in_rResult.m_Result == IRR_OK );
	//
	StartTheGame();
}

void
CGameBaseSection::Reaction( long in_SrcSectionID, const CRenderSection_CloseRenderResult& in_rResult )
{
	CLog::Print("CGameBaseSection::Reaction( const CRenderSection_CloseRenderResult& )\n");
	assert(false);
}

// ====================================================================================================================

inline
void
CGameBaseSection::LaunchWindowSection()
{
	CTCommandSender<CCommand_LaunchSection>::SendCommand(
		SECTION_ID_SYSTEM,
		CCommand_LaunchSection(
			CWindowSection_CLSID,
			LAUNCH_WINDOW_SECTION_MAGIC,
			SF_BOUND|SF_EXCLUSIVE
		)
	);
}


inline
void
CGameBaseSection::LaunchPreloaderSection()
{
	CTCommandSender<CCommand_LaunchSection>::SendCommand(
		SECTION_ID_SYSTEM,
		CCommand_LaunchSection(
			CPreloaderSection_CLSID,
			LAUNCH_PRELOADER_SECTION_MAGIC,
			SF_NONE
		)
	);
}
inline void 
CGameBaseSection::LaunchSoundsSection()
{
	CTCommandSender<CCommand_LaunchSection>::SendCommand(
		SECTION_ID_SYSTEM,
		CCommand_LaunchSection(
			CSoundsSection_CLSID,
			LAUNCH_SOUNDS_SECTION_MAGIC,
			SF_NONE
		)
	);
}





















