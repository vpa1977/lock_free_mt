#ifndef RENDER_SECTION_HPP_INCLUDED
#define RENDER_SECTION_HPP_INCLUDED

#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../MT_Core/Include/command_sender.h"
#include "render_section_interface.h"
#include "rendercritical_section_interface.h"
#include "render_section_init.h"
#include "r8.h"

#pragma warning(disable:4250)

class CRenderSection;

//===========================================================================

class CRenderLayer_2
{
public:
	CRenderLayer_2();
	~CRenderLayer_2();
	void Initialize(CRenderSection* in_pHost);
	void Activate(long in_RenderCriticalSectionID);
	void Deactivate();
	bool IsActive() const;
	void Received_DeviceLost();
private:
	CRenderSection* m_pHost;
	bool m_bActive;
	long m_RenderCriticalSectionID;
};

class CRenderLayer_1
{
public:
	CRenderLayer_1();
	~CRenderLayer_1();
	void Initialize(CRenderSection* in_pHost, CRenderLayer_2* in_pLayer2);
	void Activate(long in_RenderCriticalSectionID);
	void Deactivate();
	bool IsActive() const { return m_bActive; }
	void Received_Get3DInfo( long in_RequesterOf3DInfo );
	void Received_3DInfo( const CRenderCriticalSection_3DEnvironmentInfo& in_rInfo );
	void Received_InitRender( long in_RequesterID, const CRenderSection_InitRender& in_rInit );
	void Received_InitRenderResult( const CRenderCriticalSection_InitRenderResult& in_rResult );
	void Received_CloseRender( long in_RequesterID );
	void Received_CloseRenderResult( const CRenderSection_CloseRenderResult& in_rResult );
private:
	CRenderSection* m_pHost;
	CRenderLayer_2* m_pLayer2;
	bool m_bActive;
	long m_RenderCriticalSectionID;
	long m_RequesterOf3DInfo;
	long m_RequesterOfInit;
	long m_RequesterOfClose;
};

//===========================================================================

class CRenderLayer_0
{
public:
	CRenderLayer_0();
	~CRenderLayer_0();
	void Initialize(CRenderSection* in_pHost, CRenderLayer_1* in_pLayer1);
	inline bool IsActive() { return true; } // layer 0 is always active
	void Received_BindToWindow( long in_CallerSectionID, long in_WindowSectionID, HWND in_HWnd );
	void Received_SectionLaunchResult( long in_RenderCriticalSectionID );
	void Received_RenderCriticalBoundToWindow();
	void Received_UnbindFromWindow( HWND in_HWnd );
private:
	enum LAYER0_STATE
	{
		STATE_0_JUST_INITIALIZED	= 0,
		STATE_1_SENT_LAUNCH			= 1,
		STATE_2_SENT_CRITICAL_BIND	= 2,
		STATE_3_BOUND				= 3
	};
	LAYER0_STATE m_State;
	CRenderSection* m_pHost;
	CRenderLayer_1* m_pLayer1;
	long m_WindowSectionID;
	long m_RenderCriticalSectionID;
	HWND m_HWnd;
	long m_CallerSectionID;
};

//===========================================================================

