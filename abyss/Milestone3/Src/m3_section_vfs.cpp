#include "../Include/m3_section.h"
#include "../../MT_Core/Include/log.h"
#include "../../MT_Core/Include/rtassert.h"

void
CM3Section::Reaction( long in_SrcSectionID, const CVFSSection_MountResponse& in_rMountResponse)
{
	StartPreload();
}

void
CM3Section::Reaction( long in_SrcSectionID, const CVFSSection_UMountResponse& in_rUMountResponse)
{
}

void
CM3Section::Reaction( long in_SrcSectionID, const CVFSSection_ReadResponse& in_rReadResponse)
{
	assert(in_rReadResponse.m_Offset==0);
	//assert(in_rReadResponse.m_Size==-1);
	unsigned long Handle = in_rReadResponse.m_Handle;
	CAwaitedAction Action = ((ProxyAction<unsigned long,CAwaitedAction>&)CLoadProxy::getInstance()).PopProxiedAction(Handle);
	assert(Action.second==AA_READ);
	CLoadUser* pUser = Action.first;
	assert(pUser);
	pUser->ReadFileResult(true,"TODO",in_rReadResponse.m_pData);
	CTCommandSender<CVFSSection_CloseRequest>::SendCommand(
		GetVFSSectionID()
		,CVFSSection_CloseRequest(Handle)
	);
	((ProxyAction<unsigned long,CAwaitedAction>&)CLoadProxy::getInstance()).RegisterProxiedAction( Handle, CAwaitedAction(Action.first,AA_CLOSE_AFTER_READ) );
}

unsigned char
GetImageTypeFromFilename(const std::string& in_rFileName)
{
	const char* pFileName = in_rFileName.c_str();
	const char* pDot = strrchr(pFileName,'.');
	assert(pDot);
	if( 0==stricmp(pDot,".jpg") || 0==stricmp(pDot,".jpe") || 0==stricmp(pDot,".jpeg") )
	{
		return TYPE_JPG;
	}
	else if( 0==stricmp(pDot,".tga") )
	{
		return TYPE_TGA;
	}
	else if( 0==stricmp(pDot,".bmp") )
	{
		return TYPE_BMP;
	}
	else
	{
		assert(false);
		return 0xff;
	}
}

void
CM3Section::Reaction( long in_SrcSectionID, const CVFSSection_OpenResponse& in_rOpenResponse)
{
	const std::string& rURL = in_rOpenResponse.m_Url;
	CAwaitedAction Action = ((ProxyAction<std::string,CAwaitedAction>&)CLoadProxy::getInstance()).PopProxiedAction(rURL);
	if( AA_OPEN_FOR_READ == Action.second )
	{
		CTCommandSender<CVFSSection_ReadRequest>::SendCommand(
			GetVFSSectionID()
			,CVFSSection_ReadRequest(in_rOpenResponse.m_Handle,0)
		);
		((ProxyAction<unsigned long,CAwaitedAction>&)CLoadProxy::getInstance()).RegisterProxiedAction( in_rOpenResponse.m_Handle, CAwaitedAction(Action.first,AA_READ) );
	}
	else if( AA_OPEN_FOR_DECODE == Action.second )
	{
		CPreloaderSection_FileDecodeRequest req;
		req.m_Handle = in_rOpenResponse.m_Handle;
		req.m_Type = GetImageTypeFromFilename(rURL);
		req.m_VFSSection = GetVFSSectionID();
		CTCommandSender<CPreloaderSection_FileDecodeRequest>::SendCommand(
			GetPreloaderSectionID()
			,req
		);
		((ProxyAction<unsigned long,CAwaitedAction>&)CLoadProxy::getInstance()).RegisterProxiedAction( in_rOpenResponse.m_Handle, CAwaitedAction(Action.first,AA_DECODE_FILE) );
	}
	else
	{
		assert(false);
	}
}

void
CM3Section::Reaction( long in_SrcSectionID, const CVFSSection_CloseResponse& in_rCloseResponse)
{
	unsigned long Handle = in_rCloseResponse.m_Handle;
	CAwaitedAction Action = ((ProxyAction<unsigned long,CAwaitedAction>&)CLoadProxy::getInstance()).PopProxiedAction(Handle);
	assert( Action.second==AA_CLOSE_AFTER_READ || Action.second==AA_CLOSE_AFTER_DECODE );
}

// ============================================================================

void
CM3Section::Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_MountResponse>& in_rError )
{
	RTASSERT(false);
}

void
CM3Section::Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_UMountResponse>& in_rError )
{
	RTASSERT(false);
}

void
CM3Section::Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_ReadResponse>& in_rError )
{
	RTASSERT(false);
}

void
CM3Section::Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_OpenResponse>& in_rError )
{
	RTASSERT(false);
}

void
CM3Section::Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_CloseResponse>& in_rError )
{
	RTASSERT(false);
}
