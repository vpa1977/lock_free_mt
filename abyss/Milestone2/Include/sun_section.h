#ifndef SUN_SECTION_HPP_INCLUDED
#define SUN_SECTION_HPP_INCLUDED

#include "demo_section_interface.h"
#include "sun_section_interface.h"

#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/command_sender.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../Preloader/Include/section_preloader_interface.h"
#include "../../VFS/Include/section_vfs_interface.h"
#include "../../Render_DX8/Include/render_section_interface.h"

#pragma warning(disable:4250)

class CSunSection :
	public CSection
	// system
	,public CTCommandSender<CCommand_SetTimer_Request>
	// demo section
	,public CTCommandSender<CDemoSection_WorkerInit_Response>
	,public CTCommandSender<CDemoSection_WorkerUpdate>
	,public CTCommandSender<CDemoSection_WorkerCameraInform_Response>
	,public CTCommandSender<CDemoSection_WorkerShutdown_Response>
	// preloader
	,public CTCommandSender<CPreloaderSection_FileDecodeRequest>
	// vfs
	,public CTCommandSender<CVFSSection_OpenRequest>
	,public CTCommandSender<CVFSSection_ReadRequest>
	,public CTCommandSender<CVFSSection_CloseRequest>
	// render
	,public CTCommandSender<CRenderSection_CreateVertexBuffer_Request>
	,public CTCommandSender<CRenderSection_UpdateVertexBuffer_Request>
	,public CTCommandSender<CRenderSection_ReleaseVertexBuffer_Request>
	,public CTCommandSender<CRenderSection_CreateTexture_Request>
	,public CTCommandSender<CRenderSection_ReleaseTexture_Request>
	,public CTCommandSender<CRenderSection_CreatePixelShader_Request>
	,public CTCommandSender<CRenderSection_ReleasePixelShader_Request>
{
public:
	CSunSection( long in_SectionID );
	~CSunSection();
	std::string GetName()
	{
		return "CSunSection";
	}

	void Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand );
	// system
	void Reaction( long in_SrcSectionID, const CCommand_SetTimer_Response& );
	void Reaction( long in_SrcSectionID, const CCommand_Timer_Event& );
	// demo section
	void Reaction( long in_SrcSectionID, const CDemoSection_WorkerInit_Request& );
	void Reaction( long in_SrcSectionID, const CDemoSection_WorkerCameraInform& );
	void Reaction( long in_SrcSectionID, const CDemoSection_WorkerShutdown_Request& );
	// preloader
	void Reaction( long in_SrcSectionID, const CPreloaderSection_DecodeError& );
	void Reaction( long in_SrcSectionID, const CPreloaderSection_DecodeResponse& );
	// vfs
	void Reaction( long in_SrcSectionID, const CVFSSection_OpenResponse& );
	void Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_OpenRequest>& );
	void Reaction( long in_SrcSectionID, const CVFSSection_ReadResponse& );
	void Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_ReadRequest>& );
	void Reaction( long in_SrcSectionID, const CVFSSection_CloseResponse& );
	void Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_CloseRequest>& );
	// render
	void Reaction( long in_SrcSectionID, const CRenderSection_CreateVertexBuffer_Response& );
	void Reaction( long in_SrcSectionID, const CRenderSection_UpdateVertexBuffer_Response& );
	void Reaction( long in_SrcSectionID, const CRenderSection_CreateTexture_Response& );
	void Reaction( long in_SrcSectionID, const CRenderSection_CreatePixelShader_Response& );

private:
	// external section ids
	long m_RenderSectionID;
	long m_VfsSectionID;
	long m_MainSectionID;
	long m_PreloaderSectionID;

	// urls 
	std::string m_ConfigUrl;
	std::string m_TextureUrl;

	// file handles.
	long m_ConfigHandle;
	long m_TextureHandle;

	// VB and PS handles
	unsigned long m_VBHandle;
	bool m_bVBCreateRequestSent;
	unsigned long m_PSHandle;
	unsigned long m_THandle;

	// sun params from config
	float m_SunRadius;
	unsigned long m_NParticles;
	float m_ParticleRadiusMin;
	float m_ParticleRadiusMax;
	float m_ParticleLifeMin;
	float m_ParticleLifeMax;
	float m_DeltaAge;

	// camera info
	CVector m_CameraRight;
	CVector m_CameraUp;
	bool m_bHaveCamera;

	// particles data
	struct CParticle
	{
		CVector m_Org;
		float m_Age;
		float m_DeltaAge;
		float m_Size;
		float m_Sin;
		float m_Cos;
	};
	std::vector< CParticle > m_Particles;
	void GenerateOneParticle( CParticle& rParticle, bool in_bRandomAge );

	void ParseConfig( const std::vector<unsigned char>& in_rData );
	void CheckForInitializationCompletion();
};


#endif