class CRenderSection :
	public CSection
	//
	,public CTCommandSender<CCommand_LaunchSection>
	//
	,public CTCommandSender<CRenderSection_BoundToWindow>
	,public CTCommandSender<CRenderSection_WindowClosedCleanupComplete>
	,public CTCommandSender<CRenderSection_3DEnvironmentInfo>
	,public CTCommandSender<CRenderSection_InitRender> // goes to render_critical section
	,public CTCommandSender<CRenderSection_InitRenderResult>
	,public CTCommandSender<CRenderSection_CloseRender> // goes to render_critical section
	,public CTCommandSender<CRenderSection_CloseRenderResult>
	,public CTCommandSender<CRenderSection_NewFrame>
	,public CTCommandSender<CRenderSection_CreateVertexBuffer_Response>
	,public CTCommandSender<CRenderSection_UpdateVertexBuffer_Response>
	,public CTCommandSender<CRenderSection_CreateTexture_Response>
	,public CTCommandSender<CRenderSection_CreatePixelShader_Response>
	,public CTCommandSender<CRenderSection_CreateFont_Response>
	,public CTCommandSender<CRenderSection_RenderObject_Response>
	//
	,public CTCommandSender<CRenderCriticalSection_BindToWindow>
	,public CTCommandSender<CRenderCriticalSection_UnbindAndDie>
	,public CTCommandSender<CRenderCriticalSection_Get3DEnvironmentInfo>
	,public CTCommandSender<CRenderCriticalSection_RestoreDevice>
{
public:
	CRenderSection( long in_SectionID );
	~CRenderSection();
	std::string GetName()
	{
		return "CRenderSection";
	}
	void Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand );
	void Reaction( long in_SrcSectionID, const CCommand_LaunchSectionResult& in_rLaunchResult );
	//
	void Reaction( long in_SrcSectionID, const CRenderSection_BindToWindow& in_rBind );
	void Reaction( long in_SrcSectionID, const CRenderSection_WindowClosed& in_rWindowClosed );
	void Reaction( long in_SrcSectionID, const CRenderSection_Get3DEnvironmentInfo& in_rGetInfo );
	void Reaction( long in_SrcSectionID, const CRenderSection_InitRender& in_rInit );
	void Reaction( long in_SrcSectionID, const CRenderSection_CloseRender& in_rInit );
	void Reaction( long in_SrcSectionID, const CRenderSection_CloseRenderResult& in_rInit );
	void Reaction( long in_SrcSectionID, const CRenderSection_PresentAndClear& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_SetCamera& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_SetAmbient& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_SetDirectionalLight& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_SetPointLight& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_CreateVertexBuffer_Request& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_UpdateVertexBuffer_Request& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_ReleaseVertexBuffer_Request& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_CreateTexture_Request& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_ReleaseTexture_Request& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_CreatePixelShader_Request& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_ReleasePixelShader_Request& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_CreateFont_Request& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_RenderTestObject& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_RenderVB& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_RenderString& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_RenderDot& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_RenderObject_Request& in_rCmd );
	//
	void Reaction( long in_SrcSectionID, const CRenderCriticalSection_BoundToWindow& in_rBound );
	void Reaction( long in_SrcSectionID, const CRenderCriticalSection_3DEnvironmentInfo& in_rInfo );
	void Reaction( long in_SrcSectionID, const CRenderCriticalSection_InitRenderResult& in_rResult );
	void Reaction( long in_SrcSectionID, const CRenderCriticalSection_RestoreDeviceComplete& in_rResult );
	//
private:
	friend class CRenderLayer_2;
	CRenderLayer_2 m_Layer2;
	void Send_RestoreDevice(long in_RenderCriticalSectionID);

	friend class CRenderLayer_1;
	CRenderLayer_1 m_Layer1;
	void Send_Get3DEnvironmentInfo(long in_RenderCriticalSectionID);
	void Send_3DEnvironmentInfo(long in_RequesterSectionID, const CRenderSection_3DEnvironmentInfo& in_rInfo );
	void Send_InitRender(long in_RenderCriticalSectionID, const CRenderSection_InitRender& in_rInit );
	void Send_InitRenderResult(long in_RequesterSectionID, const CRenderSection_InitRenderResult& in_rResult );
	void Send_CloseRender(long in_RenderCriticalSectionID );
	void Send_CloseRenderResult(long in_RequesterSectionID, const CRenderSection_CloseRenderResult& in_rResult );

	friend class CRenderLayer_0;
	CRenderLayer_0 m_Layer0;
	void Send_LaunchRenderCriticalSection(long in_WindowSectionID);
	void Send_BindRenderCriticalSection(long in_RenderCriticalSectionID, HWND in_HWnd);
	void Send_UnbindAndDieRenderCriticalSection(long in_RenderCriticalSectionID);
	void Send_RenderBound(long in_CallerSectionID, HWND in_HWnd);

	//long m_RenderCriticalSectionID;
	//long m_WindowSectionID;
	//HWND m_HWnd;
	//bool m_bTerminatingDueToWindowClosed;
	//long m_SectionThatUssuedWindowClosed;
	//void Send_WindowClosedCleanupComplete();

	//bool m_b3DInfoRequested;
	//long m_3DInfoRequesterID;

	//bool m_bInitRequested;
	//long m_InitRequesterID;

	//void Get3DInfo(HWND in_HWnd);

	bool m_bLost;
	bool m_bInsideResetAttempt;
	CRenderDX8 Render;

	void RenderShadowRecurse( CShadow* in_pShadow, const CMatrix& in_CumulativeParentPosition );
};

#define RENDER_CRITICAL_LAUNCH_MAGIC 666

#endif
