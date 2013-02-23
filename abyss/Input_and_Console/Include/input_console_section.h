#ifndef INPUT_CONSOLE_SECTION_HPP_INCLUDED
#define INPUT_CONSOLE_SECTION_HPP_INCLUDED

/*

USERS <==> Console <==> Input
              ^
			  |
		      v
		 Data Storage

*/

#include "input_console_section_interface.h"
#include "input.h"
#include "console.h"
#include "data_storage.h"

// system
#include "../../App_Win32/Include/window_section_interface.h"
#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../MT_Core/Include/command_sender.h"

class CInputConsoleSection :
	public CSection
	// system
	,public CTCommandSender<CCommand_SetTimer_Request>
	// self
	,public CTCommandSender<CCommand_InputLocked>
	,public CTCommandSender<CCommand_InputUnlocked>
	,public CTCommandSender<CInputConsoleSection_KeyboardUpdate>
	,public CTCommandSender<CInputConsoleSection_MouseUpdate>
{
public:
	CInputConsoleSection( long in_SectionID );
	~CInputConsoleSection();
	std::string GetName()
	{
		return "CInputConsoleSection";
	}

	// system
	void Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand );
	void Reaction( long in_SrcSectionID, const CCommand_SetTimer_Response& );
	void Reaction( long in_SrcSectionID, const CCommand_Timer_Event& );
	void Reaction( long in_SrcSectionID, const CWindowSection_Activated& in_rStartupCommand );
	// self
	void Reaction( long in_SrcSectionID, const CInputConsoleSection_InitInput& initCommand);
//	void Reaction( long in_SrcSectionID, const CInputConsoleSection_InputBump& initCommand);
	void Reaction( long in_SrcSectionID, const CInputConsoleSection_StopInput& initCommand);
	//void Reaction( long in_SrcSectionID, /**/ );

	void IdleProc();
	void KickProc();
	// send commands
	void SendMouseUpdate(const CInputConsoleSection_MouseUpdate& mouse);
	void SendKeyboardUpdate(const CInputConsoleSection_KeyboardUpdate& keyboard);
private:
	CConsole		m_Console;
	CInput			m_Input;
	CDataStorage	m_DataStorage;
	
	BOOL m_bConsoleActive;
};

#endif


















