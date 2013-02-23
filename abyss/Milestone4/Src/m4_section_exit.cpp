#include "../Include/m4_section.h"
#include "../../MT_Core/Include/log.h"

/*
void
CM4Section::Received_WindowClosed()
{
	CLog::Print("============================= WINDOW CLOSED ======================\n");
	//
	// TODO: move this code
	//
	if (m_pO) m_pShadowCreator->DeleteAllShadowsForObject(m_pO);
	//
	CTCommandSender<CCommand_QuitApplication>::SendCommand(SYSTEM_SECTION_ID, CCommand_QuitApplication());
}
*/

void
CM4Section::Event_WindowWasClosedByUser() {
	if(0==m_NShadowsSentToRender) {
		if(m_pO) {
			m_pShadowCreator->DeleteAllShadowsForObject(m_pO);
			delete m_pO;
		}
		// close render/input now
		CloseAllSubsystems();
	} else {
		// enter zombie state
	}
}

void
CM4Section::Event_OkToExitNow() {
	// TODO:  shutdown app completely.
	CTCommandSender<CCommand_QuitApplication>::SendCommand(SYSTEM_SECTION_ID, CCommand_QuitApplication());
}
