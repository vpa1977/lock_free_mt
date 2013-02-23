#include "../Include/m3_section.h"
#include "../../MT_Core/Include/log.h"
#include "../../MT_Core/Include/rtassert.h"
#include <assert.h>

void
CM3Section::StartTheGame()
{
	RTASSERT( SECTION_ID_ERROR != GetPreloaderSectionID() );
	RTASSERT( SECTION_ID_ERROR != GetVFSSectionID() );
	RTASSERT( SECTION_ID_ERROR != GetWindowSectionID() );
	RTASSERT( SECTION_ID_ERROR != GetRenderSectionID() );

	// TODO: perform config preloading/parsing here
	CTCommandSender<CVFSSection_MountRequest>::SendCommand(
		GetVFSSectionID(),
		CVFSSection_MountRequest(".")
	);
}

void
CM3Section::StartPreload()
{
	m_pWorld = new CWorld( this );
	m_pWorld->StartPreload();
	CTCommandSender<CM3Section_WaitingForPreloadToComplete>::SendCommand(
		GetThisID(),
		CM3Section_WaitingForPreloadToComplete()
	);
}

void
CM3Section::Reaction( long in_SrcSectionID , const CM3Section_WaitingForPreloadToComplete& in_rCmd )
{
	if(m_bPreloadSuccessfull)
	{
		CTCommandSender<CM3Section_StartRender>::SendCommand(
			GetThisID(),
			CM3Section_StartRender()
		);
	}
	else
	{
		CTCommandSender<CM3Section_WaitingForPreloadToComplete>::SendCommand(
			GetThisID(),
			CM3Section_WaitingForPreloadToComplete()
		);
	}
}

void
CM3Section::SomeObjectPreloadComplete( bool in_bSuccess, void* in_Handle )
{
	RTASSERT( in_Handle == (CGameObject*)m_pWorld );
	RTASSERT( in_bSuccess );
	m_bPreloadSuccessfull = in_bSuccess;
}


