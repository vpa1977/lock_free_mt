#ifndef JPG_SUBSECTION_HPP

#define JPG_SUBSECTION_HPP

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


struct CJpgDecodeRequest : public CCommand 
{
	long m_OrigSectionID;
	std::vector<unsigned char> m_Data;
	long m_Magic;
};

struct CJpgFileDecodeRequest : public CCommand 
{
	long m_OrigSectionID;
	unsigned char  m_Type;
	long m_Handle;
	long m_VFSSection;
	long m_Magic;
};


namespace abyss
{
    namespace preloader {


        class CJpgReadSection : public CSubSection, 
            public CTCommandSender<CPreloaderSection_DecodeResponse>, 
            public CTCommandSender<CVFSSection_ReadRequest>, 
            public CTCommandSender<CPreloaderSection_DecodeError>
        {
        public:
			CJpgReadSection()
			{
				REGISTER_REACTION(CJpgReadSection, CJpgFileDecodeRequest);
				REGISTER_REACTION(CJpgReadSection, CJpgDecodeRequest);
				REGISTER_REACTION(CJpgReadSection, CVFSSection_Error<CVFSSection_ReadRequest>);
				REGISTER_REACTION(CJpgReadSection, CVFSSection_Error<CVFSSection_OpenRequest>);
				REGISTER_REACTION(CJpgReadSection,  CVFSSection_ReadResponse);

			}
			virtual void Reaction(long src,const CJpgFileDecodeRequest& decodeRequest)
            {
                CVFSSection_ReadRequest req(decodeRequest.m_Handle);
                req.m_Magic =CreateReply( decodeRequest.m_Magic , decodeRequest.m_OrigSectionID);
                CTCommandSender<CVFSSection_ReadRequest>::SendCommand( decodeRequest.m_VFSSection,req);
            }
            
			virtual void Reaction(long src,const CJpgDecodeRequest& decodeRequest)
            {
                CPreloaderSection_DecodeResponse response;
                response.m_Magic = decodeRequest.m_Magic;
                DecodeJPG((char*) &(decodeRequest.m_Data[0]) , decodeRequest.m_Data.size() , response);
				CTCommandSender<CPreloaderSection_DecodeResponse>::SendCommand(decodeRequest.m_OrigSectionID , response);
            }

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
            virtual void Reaction(long src,const CVFSSection_ReadResponse& in_rRead)
            {
				CReply reply = GetReply( in_rRead.m_Magic);
				RemoveReply(in_rRead.m_Magic);

                CPreloaderSection_DecodeResponse response;
                response.m_Magic = reply.m_Magic;
                DecodeJPG((char*) &(in_rRead.m_pData[0]) , in_rRead.m_Size , response);
                response.m_Handle = in_rRead.m_Handle;
                CTCommandSender<CPreloaderSection_DecodeResponse>::SendCommand(reply.m_OrigSection, response);
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


			void DecodeJPG( void * inBuf , int size, CPreloaderSection_DecodeResponse& response );



        };
    }
}

#endif
