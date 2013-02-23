#include "section_a.h"
#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/section_registrator.h"

#include <stdio.h>

static CTSectionRegistrator<CSectionA_CLSID,CSectionA> g_CSectionARegistrator( SINGLE_INSTANCE_ONLY, STARTUP_SECTION );

CSectionA::CSectionA( long in_SectionID ) :
	CSection( in_SectionID )
	,m_Counter(0)
{
	printf("CSectionA::CSectionA()\n");
	RegisterReaction( GetID((CCommand_SectionStartup*)NULL), CTConform<CSectionA,CCommand_SectionStartup>::Conform );
	RegisterReaction( GetID((CCommand_MTInfo*)NULL), CTConform<CSectionA,CCommand_MTInfo>::Conform );
	RegisterReaction( GetID((CCommand_LaunchThreadsResult*)NULL), CTConform<CSectionA,CCommand_LaunchThreadsResult>::Conform );
	RegisterReaction( GetID((CSectionA_Bump*)NULL), CTConform<CSectionA,CSectionA_Bump>::Conform );
	RegisterReaction( GetID((CCommand_LaunchSectionResult*)NULL), CTConform<CSectionA,CCommand_LaunchSectionResult>::Conform );
	printf("CSectionA::CSectionA() end\n");
}

CSectionA::~CSectionA()
{
	printf("CSectionA::~CSectionA()\n");
	printf("CSectionA::~CSectionA() end\n");
}      

void
CSectionA::Reaction( const CCommand_SectionStartup& in_rStartupCommand )
{
	printf("CSectionA::Reaction( STARTUP )\n");
	//m_StartTime = timeGetTime();
	CTCommandSender<CCommand_QueryMTInfo>::SendCommand( SECTION_ID_SYSTEM, CCommand_QueryMTInfo(999) );
	printf("CSectionA::Reaction( STARTUP ) end\n");
}

void
CSectionA::Reaction( const CCommand_MTInfo& in_rMTInfoCommand )
{
	printf("CSectionA::Reaction( MT_INFO )\n");
	printf("  Magic       = %ld\n",in_rMTInfoCommand.m_Magic);
	printf("  MaxThreads  = %ld\n",in_rMTInfoCommand.m_MaxThreads);
	printf("  CurrThreads = %ld\n",in_rMTInfoCommand.m_CurrThreads);
	CTCommandSender<CCommand_LaunchThreads>::SendCommand( SECTION_ID_SYSTEM, CCommand_LaunchThreads(2,333) );
	printf("CSectionA::Reaction( MT_INFO ) end\n");
}

void
CSectionA::Reaction( const CCommand_LaunchThreadsResult& in_rResult )
{
	printf("CSectionA::Reaction( LAUNCH_THREADS_RESULT )\n");
	printf("  Magic       = %ld\n",in_rResult.m_Magic);
	printf("  Result = %ld\n",in_rResult.m_Result);
	CTCommandSender<CSectionA_Bump>::SendCommand( GetThisID(), CSectionA_Bump() );
	printf("CSectionA::Reaction( LAUNCH_THREADS_RESULT ) end\n");
}

void
CSectionA::Reaction( const CSectionA_Bump& in_rBump )
{
	printf("CSectionA::Reaction( BUMP )\n");
	if(2 > m_Counter++)
		CTCommandSender<CSectionA_Bump>::SendCommand( GetThisID(), CSectionA_Bump() );
}

void
CSectionA::Reaction( const CCommand_LaunchSectionResult& in_rResult )
{
	printf("CSectionA::Reaction( LAUNCH_RESULT )\n");
	printf("  magic   = %ld\n",in_rResult.m_Magic);
	printf("  errcode = %lu\n",in_rResult.m_ErrorCode);
	printf("  newID   = %ld\n",in_rResult.m_NewSectionID);
	printf("CSectionA::Reaction( LAUNCH_RESULT ) end\n");
}





