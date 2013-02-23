#ifndef TEST5_SECTION_HPP_INCLUDED
#define TEST3_SECTION_HPP_INCLUDED

#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../MT_Core/Include/command_sender.h"
#include "../../App_Win32/Include/window_section_interface.h"
#include "../../Render_DX8/Include/render_section_interface.h"
#include "../../Render_DX8/Include/render_section_init.h"
#include "test5_section_interface.h"

#pragma warning(disable:4250)

class CTest5Section :
	public CSection
	// system commands
	,public CTCommandSender<CCommand_QuitApplication>
	,public CTCommandSender<CCommand_LaunchSection>
	,public CTCommandSender<CCommand_QueryMTInfo>
	,public CTCommandSender<CCommand_LaunchThreads>
	// window section commands
	,public CTCommandSender<CWindowSection_CreateWindow>
	,public CTCommandSender<CWindowSection_CloseWindow>
	,public CTCommandSender<CWindowSection_WindowParams>
	,public CTCommandSender<CWindowSection_WindowName>
	// render section commands
	,public CTCommandSender<CRenderSection_BindToWindow>
	,public CTCommandSender<CRenderSection_WindowClosed>
	,public CTCommandSender<CRenderSection_Get3DEnvironmentInfo>
	,public CTCommandSender<CRenderSection_InitRender>
	,public CTCommandSender<CRenderSection_CloseRender>
	,public CTCommandSender<CRenderSection_PresentAndClear>
	,public CTCommandSender<CRenderSection_SetCamera>
	,public CTCommandSender<CRenderSection_SetAmbient>
	,public CTCommandSender<CRenderSection_SetDirectionalLight>
	,public CTCommandSender<CRenderSection_CreateVertexBuffer_Request>
	,public CTCommandSender<CRenderSection_ReleaseVertexBuffer_Request>
	,public CTCommandSender<CRenderSection_RenderTestObject>
	,public CTCommandSender<CRenderSection_RenderVB>
	// own commands
	,public CTCommandSender<CTest5Section_Render>
{
public:
	CTest5Section( long in_SectionID );
	~CTest5Section();
	std::string GetName()
	{
		return "CTest5Section";
	}
	void Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand );
	void Reaction( long in_SrcSectionID, const CCommand_MTInfo& in_rMTInfoCommand );
	void Reaction( long in_SrcSectionID, const CCommand_LaunchThreadsResult& in_rResult );
	void Reaction( long in_SrcSectionID, const CCommand_LaunchSectionResult& in_rResult );
	//
	void Reaction( long in_SrcSectionID, const CWindowSection_CreateWindowResult& in_rResult );
	void Reaction( long in_SrcSectionID, const CWindowSection_WindowClosed& in_rCommand );
//	void Reaction( long in_SrcSectionID, const CWindowSection_Activated& in_rCommand );
//	void Reaction( long in_SrcSectionID, const CWindowSection_WindowSize& in_rCommand );
	//
	void Reaction( long in_SrcSectionID, const CRenderSection_BoundToWindow& in_rBound );
	void Reaction( long in_SrcSectionID, const CRenderSection_3DEnvironmentInfo& in_rInfo );
	void Reaction( long in_SrcSectionID, const CRenderSection_InitRenderResult& in_rResult );
	void Reaction( long in_SrcSectionID, const CRenderSection_CreateVertexBuffer_Response& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_CloseRenderResult& in_rResult );
	void Reaction( long in_SrcSectionID, const CRenderSection_NewFrame& in_rCmd );
//	void Reaction( long in_SrcSectionID, const CRenderSection_WindowClosedCleanupComplete& in_rCommand );
	//
	void Reaction( long in_SrcSectionID, const CTest5Section_Render& in_rCmd );
private:
	long m_WindowSectionID;
	long m_RenderSectionID;
	HWND m_HWnd;

	unsigned long m_NFramesToRender;
	unsigned char m_Color;
	unsigned long m_LastTimeFrame;
	unsigned long m_NFrames;
	void RenderFrame();
	float m_Alpha;

	bool LoadDataFromFile( const char* in_pszFileName, CVBFormat& out_rVBFormat, std::vector<unsigned char>& out_rData, unsigned long& out_rPrimitiveType, unsigned long& out_rNPrimitives );
	unsigned long m_VBHandle;
	unsigned long m_PrimitiveType;
	unsigned long m_NVertices;
	unsigned long m_NPrimitives;
	CMatrix	m_M0;
	CMatrix	m_M1;
	CMatrix	m_M2;
	CMatrix	m_M3;
	CMatrix	m_M4;
};

#endif
