#ifndef OBJECT_SECTION_HPP_INCLUDED
#define OBJECT_SECTION_HPP_INCLUDED

#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/command_sender.h"

#include "../../MT_Core/Include/system_commands.h"
#include "demo_section_interface.h"
#include "../../Preloader/Include/section_preloader_interface.h"
#include "../../VFS/Include/section_vfs_interface.h"
#include "../../Render_DX8/Include/render_section_interface.h"
#include "object_section_interface.h"

class CObjectSection :
	public CSection
	// system
	,public CTCommandSender<CCommand_LaunchSection>
	,public CTCommandSender<CCommand_SetTimer_Request>
	// demo section
	,public CTCommandSender<CDemoSection_WorkerInit_Request>
	,public CTCommandSender<CDemoSection_WorkerInit_Response>
	,public CTCommandSender<CDemoSection_WorkerUpdate>
	,public CTCommandSender<CDemoSection_WorkerShutdown_Response>
	,public CTCommandSender<CDemoSection_WorkerShutdown_Request>
	// preloader
	,public CTCommandSender<CPreloaderSection_FileDecodeRequest>
	// vfs
	,public CTCommandSender<CVFSSection_OpenRequest>
	,public CTCommandSender<CVFSSection_ReadRequest>
	,public CTCommandSender<CVFSSection_CloseRequest>
	// render
	,public CTCommandSender<CRenderSection_CreateVertexBuffer_Request>
	,public CTCommandSender<CRenderSection_ReleaseVertexBuffer_Request>
	,public CTCommandSender<CRenderSection_CreateTexture_Request>
	,public CTCommandSender<CRenderSection_ReleaseTexture_Request>
	,public CTCommandSender<CRenderSection_CreatePixelShader_Request>
	,public CTCommandSender<CRenderSection_ReleasePixelShader_Request>
{
public:
	CObjectSection( long in_SectionID );
	~CObjectSection();
	std::string GetName()
	{
		return "CObjectSection";
	}

	void Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand );
	void Reaction( long in_SrcSectionID, const CCommand_LaunchSectionResult& in_rResult );
	// system
	void Reaction( long in_SrcSectionID, const CCommand_SetTimer_Response& );
	void Reaction( long in_SrcSectionID, const CCommand_Timer_Event& );
	// demo section
	void Reaction( long in_SrcSectionID, const CDemoSection_WorkerInit_Request& );
	void Reaction( long in_SrcSectionID, const CDemoSection_WorkerInit_Response& );
	void Reaction( long in_SrcSectionID, const CDemoSection_WorkerShutdown_Request& );
	void Reaction( long in_SrcSectionID, const CDemoSection_WorkerUpdate& );
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
	void Reaction( long in_SrcSectionID, const CRenderSection_CreateTexture_Response& );
	void Reaction( long in_SrcSectionID, const CRenderSection_CreatePixelShader_Response& );

private:
	void ParseConifg(const CVFSSection_ReadResponse& );
	void ParseModel(const CVFSSection_ReadResponse& );
	void ComposeWorkerUpdate(CMatrix& workerUpdate );
	void UpdatePositions(std::vector<CMatrix>& positions, const std::vector<CMatrix>& source );
private:
	void assertReadiness(void);

private:// external section ids
	long m_RenderSectionID;
	long m_VfsSectionID;
	long m_MainSectionID;
	long m_PreloaderSectionID;
	 bool m_Slave;

private:// file handles.
	long m_ConfigHandle;
	long m_ModelHandle;
	long m_TextureHandle;


private:
	CDemoSection_WorkerInit_Response::CRenderable m_Renderable;
	long m_TexHandle;
	



private: // urls 
	std::string m_ConfigUrl;
	std::string m_ModelUrl;
	std::string m_TextureUrl;

private: // processing
	//bool m_bHasSatellite;

	std::vector<long> m_SatelliteSectionID;
	
	int m_iSattelites;
	int m_iSatellitesLaunched;
	std::vector<CDemoSection_WorkerInit_Response::CRenderable> m_SatRenderable;
	std::map<long , std::string> m_sSatelliteSectionConfig;
	typedef std::map<long, std::vector<CMatrix> > SatPositions ;

	SatPositions m_SatPositions;

	float m_fRadius;
	float m_fVelocity;
	float m_fSize; 
	float m_fPoint;
	
	float m_fSelfPoint;
	float m_fAngle;
	float m_fSelfSpeed;

	unsigned long m_ElapsedTime;
	
	float m_fX;
	float m_fY;
	float m_fZ;

	bool m_bDisk;

};


#endif























