#ifndef MATRIX_ANIMATION_CONTEXT_HPP

#define MATRIX_ANIMATION_CONTEXT_HPP

#include "../../VFS/Include/section_vfs_interface.h"
#include "section_preloader_interface.h"
#include "entity_serializer.h"
#include "../../Objects/Include/object_animation.h"

struct CMatrixDecodeRequest : public CPreloaderSection_MatrixLoadRequest
{
	long m_OrigSectionID;
};

namespace abyss
{
	namespace preloader
	{
		class CMatrixAnimationContext : public CSubSection, 
						public CTCommandSender<CPreloaderSection_MatrixLoadResponse>,
						public CTCommandSender<CPreloaderSection_DecodeResponse>, 
						public CTCommandSender<CVFSSection_ReadRequest>, 
						public CTCommandSender<CPreloaderSection_DecodeError>,
						public CTCommandSender<CVFSSection_OpenRequest>,
						public CTCommandSender<CVFSSection_CloseRequest>
		{
			public:
				CMatrixAnimationContext()
				{
					REGISTER_REACTION(CMatrixAnimationContext, CVFSSection_ReadResponse);
					REGISTER_REACTION(CMatrixAnimationContext, CVFSSection_OpenResponse);
					REGISTER_REACTION(CMatrixAnimationContext, CMatrixDecodeRequest);
					REGISTER_REACTION(CMatrixAnimationContext, CVFSSection_Error<CVFSSection_ReadRequest>);
					REGISTER_REACTION(CMatrixAnimationContext, CVFSSection_Error<CVFSSection_OpenRequest>);

				}
				virtual void Reaction(long src, const CVFSSection_ReadResponse& in_rRead);
				virtual void Reaction(long src, const CVFSSection_OpenResponse& in_rRead);
				virtual void Reaction(long src, const CMatrixDecodeRequest& decodeRequest);

	         virtual void Reaction(long src, const CVFSSection_Error<CVFSSection_ReadRequest>& in_rRead)
            {

				CReply reply = GetReply( in_rRead.m_Magic);
				RemoveReply(in_rRead.m_Magic);
                CPreloaderSection_DecodeError error;
                error.m_Message = in_rRead.m_ErrorMessage;
                error.m_Magic = reply.m_Magic;
                error.m_Handle = in_rRead.m_Req.m_Handle;
                error.m_ErrorCode = in_rRead.m_ErrorCode;
                CTCommandSender<CPreloaderSection_DecodeError>::SendCommand(reply.m_OrigSection, error);

            }

            virtual void Reaction(long src, const CVFSSection_Error<CVFSSection_OpenRequest>& in_rRead)
            {
				CReply reply = GetReply( in_rRead.m_Magic);
				RemoveReply(in_rRead.m_Magic);

                CPreloaderSection_DecodeError error;
                error.m_Message = in_rRead.m_ErrorMessage;
                error.m_Magic =reply.m_Magic;
                error.m_Handle = -1;
                error.m_ErrorCode = in_rRead.m_ErrorCode;
                const char *t = in_rRead.m_Req.m_Url.c_str();
                CLog::Println("Failed to read: %s ", in_rRead.m_Req.m_Url.data());
                CTCommandSender<CPreloaderSection_DecodeError>::SendCommand(reply.m_OrigSection, error);
            }


			private:
				std::string m_FileName;
				long m_VFS;

		};
	}
}



#endif 