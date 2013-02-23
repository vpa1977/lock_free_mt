#include "../Include/matrix_animation_context.h"

using namespace abyss::preloader;
#include <list>

void CMatrixAnimationContext::Reaction(long src,const CVFSSection_ReadResponse& in_rRead)
{
	CReply reply = GetReply(in_rRead.m_Magic);
	RemoveReply(in_rRead.m_Magic);

	CVFSSection_CloseRequest close;
	close.m_Handle = in_rRead.m_Handle;
	CTCommandSender<CVFSSection_CloseRequest>::SendCommand(m_VFS, close);

	CObjectAnimationController* pController;
	std::list<CObjectAnimation> animations;
	
	long pos = 0;
	long rate = 30;
	long mode = 0;
	memcpy(&rate, &in_rRead.m_pData[pos], sizeof(long));
	pos += sizeof(long);
	memcpy(&mode, &in_rRead.m_pData[pos], sizeof(long));
	pos+= sizeof(long);
	while (pos < in_rRead.m_pData.size())
	{
		long nameLen;
		long floatLen;
		long boneId;
		std::vector<char> name;
		if (mode == 0 ) {
			memcpy(&boneId, &in_rRead.m_pData[pos], sizeof(long));
			pos +=sizeof(long);
		}
		/*if (mode == 1 )*/ {
			memcpy(&nameLen, &in_rRead.m_pData[pos], sizeof(long));
			name.resize(nameLen);
			pos +=sizeof(long);
			memcpy(&name[0] ,&in_rRead.m_pData[pos], nameLen);
			pos += nameLen;
		}
		//else
		
		
		memcpy(&floatLen,& in_rRead.m_pData[pos], sizeof(long));
		pos += sizeof(long);

		std::list<CKeyFrame> out;
		
		for (int i = 0 ; i< floatLen ; i ++ ) 
		{
			CKeyFrame m;
			memcpy(&m , &in_rRead.m_pData[pos], sizeof(CKeyFrame));
			pos += sizeof(CKeyFrame);
			out.push_back(m);
		}
		name.push_back(0);
		CObjectAnimation current;
		current.m_BoneID = boneId;
		current.m_ObjectName = (char*) &name[0];
		current.m_AnimationData=out;
		animations.push_back(current);
	}
	pController = new CObjectAnimationController(rate,!mode,animations);
	CPreloaderSection_MatrixLoadResponse resp;
	resp.m_pController = pController;
	resp.m_Magic = reply.m_Magic;

	CTCommandSender<CPreloaderSection_MatrixLoadResponse>::SendCommand(reply.m_OrigSection, resp);
	//return true;
}

void CMatrixAnimationContext::Reaction(long src, const CVFSSection_OpenResponse& in_rRead)
{
	
	CVFSSection_ReadRequest req(in_rRead.m_Handle);
	req.m_Magic = in_rRead.m_Magic;
	CTCommandSender<CVFSSection_ReadRequest>::SendCommand(m_VFS , req);

}
void CMatrixAnimationContext::Reaction(long src, const CMatrixDecodeRequest& decodeRequest)
{

	CVFSSection_OpenRequest req;
	req.m_Url = decodeRequest.m_FileName;
	req.m_Magic =CreateReply( decodeRequest.m_Magic , decodeRequest.m_OrigSectionID);
	m_VFS = decodeRequest.m_VFSSection;
	CTCommandSender<CVFSSection_OpenRequest>::SendCommand(decodeRequest.m_VFSSection , req);
}