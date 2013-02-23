#include "../Include/demo_section.h"
#include "../../MT_Core/Include/log.h"

void
CDemoSection::Received_SectionLaunchResult(const CCommand_LaunchSectionResult& in_rResult)
{
CLog::Print("CDemoSection::Received_SectionLaunchResult()\n");
	if(EIS_AWAITING_WORKERS_LAUNCH==m_InitStage)
	{
CLog::Print("  awaiting worker\n");
		assert( in_rResult.m_ErrorCode == SLR_OK );
		assert( in_rResult.m_NewSectionID != SECTION_ID_ERROR );

		// find what worker is it
		long FoundAt = -1;
		for(size_t i=0; i<m_ObjectSectionsInfo.size(); ++i)
		{
			if( m_ObjectSectionsInfo[i].m_Magic == in_rResult.m_Magic )
			{
				FoundAt = i;
				break;
			}
		}
		assert( 0<=FoundAt && FoundAt<m_ObjectSectionsInfo.size() );
		assert( SECTION_ID_ERROR == m_ObjectSectionsInfo[FoundAt].m_SectionID );

		m_ObjectSectionsInfo[FoundAt].m_SectionID = in_rResult.m_NewSectionID;
		m_NPendingObjectSections--;
		if( 0 == m_NPendingObjectSections )
			SendWorkerInitCommands();
	}
}


