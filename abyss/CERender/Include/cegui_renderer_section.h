#ifndef CEGUI_RENDERER_SECTION

#define CEGUI_RENDERER_SECTION

#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../MT_Core/Include/command_sender.h"
#include "../../Render_DX8/Include/render_section_interface.h"
#include "../../Preloader/Include/section_preloader_interface.h"
#include "../../VFS/Include/section_vfs_interface.h"
#include <map>
#include <vector>
#include <list>
#include "cegui_renderer_interface.h"

namespace abyss {
	namespace cegui
	{




		class CEGUI_Renderer_Section : public CSection, 
			public CTCommandSender<CRenderSection_CreatePixelShader_Request>, 
			public CTCommandSender<CRenderSection_ReleasePixelShader_Request>,
			public CTCommandSender<CRenderSection_CreateTexture_Request>, 
			public CTCommandSender<CRenderSection_CreateVertexBuffer_Request>, 
			public CTCommandSender<CRenderSection_UpdateVertexBuffer_Request>, 
			public CTCommandSender<CRenderSection_ReleaseTexture_Request>, 
			public CTCommandSender<CRenderSection_ReleaseVertexBuffer_Request>,
			public CTCommandSender<CRenderSection_RenderVB>,
			public CTCommandSender<CPreloaderSection_DecodeRequest>,
			public CTCommandSender<CPreloaderSection_FileDecodeRequest>,
			public CTCommandSender<CEGUI_Render_InitResponse>,
			public CTCommandSender<CRenderSection_PresentAndClear>,
			public CTCommandSender<CRenderSection_NewFrame>,
			public CTCommandSender<CEGUI_UpdateResponse>,
			public CTCommandSender<CVFSSection_OpenRequest>

		{
			public:
				CEGUI_Renderer_Section( long in_SectionID );
				~CEGUI_Renderer_Section();
				std::string GetName()
				{
					return "CEGUI_Renderer_Section";
				}

				void Reaction( long in_SrcSectionID, const CVFSSection_OpenResponse& in_rOpenResponse);
				void Reaction(long  in_SrcSectionID, const CVFSSection_Error<CVFSSection_OpenRequest>& in_rOpenError);

				// system
				void Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand );
				// render texture creation
				void Reaction(long in_SrcSectionID, const CRenderSection_NewFrame& in );
				void Reaction(long in_SrcSectionID , const CRenderSection_UpdateVertexBuffer_Response & in);
				void Reaction(long in_SrcSectionID , const CRenderSection_CreatePixelShader_Response & in );
				void Reaction(long in_SrcSectionID , const CRenderSection_CreateTexture_Response & in );
				void Reaction(long in_SrcSectionID , const CRenderSection_CreateVertexBuffer_Response & in );
				void Reaction(long in_SrcSectionID , const CPreloaderSection_DecodeResponse & in );
				// interface
				void Reaction( long in_SrcSectionID, const CEGUI_Render_InitRequest &in);
				void Reaction( long in_SrcSectionID, const CEGUI_UpdateRequest& in );

			private:
				void CreateMemoryTexture(const CEGUI_Render_CreateTextureFromMemory &in);
				void DestroyAllTextures();
				void ClearRenderList();
				void DestroyTexture(long textureID);
				void TryRender();
				void UpdateRenderBuffers();

				typedef std::map<long ,CEGUI_Texture> TextureData;
				TextureData m_Textures;
				long m_RenderSectionID;
				long m_PreloaderSectionID;
				long m_VFSSectionID;
				long m_UpdateCount;
				long m_SrcID;
				long m_RequestSource;

		};

	}
}



#endif

