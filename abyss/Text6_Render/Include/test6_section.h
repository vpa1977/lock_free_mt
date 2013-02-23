#ifndef TEST6_SECTION_HPP_INCLUDED
#define TEST6_SECTION_HPP_INCLUDED

#include "test6_section_interface.h"
#include "../../Game_Base/Include/gamebase_section.h"

class CTest6Section :
	public CGameBaseSection
	// window section commands
	,public CTCommandSender<CWindowSection_WindowName>
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
	,public CTCommandSender<CTest6Section_Render>
{
public:
	CTest6Section( long in_SectionID );
	~CTest6Section();
	std::string GetName()
	{
		return "CTest6Section";
	}
	void Reaction( long in_SrcSectionID, const CRenderSection_NewFrame& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_CreateVertexBuffer_Response& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_CreateTexture_Response& in_rCmd );
	void Reaction( long in_SrcSectionID, const CRenderSection_CreatePixelShader_Response& in_rCmd );
	void Reaction( long in_SrcSectionID, const CTest6Section_Render& in_rCmd );

protected:
	void Received_MTInfo( const CCommand_MTInfo& in_rInfo );
	long GetNThreadsToRun();
	bool GetNeedVFS();
	bool GetNeedPreloader() { return true;};
	void Received_SectionLaunchResult( const CCommand_LaunchSectionResult& );
	void GetGameWindowParams( CWindowParam& );
	void Received_3DInfo( const CRenderSection_3DEnvironmentInfo& );
	void GetRenderInitParams( CRenderSection_InitRender& );
	void Received_InitRenderResult( INIT_RENDER_RESULT in_bResult );
	void Received_WindowClosed();
	void StartTheGame();

private:
	long m_NThreadsToRun;
	CRenderSection_3DEnvironmentInfo m_3DInfo;

	unsigned long m_NFramesToRender;
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
	unsigned long m_TextureHandle;
	unsigned long m_PSHandle;
};

#endif