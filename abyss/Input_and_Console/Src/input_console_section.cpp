#include "../Include/input_console_section.h"
#include "../../MT_Core/Include/section_registrator.h"

static CTSectionRegistrator<CInputConsoleSection_CLSID,CInputConsoleSection> g_CInputConsoleRegistrator(SINGLE_INSTANCE_ONLY);

void Touch_InputConsoleSection()
{
}

void CInputConsoleSectionKickProc(thread* t, section* s) 
{
	( (CInputConsoleSection*) s)->KickProc();
}
void CInputConsoleSectionIdleProc(thread* t, section* s) 
{
	( (CInputConsoleSection*) s)->IdleProc();
}

CInputConsoleSection::CInputConsoleSection( long in_SectionID ) :
	CSection( in_SectionID ),
	m_bConsoleActive(false)
{
	RegisterReaction( GetID((CCommand_SectionStartup*)NULL), CTConform<CInputConsoleSection,CCommand_SectionStartup>::Conform );
	RegisterReaction( GetID((CCommand_SetTimer_Response*)NULL), CTConform<CInputConsoleSection,CCommand_SetTimer_Response>::Conform );
	RegisterReaction( GetID((CCommand_Timer_Event*)NULL), CTConform<CInputConsoleSection,CCommand_Timer_Event>::Conform );
	RegisterReaction( GetID((CInputConsoleSection_InitInput*)NULL), CTConform<CInputConsoleSection,CInputConsoleSection_InitInput>::Conform );
//	RegisterReaction( GetID((CInputConsoleSection_StopInput*)NULL), CTConform<CInputConsoleSection,CInputConsoleSection_StopInput>::Conform );
//	RegisterReaction( GetID((CInputConsoleSection_InputBump*)NULL), CTConform<CInputConsoleSection,CInputConsoleSection_InputBump>::Conform );
	
	RegisterReaction( GetID((CWindowSection_Activated*)NULL),  CTConform<CInputConsoleSection,CWindowSection_Activated>::Conform );
}

CInputConsoleSection::~CInputConsoleSection()
{

}

void
CInputConsoleSection::Reaction( long in_SrcSectionID, const CWindowSection_Activated& in_rStartupCommand )
{
	CLog::Println("Input Recieved Window Activated");
	if (in_rStartupCommand.m_Active) 
	{
		CLog::Println("Aquire device");
		m_Input.Acquire();
	}
	else
	{
		CLog::Println("Unaquire device");
		m_Input.Unaquire();
	}
}


void
CInputConsoleSection::Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )
{
	InstallThreadProcs( &CInputConsoleSectionIdleProc, &CInputConsoleSectionKickProc);
} 

void
CInputConsoleSection::Reaction( long in_SrcSectionID, const CCommand_SetTimer_Response& in_rResponse )
{
}

void
CInputConsoleSection::Reaction( long in_SrcSectionID, const CCommand_Timer_Event& in_rEvent )
{
}

void
CInputConsoleSection::Reaction( long in_SrcSectionID, const CInputConsoleSection_InitInput& initCommand)
{	
	if (FAILED (m_Input.InitDirectInput(initCommand)))
	{
		assert(0);
		return;
	};
	m_Input.SetSection(this);
	/*CTCommandSender<CCommand_InputLocked>::SendCommand(
				SECTION_ID_SYSTEM,
				CCommand_InputLocked( m_Input.GetInputLock() )
			);*/
	

}


void CInputConsoleSection::KickProc()
{
	m_Input.Kick();
}
void 
CInputConsoleSection::IdleProc()
{
	//m_Input.Acquire();
	m_Input.ReadInput(get_thread()->m_handle);
}

void 
CInputConsoleSection::Reaction( long in_SrcSectionID, const CInputConsoleSection_StopInput& initCommand)
{
	CTCommandSender<CCommand_InputUnlocked>::SendCommand(
				SECTION_ID_SYSTEM,
				CCommand_InputUnlocked( m_Input.GetInputLock() )
			);

}


void CInputConsoleSection::SendMouseUpdate(const CInputConsoleSection_MouseUpdate& mouse ) 
{
	CTCommandSender<CInputConsoleSection_MouseUpdate>::SendCommand(SECTION_ID_BROADCAST , 
		mouse);
}

void CInputConsoleSection::SendKeyboardUpdate(const CInputConsoleSection_KeyboardUpdate&  keyboard )
{
	if (keyboard.m_bPressed) 
	{
		if (keyboard.m_bScanCode == DIK_GRAVE  ) 
		{
			m_bConsoleActive = !m_bConsoleActive;
		}
	}
	if (!m_bConsoleActive ) 
	{
		CTCommandSender<CInputConsoleSection_KeyboardUpdate>::SendCommand(SECTION_ID_BROADCAST , 
			keyboard);
	}

}