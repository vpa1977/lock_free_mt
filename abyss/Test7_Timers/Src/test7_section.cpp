#include "../Include/test7_section.h"
#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/section_registrator.h"
#include "../../MT_Core/Include/log.h"

#include <stdio.h>

static CTSectionRegistrator<CTest7Section_CLSID,CTest7Section> g_CTest7SectionRegistrator( SINGLE_INSTANCE_ONLY, STARTUP_SECTION );

CTest7Section::CTest7Section( long in_SectionID ) :
	CSection(in_SectionID)
{
	RegisterReaction( GetID((CCommand_SectionStartup*)NULL),		CTConform<CTest7Section,CCommand_SectionStartup>::Conform );
	RegisterReaction( GetID((CCommand_MTInfo*)NULL),				CTConform<CTest7Section,CCommand_MTInfo>::Conform );
	RegisterReaction( GetID((CCommand_LaunchThreadsResult*)NULL),	CTConform<CTest7Section,CCommand_LaunchThreadsResult>::Conform );
	RegisterReaction( GetID((CCommand_LaunchSectionResult*)NULL),	CTConform<CTest7Section,CCommand_LaunchSectionResult>::Conform );
	RegisterReaction( GetID((CCommand_SetTimer_Response*)NULL),		CTConform<CTest7Section,CCommand_SetTimer_Response>::Conform );
	RegisterReaction( GetID((CCommand_Timer_Event*)NULL),			CTConform<CTest7Section,CCommand_Timer_Event>::Conform );
	RegisterReaction( GetID((TestCommandA*)NULL),			CTConform<CTest7Section,TestCommandA>::Conform );
}

CTest7Section::~CTest7Section()
{
}
#include <time.h>
float m_start_clock =0;
void
CTest7Section::Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )
{
	CLog::Print("CTest7Section::Reaction( const CCommand_SectionStartup& )\n");
	CLog::Print("  MAXIMUM_WAIT_OBJECTS = %ld\n",MAXIMUM_WAIT_OBJECTS);
	m_start_clock = clock();
	CTCommandSender<TestCommandA>::SendCommand(GetThisID() , TestCommandA());
}

long count = 0;
void
CTest7Section::Reaction( long in_SrcSectionID, const TestCommandA& in_rStartupCommand )
{
	count++;
	if (!(count % 100000))
	{
		float res = (clock() - m_start_clock)/CLOCKS_PER_SEC;
		m_start_clock = clock();
		printf("magic result %f\n" , res);
	}
	CTCommandSender<TestCommandA>::SendCommand(GetThisID() , TestCommandA());
}


void
CTest7Section::Reaction( long in_SrcSectionID, const CCommand_MTInfo& in_rMTInfoCommand )
{
}

void
CTest7Section::Reaction( long in_SrcSectionID, const CCommand_LaunchThreadsResult& in_rResult )
{
}

void
CTest7Section::Reaction( long in_SrcSectionID, const CCommand_LaunchSectionResult& in_rResult )
{
}

void
CTest7Section::Reaction( long in_SrcSectionID, const CCommand_SetTimer_Response& in_rResponse )
{
	CLog::Print("CTest7Section::Reaction( const CCommand_SetTimer_Response& )\n");
}

void
CTest7Section::Reaction( long in_SrcSectionID, const CCommand_Timer_Event& in_rEvent )
{
	CLog::Print("CTest7Section::Reaction( const CCommand_Timer_Event& )\n");
}






























