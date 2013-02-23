#include "../Include/m3_section.h"
#include "../../MT_Core/Include/log.h"

void
CM3Section::Reaction( long in_SrcSectionID, const CRenderSection_NewFrame& in_rCmd )
{
	++m_NFramesToRender;
	++m_NFrames;
	if(GetHWND()!=NULL)
	{
		unsigned long ThisFrameTime = GetTickCount();
		if( (ThisFrameTime-m_LastTimeFrame)>=1000 )
		{
			float FPS = 1000.0f*(float(m_NFrames))/(float(ThisFrameTime-m_LastTimeFrame));
			char Buffer[256];
			sprintf(Buffer,"Milestone 3: The 15. (FPS=%1.0f)",FPS);
			/*CTCommandSender<CWindowSection_WindowName>::SendCommand(
				GetWindowSectionID(),
				CWindowSection_WindowName(std::string(Buffer))
			);*/
			//
			m_LastTimeFrame = ThisFrameTime;
			m_NFrames=0;
		}
		OneFrame();
	}
}

















