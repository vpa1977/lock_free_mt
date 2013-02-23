#include "../Include/m3_section.h"
#include "../../MT_Core/Include/log.h"

void
CM3Section::Received_WindowClosed()
{
	CLog::Print("============================= WINDOW CLOSED ======================\n");
	CTCommandSender<CCommand_QuitSystem>::SendCommand( SYSTEM_SECTION_ID, CCommand_QuitSystem() );
}
