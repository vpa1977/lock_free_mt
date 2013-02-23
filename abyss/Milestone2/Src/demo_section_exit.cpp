#include "../Include/demo_section.h"
#include "../../MT_Core/Include/log.h"

void
CDemoSection::Received_WindowClosed()
{
	CLog::Print("============================= WINDOW CLOSED ======================\n");
	for(unsigned long i=0; i<m_ObjectSectionsInfo.size(); ++i)
	{
		assert( SECTION_ID_ERROR != m_ObjectSectionsInfo[i].m_SectionID );
		CTCommandSender< CDemoSection_WorkerShutdown_Request >::SendCommand(
			m_ObjectSectionsInfo[i].m_SectionID,
			CDemoSection_WorkerShutdown_Request()
		);
	}
	m_NPendingObjectSections = m_ObjectSectionsInfo.size();
	m_InitStage = EIS_SHUTDOWN;
}

void
CDemoSection::Reaction( long in_SrcSectionID, const CDemoSection_WorkerShutdown_Response& in_rResponse)
{
	if(EIS_SHUTDOWN==m_InitStage)
	{
		// find what worker is it
		long FoundAt = -1;
		for(size_t i=0; i<m_ObjectSectionsInfo.size(); ++i)
		{
			if( m_ObjectSectionsInfo[i].m_SectionID == in_SrcSectionID )
			{
				FoundAt = i;
				break;
			}
		}
		assert( 0<=FoundAt && FoundAt<m_ObjectSectionsInfo.size() );
		assert( m_ObjectSectionsInfo[FoundAt].m_bAlive );
		m_ObjectSectionsInfo[FoundAt].m_bAlive = false;
		m_NPendingObjectSections--;
		if( 0 == m_NPendingObjectSections )
		{
			CLog::Print("========== ALL WORKERS QUIT =============\n");
			CTCommandSender< CCommand_QuitSystem >::SendCommand(
				SECTION_ID_SYSTEM,
				CCommand_QuitSystem()
			);
		}
	}
}

void
CDemoSection::Reaction(long in_SrcSectionID,const CVFSSection_UMountResponse& in_rUMountResponse)
{
}
