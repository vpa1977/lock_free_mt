#ifndef SECTION_PRELOADER_HPP

#define SECTION_PRELOADER_HPP

#include "../../MT_Core/Include/log.h"
#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../MT_Core/Include/command_sender.h"
#include "../../VFS/Include/section_vfs_interface.h"

#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>

#include "section_preloader_interface.h"

#include "cbmpfile.h"
#include "tga_subsection.h"
#include "bmp_read_section.h"
#include "jpg_read_section.h"
#include "model_context.h"
#include "matrix_animation_context.h"
#pragma warning(disable:4250)



namespace abyss
{
	namespace preloader {



	



		class CPreloaderSection : public CSection
			,public CTCommandSender<CBmpDecodeRequest>
			,public CTCommandSender<CJpgDecodeRequest>
			,public CTCommandSender<CTgaDecodeRequest>
			,public CTCommandSender<CBmpFileDecodeRequest>
			,public CTCommandSender<CJpgFileDecodeRequest>
			,public CTCommandSender<CTgaFileDecodeRequest>
			,public CTCommandSender<CModelDecodeRequest>
			,public CTCommandSender<CMatrixDecodeRequest>

			
		{
			public:
				CPreloaderSection(long sectionID );
				virtual ~CPreloaderSection();
				
				void Reaction(long in_SrcSectionID , const CVFSSection_ReadResponse& in_rRead);
				void Reaction(long in_SrcSectionID , const CPreloaderSection_DecodeRequest& decodeRequest);
				void Reaction(long in_SrcSectionID , const CPreloaderSection_FileDecodeRequest& decodeRequest);
				void Reaction(long in_SrcSectionID , const CPreloaderSection_ModelLoadRequest& decodeRequest);
				void Reaction(long in_SrcSectionID , const CPreloaderSection_MatrixLoadRequest& in_rRead);
				
		
				
				void Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )
				{
					
				};

				virtual std::string GetName() {return "CPreloaderSection"; }

		};



	}
}


#endif