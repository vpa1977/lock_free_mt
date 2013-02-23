#ifndef BMP_SUBSECTION_HPP

#define BMP_SUBSECTION_HPP

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


struct CBmpDecodeRequest : public CCommand 
{
	long m_OrigSectionID;
	std::vector<unsigned char> m_Data;
	long m_Magic;
};

struct CBmpFileDecodeRequest : public CCommand 
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


        class CBmpReadSection : public CSubSection, 
            public CTCommandSender<CPreloaderSection_DecodeResponse>, 
            public CTCommandSender<CVFSSection_ReadRequest>, 
            public CTCommandSender<CPreloaderSection_DecodeError>
        {
        public:
			CBmpReadSection()
			{
				REGISTER_REACTION(CBmpReadSection, CBmpFileDecodeRequest);
				REGISTER_REACTION(CBmpReadSection, CBmpDecodeRequest);
				REGISTER_REACTION(CBmpReadSection, CVFSSection_Error<CVFSSection_ReadRequest>);
				REGISTER_REACTION(CBmpReadSection, CVFSSection_Error<CVFSSection_OpenRequest>);
				REGISTER_REACTION(CBmpReadSection,  CVFSSection_ReadResponse);

			}
			virtual void Reaction(long src,const CBmpFileDecodeRequest& decodeRequest)
            {
                CVFSSection_ReadRequest req(decodeRequest.m_Handle);
                req.m_Magic =CreateReply( decodeRequest.m_Magic , decodeRequest.m_OrigSectionID);
                CTCommandSender<CVFSSection_ReadRequest>::SendCommand( decodeRequest.m_VFSSection,req);
            }
            
			virtual void Reaction(long src,const CBmpDecodeRequest& decodeRequest)
            {
                CPreloaderSection_DecodeResponse response;
                response.m_Magic = decodeRequest.m_Magic;
                DecodeBMP((char*) &(decodeRequest.m_Data[0]) , decodeRequest.m_Data.size() , response);
				CTCommandSender<CPreloaderSection_DecodeResponse>::SendCommand(decodeRequest.m_OrigSectionID , response);
            }

            virtual void Reaction(long src,const CVFSSection_ReadResponse& in_rRead)
            {
				CReply reply = GetReply( in_rRead.m_Magic);
				RemoveReply(in_rRead.m_Magic);

                CPreloaderSection_DecodeResponse response;
                response.m_Magic = reply.m_Magic;
                DecodeBMP((char*) &(in_rRead.m_pData[0]) , in_rRead.m_Size , response);
                response.m_Handle = in_rRead.m_Handle;
                CTCommandSender<CPreloaderSection_DecodeResponse>::SendCommand(reply.m_OrigSection, response);
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


           	void DecodeBMP(void* inBuf, int size, CPreloaderSection_DecodeResponse& resp)
				{								  
 					char* in = (char*)inBuf;
					CBmpFile bmpFile;
					int res = bmpFile.Read((unsigned char*)inBuf , size ) ;
					if (res > 0  )
					{	
						int count = 0;
						resp.m_ARGBData.resize(bmpFile.m_ImgData.size() +  bmpFile.m_ImgData.size()/3);
						resp.m_Height = bmpFile.m_Height;
						resp.m_Width = bmpFile.m_Width;
						for (int i = 0 ; i < bmpFile.m_Width * bmpFile.m_Height ; i ++ ) 
						{
							resp.m_ARGBData[count] = bmpFile.m_ImgData[i*3    ];count++;
							resp.m_ARGBData[count] = bmpFile.m_ImgData[i*3 + 1];count++;
							resp.m_ARGBData[count] = bmpFile.m_ImgData[i*3 + 2];count++;
							resp.m_ARGBData[count] = 1;count++;

						}
					}
					else
					{
						resp.m_ARGBData.resize(0);
					}

				}



        };
    }
}

#endif
