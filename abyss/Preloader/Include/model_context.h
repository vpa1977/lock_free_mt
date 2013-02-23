#ifndef MODEL_CONTEXT_HPP

#define MODEL_CONTEXT_HPP
#include "../../MT_Core/Include/core.h"
#include "entity_serializer.h"
#include <map>
#include <boost/shared_ptr.hpp>
#include "section_preloader_interface.h"

struct CModelDecodeRequest : public CPreloaderSection_ModelLoadRequest
{
	long m_OrigSectionID;
};

namespace abyss
{
	namespace preloader
	{
		enum ELoadState
		{
			MATERIAL,
			NEXT_TEXTURE,
			GEOMETRY,
			ANIMATION
		};
		class CModelContext : public CSubSection, 
						public CTCommandSender<CPreloaderSection_DecodeResponse>, 
						public CTCommandSender<CVFSSection_ReadRequest>, 
						public CTCommandSender<CPreloaderSection_DecodeError>,
						public CTCommandSender<CVFSSection_OpenRequest>,
						public CTCommandSender<CVFSSection_CloseRequest>, 
						public CTCommandSender<CPreloaderSection_ModelLoadResponse>
		{
			public:
				CModelContext()  
				{
					REGISTER_REACTION(CModelContext, CVFSSection_ReadResponse);
					REGISTER_REACTION(CModelContext, CVFSSection_OpenResponse);
					REGISTER_REACTION(CModelContext, CModelDecodeRequest);
					REGISTER_REACTION(CModelContext, CVFSSection_Error<CVFSSection_OpenRequest>);
				}

				virtual void Reaction(long src, const CVFSSection_ReadResponse& in_rRead);
				virtual void Reaction(long src, const CVFSSection_OpenResponse& in_rRead);
				virtual void Reaction(long src, const CModelDecodeRequest& decodeRequest);
				virtual void Reaction(long src, const CVFSSection_Error<CVFSSection_OpenRequest>& in_rRead);
		private:
			bool LoadMaterial(const CVFSSection_ReadResponse& in_rRead);
			bool LoadTexture( const CVFSSection_ReadResponse& in_rRead);
			bool LoadGeometry(long magic, const std::vector<unsigned char>& data, int mark);
			static std::map< std::string , boost::shared_ptr<CRenderable_AbstractTextureMap> > LOADED_IMAGES;
			void assert_input(const std::vector<unsigned char>& data, long pos, long size);
			private:
				std::vector<unsigned char> m_Read;
				long m_GlobalMark;
				std::vector<CSavedMaterials> m_MaterialList;
				std::vector<CRenderable_AbstractMaterial> m_LoadedMaterials;
				std::string m_FileName;
				long m_TextureMaterialCount;
				long m_Stage;
				long m_VFS;

		};
	}
}

#endif