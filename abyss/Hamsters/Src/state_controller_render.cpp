#include "../include/hamsters.h"

#include "../include/state_controller_section.h"
#include "../../MT_Core/include/impl/mutex.h"

void
CSectionController::OneFrame()
{
	
	assert(GetHWND()!=NULL);
	assert(GetRenderSectionID()!=SECTION_ID_ERROR);
	if(m_NFramesToRender>0)
	{
		if (m_bInTransition) 
		{
			PrepareTransition(GetCurrentTransition());
		}
		else
		{
			CTCommandSender<CState_UpdateRequest>::SendCommand( m_CurrentState, CState_UpdateRequest());
		}
		
		--m_NFramesToRender;
	}
}

void CSectionController::TransitionUpdate() 
{
	CEGUI_UpdateRequest req;
	m_pProxy->ComposeRequest(req);
	CTCommandSender<CEGUI_UpdateRequest>::SendCommand(m_ProxySectionID, req);
}

void CSectionController::Reaction(long inSectionID , const CState_UpdateResponse& upd)
{
	CEGUI_UpdateRequest req;
	m_pProxy->ComposeRequest(req);
	CTCommandSender<CEGUI_UpdateRequest>::SendCommand(m_ProxySectionID, req);
}


void CSectionController::Reaction(long inSectionID , const CEGUI_UpdateResponse& resp)
{
	CTCommandSender<CRenderSection_PresentAndClear>::SendCommand(GetRenderSectionID() , CRenderSection_PresentAndClear());
}




void
CSectionController::Reaction( long in_SrcSectionID, const CRenderSection_NewFrame& in_rCmd )
{


	++m_NFramesToRender;
	++m_NFrames;
	if(GetHWND()!=NULL)
	{
		unsigned long ThisFrameTime = GetTickCount();
		if( (ThisFrameTime-m_LastTimeFrame)>=3000 )
		{
			float FPS = 100.0f*(float(m_NFrames))/(float(ThisFrameTime-m_LastTimeFrame));
			char Buffer[256];
			sprintf(Buffer,"Hamsters. (FPS=%1.0f)",FPS);
			CTCommandSender<CWindowSection_WindowName>::SendCommand(
				GetWindowSectionID(),
				CWindowSection_WindowName(std::string(Buffer))
			);
			//
			m_LastTimeFrame = ThisFrameTime;
			m_NFrames=0;
		}
		OneFrame();
	}
	
}



