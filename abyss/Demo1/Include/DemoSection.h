#ifndef DEMOSECTION_HPP 

#define DEMOSECTION_HPP
// main demo section.

#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../MT_Core/Include/command_sender.h"
#include "../../MT_Core/Include/command_sender.h"

#include "../../Game_Base/Include/gamebase_section.h"
#include "../../Preloader/Include/section_preloader_interface.h"
#include "../../VFS/Include/section_vfs_interface.h"


class DemoSection : public CGameBaseSection, 
		CTCommandSender<CPreloaderSection_DecodeRequest>, 
		CTCommandSender<CPreloaderSection_FileDecodeRequest>, 
		CTCommandSender<CVFSSection_ReadRequest>,
		CTCommandSender<CVFSSection_MountRequest>,
		CTCommandSender<CVFSSection_OpenRequest>,
		CTCommandSender<CVFSSection_ExistsRequest>,
		
		public CTCommandSender<CWindowSection_WindowName>
		// render
		,public CTCommandSender<CRenderSection_PresentAndClear>
		,public CTCommandSender<CRenderSection_SetCamera>
		,public CTCommandSender<CRenderSection_SetAmbient>
		,public CTCommandSender<CRenderSection_SetDirectionalLight>
		,public CTCommandSender<CRenderSection_CreateVertexBuffer_Request>
		,public CTCommandSender<CRenderSection_ReleaseVertexBuffer_Request>
		,public CTCommandSender<CRenderSection_CreateTexture_Request>
		,public CTCommandSender<CRenderSection_ReleaseTexture_Request>
		,public CTCommandSender<CRenderSection_CreatePixelShader_Request>
		,public CTCommandSender<CRenderSection_ReleasePixelShader_Request>
		,public CTCommandSender<CRenderSection_RenderVB>
		// our own comands
		,public CTCommandSender<DemoSection_Render>



	
{

public:
	DemoSection( long in_SectionID );
	~DemoSection();

	std::string GetName()
	{
		return "DemoSection";
	}
// vfs handling
	void Reaction( long in_SrcSectionID, const CVFSSection_ExistsResponse& in_rRead );
	void Reaction( long in_SrcSectionID, const CVFSSection_ReadResponse& in_rRead );
	void Reaction( long in_SrcSectionID, const CVFSSection_MountResponse& in_rRead );
	void Reaction( long in_SrcSectionID, const CVFSSection_OpenResponse& in_rRead );
	void Reaction( long in_SrcSectionID, const CPreloaderSection_DecodeResponse& in_rRead );

	void Reaction( long in_SrcSectionID, const CRenderSection_NewFrame& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_CreateVertexBuffer_Response& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_CreateTexture_Response& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_CreatePixelShader_Response& in_rCmd );
	void Reaction( long in_SrcSectionID, const DemoSection_Render& in_rCmd );



protected:
	void StartTheGame();

	void Received_MTInfo(const CCommand_MTInfo&);
	long GetNThreadsToRun();
	bool GetNeedVFS();
	bool GetNeedPreloader() { return true; };
	void GetGameWindowParams( CWindowParam& );
	void Received_3DInfo( const CRenderSection_3DEnvironmentInfo& );
	void GetRenderInitParams( CRenderSection_InitRender& );
	void Received_InitRenderResult( INIT_RENDER_RESULT in_bResult );
	void Received_WindowClosed();

	virtual void Received_SectionLaunchResult(const CCommand_LaunchSectionResult&);

private:
	long m_ImageHandle, m_ObjectHandle;
	long m_hPreloader;
	long m_NThreadsToRun;
	CRenderSection_3DEnvironmentInfo m_3DInfo;

	std::vector<char> m_DataBuffer;

private:

	unsigned long m_NFramesToRender;
	unsigned long m_LastTimeFrame;
	unsigned long m_NFrames;
	void RenderFrame();
	float m_Alpha;

	unsigned long m_VBHandle;
	unsigned long m_PrimitiveType;
	unsigned long m_NVertices;
	unsigned long m_NPrimitives;
	CMatrix	m_M0;
	CMatrix	m_M1;
	CMatrix	m_M2;
	CMatrix	m_M3;
	CMatrix	m_M4;
	unsigned long m_TextureHandle;
	unsigned long m_PSHandle;


};





#endif


