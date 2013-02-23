#ifndef WINDOW_SECTION_HPP_INCLUDED
#define WINDOW_SECTION_HPP_INCLUDED

#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../MT_Core/Include/command_sender.h"
#include "window_section_interface.h"

#pragma warning(disable:4250)

#define WM_NEW_COMMAND_ARRIVED (WM_APP+10)

class CWindowSection :
	public CSection
	,public CTCommandSender<CCommand_WindowCreated>
	,public CTCommandSender<CCommand_WindowClosed>
	,public CTCommandSender<CWindowSection_CreateWindowResult>
	,public CTCommandSender<CWindowSection_CloseWindowResult>
	,public CTCommandSender<CWindowSection_WindowParamsResult>
	,public CTCommandSender<CWindowSection_WindowClosed>
	,public CTCommandSender<CWindowSection_Char>
	,public CTCommandSender<CWindowSection_Activated>
	,public CTCommandSender<CWindowSection_WindowSize>
{
public:
	void IdleProcImpl();
	void KickProcImpl();

	CWindowSection( long in_SectionID );
	~CWindowSection();
	std::string GetName()
	{
		return "CWindowSection";
	}
	void Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand );
	void Reaction( long in_SrcSectionID, const CWindowSection_CreateWindow& in_rCreateWindow );
	void Reaction( long in_SrcSectionID, const CWindowSection_CloseWindow& in_rCloseWindow );
	void Reaction( long in_SrcSectionID, const CWindowSection_WindowParams& in_rWindowParams );
	//void Reaction( long in_SrcSectionID, const CCommand_WindowSectionBump& in_rBump ); -- no longer used. 
	// see 
	void Reaction( long in_SrcSectionID, const CWindowSection_WindowName& in_rName );
	void Reaction( long in_SrcSectionID, const CWindowSection_TestCommand& in_rTest );
	void SendQuitMessage();
private:
	void PeekMessage();
	void ProcessMessage(MSG& msg);

	//
	std::string m_WindowName;
	bool m_bNameChanged;
	long m_MagicID;
	HWND m_Handle;
	HWND m_PreviousHandle;
	void GetStylePositionAndDimensions(	WINDOW_STYLE in_WS,
										long in_ClientWidth, long in_ClientHeight,
										DWORD& out_rStyle,
										long& out_rX, long& out_rY,
										long& out_rDX, long& out_rDY
									);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	//
	long m_bResizing;
	long m_LastWidth;
	long m_LastHeight;

	static CWindowSection* m_pInstance;
	void SendActivated( bool in_bActive );
};

#endif













