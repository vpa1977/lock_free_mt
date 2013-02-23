#ifndef M4_SECTION_HPP_INCLUDED
#define M4_SECTION_HPP_INCLUDED

#include "../../Objects/Include/object.h"
#include "../../Objects/Include/e_static_mesh.h"
#include "../../Objects/Include/shadow.h"

#include "../../Game_Base/Include/gamebase_section.h"
#include "m4_section_interface.h"

#include "../../CERender/Include/renderer.h"
#include "../../CERender/Include/cegui_renderer_interface.h"




#define TEST_WINDOW_WIDTH	800
#define TEST_WINDOW_HEIGHT	600

using namespace CEGUI;

#include "m4_gui.h"

class CM4Section :
	public CGameBaseSection
	,public CTCommandSender<CEGUI_UpdateRequest>
	// system
//	,public CTCommandSender< CCommand_QuitApplication >
//	,public CTCommandSender<CCommand_SetTimer_Request>
	// vfs
	,public CTCommandSender<CRenderSection_RenderDot>
	,public CTCommandSender< CVFSSection_MountRequest >
//	,public CTCommandSender< CVFSSection_UMountRequest >
//	,public CTCommandSender< CVFSSection_ReadRequest >
//	,public CTCommandSender< CVFSSection_OpenRequest >
//	,public CTCommandSender< CVFSSection_CloseRequest >
	// preloader
	,public CTCommandSender< CPreloaderSection_ModelLoadRequest>
	,public CTCommandSender< CPreloaderSection_MatrixLoadRequest>
//	,public CTCommandSender< CPreloaderSection_DecodeRequest >
//	,public CTCommandSender< CPreloaderSection_FileDecodeRequest >
	// window
	,public CTCommandSender< CWindowSection_WindowName >
	// render
	,public CTCommandSender< CRenderSection_PresentAndClear >
	,public CTCommandSender< CRenderSection_SetCamera >
	,public CTCommandSender< CRenderSection_SetAmbient >
	,public CTCommandSender< CRenderSection_SetDirectionalLight >
//	,public CTCommandSender< CRenderSection_SetPointLight >
//	,public CTCommandSender< CRenderSection_RenderVB >
//	,public CTCommandSender< CRenderSection_RenderString >
//	,public CTCommandSender< CRenderSection_RenderDot >
	,public CTCommandSender< CRenderSection_RenderObject_Request >
	,public CTCommandSender<CEGUI_Render_InitRequest>
//	,public CTCommandSender< CRenderSection_CreateFont_Request >
//	,public CTCommandSender< CRenderSection_CreatePixelShader_Request >
//	,public CTCommandSender< CRenderSection_CreateTexture_Request >
//	,public CTCommandSender< CRenderSection_UpdateVertexBuffer_Request >
//	,public CTCommandSender< CRenderSection_CreateVertexBuffer_Request >
	// self
{
public:
	CM4Section( long in_SectionID );
	~CM4Section();
	std::string GetName()
	{
		return "CM4Section( Milestone 4 )";
	}

public:
	void Reaction(long in_SrcSectionID, const CEGUI_UpdateResponse& in ) ;

	// system
//	void Reaction( long in_SrcSectionID, const CCommand_SetTimer_Response& );
//	void Reaction( long in_SrcSectionID, const CCommand_Timer_Event& );
	// vfs
	void Reaction( long in_SrcSectionID, const CVFSSection_MountResponse& in_rMountResponse);
	void Reaction(long in_SrcSectionID, const CEGUI_Render_InitResponse& in ) ;
//	void Reaction( long in_SrcSectionID, const CVFSSection_UMountResponse& in_rUMountResponse);
//	void Reaction( long in_SrcSectionID, const CVFSSection_ReadResponse& in_rReadResponse);
//	void Reaction( long in_SrcSectionID, const CVFSSection_OpenResponse& in_rOpenResponse);
//	void Reaction( long in_SrcSectionID, const CVFSSection_CloseResponse& in_rCloseResponse);
	// vfs errors
//	void Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_MountResponse>& in_rError );
//	void Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_UMountResponse>& in_rError );
//	void Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_ReadResponse>& in_rError );
//	void Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_OpenResponse>& in_rError );
//	void Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_CloseResponse>& in_rError );

	// preloader
	void Reaction( long in_SrcSectionID, const CPreloaderSection_DecodeError& );
	void Reaction( long in_SrcSectionID, const CPreloaderSection_ModelLoadResponse& );
	void Reaction( long in_SrcSectionID, const CPreloaderSection_MatrixLoadResponse& );
	// window
	//
	// render
	void Reaction( long in_SrcSectionID, const CRenderSection_NewFrame& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_RenderObject_Response& in_rCmd );
	// input
//	void Reaction( long in_SrcSectionID, const CInputConsoleSection_KeyboardUpdate& update );
	void Reaction( long in_SrcSectionID , const CInputConsoleSection_MouseUpdate& update );

	

	// render section responses
//	void Reaction(long in_SrcSectionID , const CRenderSection_CreateVertexBuffer_Response& );
//	void Reaction(long in_SrcSectionID , const CRenderSection_UpdateVertexBuffer_Response& );
//	void Reaction(long in_SrcSectionID , const CRenderSection_CreateTexture_Response& );
//	void Reaction(long in_SrcSectionID , const CRenderSection_CreatePixelShader_Response& );
//	void Reaction(long in_SrcSectionID , const CRenderSection_CreateFont_Response& );


protected:
	void Received_MTInfo(const CCommand_MTInfo&);
	long GetNThreadsToRun();
	bool GetNeedVFS();
	bool GetNeedPreloader();
	void Received_SectionLaunchResult(const CCommand_LaunchSectionResult&);
	void GetGameWindowParams( CWindowParam& );
	void Received_3DInfo( const CRenderSection_3DEnvironmentInfo& );
	void GetRenderInitParams( CRenderSection_InitRender& );
	void Received_InitRenderResult( INIT_RENDER_RESULT in_bResult );
	//void Received_WindowClosed();
	void Event_WindowWasClosedByUser();
	void Event_OkToExitNow();

	void StartTheGame();
	
private:
	
	void StartPreload();

	long m_NThreadsToRun;
	CRenderSection_3DEnvironmentInfo m_3DInfo;

	CShadowCreator* m_pShadowCreator;
	CEntity_StaticMesh* m_pE;
	CObject* m_pO;
	CObject* m_pLoadedObject;
	CBox m_Box;

	CObjectAnimationController *m_pController;

	unsigned long m_NFramesToRender;
	unsigned long m_FirstTimeFrame;
	unsigned long m_LastTimeFrame;
	unsigned long m_NFrames;
	void OneFrame();

	unsigned long m_NShadowsSentToRender;

	CMatrix m_ObjectTransform;
	float m_fAngleX;
	float m_fAngleY;
	float m_fScale;
	CVector m_Pos;
private:
	FrameWindow* fWnd;
	CObjectRendererProxy* m_pProxy;
	long m_ProxySectionID;
	boost::shared_ptr<CGuiManager> m_pManager;
	
};

#endif










