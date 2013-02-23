#include "section_b.h"
#include "../MT_Core/Include/idgen.h"
#include "../MT_Core/Include/section_registrator.h"

#include <stdio.h>

static CTSectionRegistrator<CSectionB_CLSID,CSectionB> g_CSectionBRegistrator( SINGLE_INSTANCE_ONLY );

CSectionB::CSectionB( long in_SectionID ) :
	CSection( in_SectionID )
{
	printf("CSectionB::CSectionB()\n");
	RegisterReaction( GetID((CCommand_SectionStartup*)NULL), CTConform<CSectionB,CCommand_SectionStartup>::Conform );
	RegisterReaction( GetID((CSectionB_Bump*)NULL), CTConform<CSectionB,CSectionB_Bump>::Conform );
	printf("CSectionB::CSectionB() end\n");
}

CSectionB::~CSectionB()
{
	printf("CSectionB::~CSectionB()\n");
	printf("CSectionB::~CSectionB() end\n");
}

void
CSectionB::Reaction( const CCommand_SectionStartup& in_rStartupCommand )
{
	printf("CSectionB::Reaction( STARTUP )\n");
	SendCommand( GetThisID(), CSectionB_Bump() );
	printf("CSectionB::Reaction( STARTUP ) end\n");
}

void
CSectionB::Reaction( const CSectionB_Bump& in_rBump )
{
	printf("CSectionB::Reaction( BUMP )\n");
	printf("CSectionB::Reaction( BUMP ) end\n");
}




