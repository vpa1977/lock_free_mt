#include "../Include/m3_section.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>


void
CM3Section::ReadFile( const char* in_pszFileName, CLoadUser* in_pUser )
{
	// TODO: test it
	assert(in_pUser);
	assert(in_pszFileName);
	CTCommandSender<CVFSSection_OpenRequest>::SendCommand(
		GetVFSSectionID()
		,CVFSSection_OpenRequest(in_pszFileName)
	);
	((ProxyAction<std::string,CAwaitedAction>&)CLoadProxy::getInstance()).RegisterProxiedAction( in_pszFileName, CAwaitedAction(in_pUser,AA_OPEN_FOR_READ) );
}

void
CM3Section::DecodeImageFromFile( const char* in_pszFileName, CLoadUser* in_pUser )
{
	// TODO: test it
	assert(in_pUser);
	assert(in_pszFileName);
	CTCommandSender<CVFSSection_OpenRequest>::SendCommand(
		GetVFSSectionID()
		,CVFSSection_OpenRequest(in_pszFileName)
	);
	((ProxyAction<std::string,CAwaitedAction>&)CLoadProxy::getInstance()).RegisterProxiedAction( in_pszFileName, CAwaitedAction(in_pUser,AA_OPEN_FOR_DECODE) );
}

void
CM3Section::DecodeRawImage( IMAGE_TYPE in_Type, const std::vector<unsigned char> in_rData, CLoadUser* in_pUser )
{
	// TODO
	assert(in_pUser);
	CPreloaderSection_DecodeRequest req;
	req.m_Handle = (long)in_pUser;
	req.m_Type = (in_Type==CLoadProxy::IT_BMP) ? TYPE_BMP : ((in_Type==CLoadProxy::IT_TGA)?TYPE_TGA:TYPE_JPG);
	req.m_Data = in_rData;
	CTCommandSender<CPreloaderSection_DecodeRequest>::SendCommand(
		GetPreloaderSectionID()
		,req
	);
	((ProxyAction<unsigned long,CAwaitedAction>&)CLoadProxy::getInstance()).RegisterProxiedAction( (unsigned long)in_pUser, CAwaitedAction(in_pUser,AA_DECODE_RAW) );
}





