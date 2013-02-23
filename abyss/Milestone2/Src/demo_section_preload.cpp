#include "../Include/demo_section.h"
#include "../Include/object_section_interface.h"
#include "../Include/stars_section_interface.h"
#include "../Include/sun_section_interface.h"
#include "../../MT_Core/Include/log.h"

#include <assert.h>

/*

1. pro4itat' config fail
2. zapustit' worker sections
3. kazhdoj worker sekcii poslat` komandu CDemoSection_WorkerInit_Request
4. podozhdat` otveta CDemoSection_WorkerInit_Response ot vseh worker sekcij
4.1. zapolnit' m_StoredInfo
5. enter render loop

*/

const char* g_ConfigFileName = "/data/config.txt";

void
CDemoSection::StartTheGame()
{
	assert( SECTION_ID_ERROR != GetPreloaderSectionID() );
	assert( SECTION_ID_ERROR != GetVFSSectionID() );
	assert( SECTION_ID_ERROR != GetWindowSectionID() );
	assert( SECTION_ID_ERROR != GetRenderSectionID() );

	CTCommandSender< CVFSSection_MountRequest >::SendCommand(
		GetVFSSectionID(),
		CVFSSection_MountRequest(".")
	);
}

void
CDemoSection::Reaction(long in_SrcSectionID,const CVFSSection_MountResponse& in_rMountResponse)
{
	CTCommandSender< CVFSSection_OpenRequest >::SendCommand(
		GetVFSSectionID(),
		CVFSSection_OpenRequest(g_ConfigFileName)
	);
}

void
CDemoSection::Reaction(long in_SrcSectionID,const CVFSSection_OpenResponse& in_rOpenResponse)
{
	assert( in_rOpenResponse.m_Url == g_ConfigFileName );
	m_ConfigFileHandle = in_rOpenResponse.m_Handle;
	CTCommandSender< CVFSSection_ReadRequest >::SendCommand(
		GetVFSSectionID(),
		CVFSSection_ReadRequest( m_ConfigFileHandle, 0, -1 )
	);
}

void
CDemoSection::Reaction(long in_SrcSectionID,const CVFSSection_ReadResponse& in_rReadResponse)
{
	m_ConfigData = in_rReadResponse.m_pData;
	CTCommandSender< CVFSSection_CloseRequest >::SendCommand(
		GetVFSSectionID(),
		CVFSSection_CloseRequest(m_ConfigFileHandle)
	);
}

void
CDemoSection::Reaction(long in_SrcSectionID,const CVFSSection_CloseResponse& in_rCloseResponse)
{
	assert( in_rCloseResponse.m_Handle == m_ConfigFileHandle );

	// parse config file here
	ParseConfig();
	assert( !m_ConfigFileNames.empty() );

	m_ObjectSectionsInfo.resize( m_ConfigFileNames.size() );

	// launch sections
	for(unsigned long i=0; i<m_ObjectSectionsInfo.size(); ++i)
	{
		m_ObjectSectionsInfo[i].m_Magic = 666000+i;
		if( ET_STARS == m_ConfigFileNames[i].m_Type )
		{
			CTCommandSender< CCommand_LaunchSection >::SendCommand(
				SECTION_ID_SYSTEM,
				CCommand_LaunchSection( CStarsSection_CLSID, m_ObjectSectionsInfo[i].m_Magic )
			);
		}
		else if( ET_PLANET == m_ConfigFileNames[i].m_Type )
		{
			CTCommandSender< CCommand_LaunchSection >::SendCommand(
				SECTION_ID_SYSTEM,
				CCommand_LaunchSection( CObjectSection_CLSID, m_ObjectSectionsInfo[i].m_Magic )
			);
		}
		else if( ET_SUN == m_ConfigFileNames[i].m_Type )
		{
			CTCommandSender< CCommand_LaunchSection >::SendCommand(
				SECTION_ID_SYSTEM,
				CCommand_LaunchSection( CSunSection_CLSID, m_ObjectSectionsInfo[i].m_Magic )
			);
		}
		else
		{
			assert(false);
		}
	}
	m_NPendingObjectSections = m_ObjectSectionsInfo.size();
	m_InitStage = EIS_AWAITING_WORKERS_LAUNCH;
}

