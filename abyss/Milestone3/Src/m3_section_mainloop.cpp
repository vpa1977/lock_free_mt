#include "../Include/m3_section.h"
#include "../../MT_Core/Include/log.h"
#include "../../MT_Core/Include/rtassert.h"

#include <assert.h>

void
CM3Section::Reaction( long in_SrcSectionID,const CM3Section_StartRender& )
{
	RTASSERT( m_bPreloadSuccessfull );

	m_NFramesToRender = 2;
	m_FirstTimeFrame = GetTickCount();
	if(GetHWND()!=NULL)
	{
		OneFrame();
		OneFrame();
	}
}

void
CM3Section::OneFrame()
{
	assert(GetHWND()!=NULL);
	assert(GetRenderSectionID()!=SECTION_ID_ERROR);
	if(m_NFramesToRender>0)
	{
		assert(m_bPreloadSuccessfull);
		assert(m_pWorld);

		// update
		m_pWorld->UpdateAndDraw( GetTickCount() );

		--m_NFramesToRender;
	}
}

