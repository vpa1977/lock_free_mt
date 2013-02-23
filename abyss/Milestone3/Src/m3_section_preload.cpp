#include "../Include/m3_section.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

void
CM3Section::Reaction( long in_SrcSectionID, const CPreloaderSection_DecodeError& in_rResponse )
{
	assert(false);
}

void
CM3Section::Reaction( long in_SrcSectionID, const CPreloaderSection_DecodeResponse& in_rResponse )
{
	unsigned long MagicHandle = in_rResponse.m_Handle;
	CAwaitedAction Action = ((ProxyAction<unsigned long,CAwaitedAction>&)CLoadProxy::getInstance()).PopProxiedAction(MagicHandle);
	if( AA_DECODE_RAW == Action.second )
	{
		assert( MagicHandle == (unsigned long)Action.first ); // TODO: DANGEROUS ! Oni mogut byt` ravni slu4ajno.
		CLoadUser* pUser = Action.first;
		assert(pUser);
		pUser->DecodeRawResult(true,in_rResponse.m_Width,in_rResponse.m_Height,in_rResponse.m_ARGBData);
	}
	else if( AA_DECODE_FILE == Action.second )
	{
		CLoadUser* pUser = Action.first;
		assert(pUser);
		pUser->DecodeFileResult(true,"TODO",in_rResponse.m_Width,in_rResponse.m_Height,in_rResponse.m_ARGBData);
		CTCommandSender<CVFSSection_CloseRequest>::SendCommand(
			GetVFSSectionID()
			,CVFSSection_CloseRequest(MagicHandle)
		);
		((ProxyAction<unsigned long,CAwaitedAction>&)CLoadProxy::getInstance()).RegisterProxiedAction( MagicHandle, CAwaitedAction(Action.first,AA_CLOSE_AFTER_DECODE) );
	}
	else
	{
		assert(false);
	}
}

