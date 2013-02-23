#ifndef GAMEBASE_SECTION_HPP_INCLUDED
#define GAMEBASE_SECTION_HPP_INCLUDED

// system
#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../MT_Core/Include/command_sender.h"

// what we use
#include "../../VFS/Include/section_vfs_interface.h "
#include "../../App_Win32/Include/window_section_interface.h"
#include "../../Render_DX8/Include/render_section_interface.h"
#include "../../Render_DX8/Include/render_section_init.h"
#include "../../Preloader/Include/section_preloader_interface.h"
#include "../../Input_and_Console/Include/input_console_section_interface.h"
#include "../../Sounds/Include/sounds_section_interface.h"

// our own interface
#include "gamebase_section_interface.h"

#pragma warning(disable:4250)

class CGameBaseSection :
	public CSection
	// system commands
	,public CTCommandSender<CCommand_QuitApplication>
	,public CTCommandSender<CCommand_LaunchSection>
	,public CTCommandSender<CCommand_QueryMTInfo>
	,public CTCommandSender<CCommand_LaunchThreads>
	// window section commands
	,public CTCommandSender<CWindowSection_CreateWindow>
	,public CTCommandSender<CWindowSection_CloseWindow>
	// render section commands
	,public CTCommandSender<CRenderSection_BindToWindow>
	,public CTCommandSender<CRenderSection_WindowClosed>
	,public CTCommandSender<CRenderSection_Get3DEnvironmentInfo>
	,public CTCommandSender<CRenderSection_InitRender>
	,public CTCommandSender<CRenderSection_CloseRender>
	// input registration
	,public CTCommandSender<CInputConsoleSection_InitInput>
	,public CTCommandSender<CInputConsoleSection_StopInput>
	// our own commands
	,public CTCommandSender<CSoundsSection_InitSoundsRequest>
{
public:
	CGameBaseSection( long in_SectionID );
	~CGameBaseSection();
	std::string GetName()
	{
		return "CGameBaseSection";
	}
	void Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand );
	void Reaction( long in_SrcSectionID, const CCommand_MTInfo& in_rMTInfoCommand );
	void Reaction( long in_SrcSectionID, const CCommand_LaunchThreadsResult& in_rResult );
	void Reaction( long in_SrcSectionID, const CCommand_LaunchSectionResult& in_rResult );
	//
	void Reaction( long in_SrcSectionID, const CWindowSection_CreateWindowResult& in_rResult );
	void Reaction( long in_SrcSectionID, const CWindowSection_WindowClosed& in_rCommand );
	void Reaction( long in_SrcSectionID, const CWindowSection_Activated& in_rCommand );
	void Reaction( long in_SrcSectionID, const CWindowSection_WindowSize& in_rCommand );
	//
	void Reaction( long in_SrcSectionID, const CRenderSection_BoundToWindow& in_rBound );
	void Reaction( long in_SrcSectionID, const CRenderSection_WindowClosedCleanupComplete& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_3DEnvironmentInfo& in_rInfo );
	void Reaction( long in_SrcSectionID, const CRenderSection_InitRenderResult& in_rResult );
	void Reaction( long in_SrcSectionID, const CRenderSection_CloseRenderResult& in_rResult );
	//
protected:
	virtual void Received_MTInfo(const CCommand_MTInfo&) = 0;
	virtual long GetNThreadsToRun() = 0;
	virtual bool GetNeedVFS() = 0;
	virtual bool GetNeedPreloader() = 0;
	virtual void Received_SectionLaunchResult(const CCommand_LaunchSectionResult&) {}
	struct CWindowParam
	{
		CWindowParam() : m_Style(WS_CONSTANT), m_ClientWidth(320), m_ClientHeight(240), m_Caption("") {}
		WINDOW_STYLE m_Style;
		long m_ClientWidth;
		long m_ClientHeight;
		std::string m_Caption;
	};
	virtual void GetGameWindowParams( CWindowParam& ) = 0;
	virtual void Received_3DInfo( const CRenderSection_3DEnvironmentInfo& ) = 0;
	virtual void GetRenderInitParams( CRenderSection_InitRender& ) = 0;
	virtual void Received_InitRenderResult( INIT_RENDER_RESULT in_bResult ) = 0;
	//virtual void Received_WindowClosed() = 0;
	
	
	virtual void StartTheGame() = 0;
	//bool IsClosing() { return m_bClosing;}
	long GetPreloaderSectionID() const { return m_PreloaderSectionID; }
	long GetVFSSectionID() const { return m_VFSSectionID; }
	long GetWindowSectionID() const { return m_WindowSectionID; }
	HWND GetHWND() const { return m_HWnd; }
	long GetRenderSectionID() const { return m_RenderSectionID; }
	long GetInputConsoleSectionID() const { return m_InputConsoleSectionID ; }
	long GetSoundSectionID() const { return m_SoundsSectionID ; }

	// exit sequence related stuff
	bool GameWindowWasClosed() const { return m_GameWindowWasClosed; }
	virtual void Event_WindowWasClosedByUser() {}
	void CloseAllSubsystems();
	bool CanExitNow() const { return m_CanExitNow; }
	virtual void Event_OkToExitNow() {}
	
private:
	//inline void Recieved_WindowClosing(){ m_bClosing  = true;}
	inline void LaunchSoundsSection();
	inline void LaunchWindowSection();
	inline void LaunchPreloaderSection();
	long m_VFSSectionID;
	long m_WindowSectionID;
	long m_RenderSectionID;
	long m_PreloaderSectionID;
	long m_InputConsoleSectionID;
	long m_SoundsSectionID;
	//bool m_bClosing;
	HWND m_HWnd;

	bool m_GameWindowWasClosed;
	bool m_CanExitNow;
};

#endif






























