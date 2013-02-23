#ifndef STARS_SECTION_HPP_INCLUDED
#define STARS_SECTION_HPP_INCLUDED

#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/command_sender.h"

//#include "../../MT_Core/Include/system_commands.h"
#include "demo_section_interface.h"
#include "../../Preloader/Include/section_preloader_interface.h"
#include "../../VFS/Include/section_vfs_interface.h"
#include "../../Render_DX8/Include/render_section_interface.h"
#include "stars_section_interface.h"

#pragma warning(disable:4250)

class CStarsSection :
	public CSection
	// system
	// demo section
	,public CTCommandSender<CDemoSection_WorkerInit_Response>
	,public CTCommandSender<CDemoSection_WorkerUpdate>
	,public CTCommandSender<CDemoSection_WorkerShutdown_Response>
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
	CStarsSection( long in_SectionID );
	~CStarsSection();
	std::string GetName()
	{
		return "CStarsSection";
	}

	// system
	void Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand );
	// demo section
	void Reaction( long in_SrcSectionID, const CDemoSection_WorkerInit_Request& );
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
	std::string m_CatalogUrl;
	std::string m_TextureUrl;

	// file handles.
	long m_ConfigHandle;
	long m_CatalogHandle;
	long m_TextureHandle;

	// VB and PS handles
	unsigned long m_VBHandle;
	unsigned long m_PSHandle;
	unsigned long m_THandle;

	unsigned long m_TriangleCount;

	void ParseConfig( const std::vector<unsigned char>& in_rData );
	void ParseStarsCatalog( const std::vector<unsigned char>& in_rData, std::vector<unsigned char>& out_rVBData );
	void CheckForInitializationCompletion();
};


#endif























