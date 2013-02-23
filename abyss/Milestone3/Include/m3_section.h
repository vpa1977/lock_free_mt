#ifndef M3_SECTION_HPP_INCLUDED
#define M3_SECTION_HPP_INCLUDED

#include "../../Game_Base/Include/gamebase_section.h"
#include "m3_section_interface.h"
#include "load_proxy.h"
#include "render_proxy.h"
#include "m3_world.h"

#include "m3_window_params.h"

class CM3Section :
	public CGameBaseSection
	,protected CLoadProxy
	,protected CRenderProxy
	,public CGameObjectOwner
	// system
	//,public CTCommandSender< CCommand_QuitApplication >
	,public CTCommandSender<CCommand_SetTimer_Request>
	// vfs
	,public CTCommandSender< CVFSSection_MountRequest >
	,public CTCommandSender< CVFSSection_UMountRequest >
	,public CTCommandSender< CVFSSection_ReadRequest >
	,public CTCommandSender< CVFSSection_OpenRequest >
	,public CTCommandSender< CVFSSection_CloseRequest >
	// preloader
	,public CTCommandSender< CPreloaderSection_DecodeRequest >
	,public CTCommandSender< CPreloaderSection_FileDecodeRequest >
	// window
	,public CTCommandSender< CWindowSection_WindowName >
	// render
	,public CTCommandSender< CRenderSection_PresentAndClear >
	,public CTCommandSender< CRenderSection_SetCamera >
	,public CTCommandSender< CRenderSection_SetAmbient >
	,public CTCommandSender< CRenderSection_SetDirectionalLight >
	,public CTCommandSender< CRenderSection_SetPointLight >
	,public CTCommandSender< CRenderSection_RenderVB >
	,public CTCommandSender< CRenderSection_RenderString >
	,public CTCommandSender< CRenderSection_RenderDot >
	,public CTCommandSender< CRenderSection_CreateFont_Request >
	,public CTCommandSender< CRenderSection_CreatePixelShader_Request >
	,public CTCommandSender< CRenderSection_CreateTexture_Request >
	,public CTCommandSender< CRenderSection_UpdateVertexBuffer_Request >
	,public CTCommandSender< CRenderSection_CreateVertexBuffer_Request >
	// self
	,public CTCommandSender< CM3Section_WaitingForPreloadToComplete >
	,public CTCommandSender< CM3Section_StartRender >
	,public CTCommandSender< CSoundsSection_PlaySoundRequest>
	,public CTCommandSender< CSoundsSection_OpenSoundRequest>
	,public CTCommandSender< CSoundsSection_SetVolumeRequest>
{
public:
	CM3Section( long in_SectionID );
	~CM3Section();
	std::string GetName()
	{
		return "CM3Section( Milestone 3 )";
	}

	// virtual from CGameObjectOwner
	void SomeObjectPreloadComplete( bool in_bSuccess, void* in_Handle );

	// virtual from CLoadProxy
protected:
	void ReadFile( const char* in_pszFileName, CLoadUser* in_pUser );
	void DecodeImageFromFile( const char* in_pszFileName, CLoadUser* in_pUser );
	void DecodeRawImage( IMAGE_TYPE in_Type, const std::vector<unsigned char> in_rData, CLoadUser* in_pUser );

	// virtual from CLoadProxy
protected:
	void CreateVB( const CRenderSection_CreateVertexBuffer_Request&, CRenderUser* );
	void UpdateVB( const CRenderSection_UpdateVertexBuffer_Request&, CRenderUser* );
	void CreateTexture( const CRenderSection_CreateTexture_Request&, CRenderUser* );
	void CreatePS( const CRenderSection_CreatePixelShader_Request&, CRenderUser* );
	void CreateFont(const CFontDescription & , CRenderUser * );
	void RenderVB( const CRenderSection_RenderVB&/*, CRenderUser* */);
	void SetAmbient(unsigned long ambient );
	void SetDirectionalLight( const CVector& in_Dir, float in_R, float in_G, float in_B );
	void SetCamera( const CRenderSection_SetCamera& );
	void RenderText(	long in_X, long in_Y,
						const std::string& in_rString,
						unsigned long in_FontHandle,
						unsigned long in_Color /*,
						CRenderUser* in_pUser	*/);
	void Present( unsigned long in_ClearColor );

public:
	// system
	void Reaction( long in_SrcSectionID, const CCommand_SetTimer_Response& );
	void Reaction( long in_SrcSectionID, const CCommand_Timer_Event& );
	// vfs
	void Reaction( long in_SrcSectionID, const CVFSSection_MountResponse& in_rMountResponse);
	void Reaction( long in_SrcSectionID, const CVFSSection_UMountResponse& in_rUMountResponse);
	void Reaction( long in_SrcSectionID, const CVFSSection_ReadResponse& in_rReadResponse);
	void Reaction( long in_SrcSectionID, const CVFSSection_OpenResponse& in_rOpenResponse);
	void Reaction( long in_SrcSectionID, const CVFSSection_CloseResponse& in_rCloseResponse);
	// vfs errors
	void Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_MountResponse>& in_rError );
	void Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_UMountResponse>& in_rError );
	void Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_ReadResponse>& in_rError );
	void Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_OpenResponse>& in_rError );
	void Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_CloseResponse>& in_rError );

	// preloader
	void Reaction( long in_SrcSectionID, const CPreloaderSection_DecodeError& );
	void Reaction( long in_SrcSectionID, const CPreloaderSection_DecodeResponse& );
	// window
	//
	// render
	void Reaction( long in_SrcSectionID, const CRenderSection_NewFrame& in_rCmd );
	// self
	void Reaction( long in_SrcSectionID , const CM3Section_WaitingForPreloadToComplete& );
	void Reaction( long in_SrcSectionID , const CM3Section_StartRender& );
	// input
	void Reaction( long in_SrcSectionID, const CInputConsoleSection_KeyboardUpdate& update );
	void Reaction( long in_SrcSectionID , const CInputConsoleSection_MouseUpdate& update );

	// render section responses
	void Reaction(long in_SrcSectionID , const CRenderSection_CreateVertexBuffer_Response& );
	void Reaction(long in_SrcSectionID , const CRenderSection_UpdateVertexBuffer_Response& );
	void Reaction(long in_SrcSectionID , const CRenderSection_CreateTexture_Response& );
	void Reaction(long in_SrcSectionID , const CRenderSection_CreatePixelShader_Response& );
	void Reaction(long in_SrcSectionID , const CRenderSection_CreateFont_Response& );


protected:
	void Received_MTInfo(const CCommand_MTInfo&);
	long GetNThreadsToRun();
	bool GetNeedVFS();
	bool GetNeedPreloader();
	//void Received_SectionLaunchResult(const CCommand_LaunchSectionResult&);
	void GetGameWindowParams( CWindowParam& );
	void Received_3DInfo( const CRenderSection_3DEnvironmentInfo& );
	void GetRenderInitParams( CRenderSection_InitRender& );
	void Received_InitRenderResult( INIT_RENDER_RESULT in_bResult );
	void Received_WindowClosed();
	void StartTheGame();

private:
	void StartPreload();

	long m_NThreadsToRun;
	CRenderSection_3DEnvironmentInfo m_3DInfo;

	unsigned long m_NFramesToRender;
	unsigned long m_FirstTimeFrame;
	unsigned long m_LastTimeFrame;
	unsigned long m_NFrames;
	void OneFrame();

	// game object
	CWorld* m_pWorld;
	bool m_bPreloadSuccessfull;
	bool m_bMouseButtonArmed;

};

#endif










