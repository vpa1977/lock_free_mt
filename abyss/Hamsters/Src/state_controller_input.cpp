#include "../include/hamsters.h"
#include "../include/hamster_gui.h"
#include "../include/hamster_section.h"
#include "../include/state_controller_section.h"


bool m_bDown = false;
void 
CHamsterSection::Reaction(long in_SrcSectionID, const CInputConsoleSection_MouseUpdate& in_rCmd ) 
{
	if (m_pProxy) 
	{
		CEGUI::System::getSingleton().injectMousePosition(in_rCmd.m_dwX, in_rCmd.m_dwY);
		if (in_rCmd.m_bLeftPressed && !m_bDown) 
		{
			CEGUI::System::getSingleton().injectMouseButtonDown(LeftButton);
			m_bDown = true;
		}
		if (!in_rCmd.m_bLeftPressed && m_bDown) 
		{
			CEGUI::System::getSingleton().injectMouseButtonUp(LeftButton);
			m_bDown = false;
		}
/// handle method returns
		if ( ((CHamsterGUI*)m_pProxy)->IsNewGame() ) 
		{
			CTCommandSender<CHamster_NewGame>::SendCommand(SECTION_ID_BROADCAST, CHamster_NewGame());
		}
	}
	


}