void
CDemoSection::Reaction(long in_SrcSectionID,const CDemoSection_WorkerInit_Response& in_rResponse)
{
CLog::Print("CDemoSection::Reaction( const CDemoSection_WorkerInit_Response& )\n");
CLog::Print("  sectionID = %ld\n",in_SrcSectionID);
CLog::Print("  success   = %s\n",in_rResponse.m_bSuccess?"YES":"NO");
	if(EIS_AWAITING_WORKERS_INIT==m_InitStage)
	{
		assert( in_rResponse.m_bSuccess );

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

		// convert info
		CStoredInfo::value_type InsertData( in_SrcSectionID, std::vector<CRenderable>() );
		InsertData.second.resize( in_rResponse.m_Renderables.size() );
		for(size_t i=0; i<in_rResponse.m_Renderables.size(); ++i)
		{
			InsertData.second[i].m_VBHandle       = in_rResponse.m_Renderables[i].m_VBHandle;
			InsertData.second[i].m_PSHandle       = in_rResponse.m_Renderables[i].m_PSHandle;
			InsertData.second[i].m_PrimitiveType  = in_rResponse.m_Renderables[i].m_PrimitiveType;
			InsertData.second[i].m_PrimitiveCount = in_rResponse.m_Renderables[i].m_PrimitiveCount;
			InsertData.second[i].m_Position       = in_rResponse.m_Renderables[i].m_InitialPosition;
			InsertData.second[i].m_Type           = in_rResponse.m_Renderables[i].m_Type;
		}
		// add info
		std::pair< CStoredInfo::iterator, bool > InsertResult = m_StoredInfo.insert( InsertData );
		assert( true == InsertResult.second );

		m_NPendingObjectSections--;
		if( 0 == m_NPendingObjectSections )
		{
CLog::Print("  entering main render loop\n");
			CTCommandSender<CDemoSection_StartRender>::SendCommand(
				GetThisID(),
				CDemoSection_StartRender()
			);
			m_InitStage = EIS_DONE;
		}
	}
}

void
CDemoSection::ParseConfig()
{
	CLog::Print("CDemoSection::ParseConfig()\n");
	if(!m_ConfigData.empty())
	{
		std::string ConfigData( (const char*)&m_ConfigData[0], m_ConfigData.size() );
		const char* pData = ConfigData.c_str();
		while( pData && (*pData)!='\0' )
		{
			const char* pEndLine = pData;
			while( *pEndLine != '\r' && *pEndLine != '\n' && *pEndLine != '\0' )
				pEndLine++;
			std::string Stroka( pData, pEndLine-pData );
			const char* pS = Stroka.c_str();
			CLog::Print("  stroka = '%s'\n", pS );
			//
			E_TYPE Type=ET_UNKNOWN;
			if( 0 == strncmp(pS,"stars",5) )
			{
				Type = ET_STARS;
				pS += 5;
			}
			else if( 0 == strncmp(pS,"planet",6) )
			{
				Type = ET_PLANET;
				pS += 6;
			}
			else if( 0 == strncmp(pS,"sun",3) )
			{
				Type = ET_SUN;
				pS += 3;
			}
			//else if( 0 == strncmp(pS,"sun",3) )
			//{
			//	Type = ET_SUN;
			//	pS += 3;
			//}
			else
			{
				assert(false);
			}
			while(*pS==' ' || *pS=='\t')
				pS++;
			assert(*pS!='\0');
			//
			m_ConfigFileNames.push_back( CConfigInfo(Type,pS) );
			while( *pEndLine == '\r' || *pEndLine == '\n' )
				pEndLine++;
			pData = pEndLine;
		}
	}
}

void
CDemoSection::SendWorkerInitCommands()
{
CLog::Print("CDemoSection::SendWorkerInitCommands()\n");
	assert( m_ConfigFileNames.size() == m_ObjectSectionsInfo.size() );
	for(unsigned long i=0; i<m_ObjectSectionsInfo.size(); ++i)
	{
		assert( SECTION_ID_ERROR != m_ObjectSectionsInfo[i].m_SectionID );
		CTCommandSender< CDemoSection_WorkerInit_Request >::SendCommand(
			m_ObjectSectionsInfo[i].m_SectionID,
			CDemoSection_WorkerInit_Request( m_ConfigFileNames[i].m_FileName, GetVFSSectionID(), GetPreloaderSectionID(), GetRenderSectionID() )
		);
		if(ET_SUN==m_ConfigFileNames[i].m_Type)
		{
			CDemoSection_WorkerCameraInform Cmd;
			Cmd.m_CameraRight = CVector(1,0,0);
			Cmd.m_CameraUp = CVector(0,1,0);
			CTCommandSender< CDemoSection_WorkerCameraInform >::SendCommand(
				m_ObjectSectionsInfo[i].m_SectionID
				,Cmd
			);
			assert(SECTION_ID_ERROR==m_SunSectionID);
			m_SunSectionID = m_ObjectSectionsInfo[i].m_SectionID;
			m_LastCameraUpdateTime = GetTickCount();
		}
	}
	m_NPendingObjectSections = m_ObjectSectionsInfo.size();
	m_InitStage = EIS_AWAITING_WORKERS_INIT;
}





























