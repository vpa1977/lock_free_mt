#include "../Include/test3.h"
#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/section_registrator.h"
#include "../../MT_Core/Include/log.h"
#include "../Include/worker_section_interface.h"
#include <assert.h>
#include <stdlib.h>
#include <time.h>

static CTSectionRegistrator<CTest3Section_CLSID,CTest3Section> g_CTest3SectionRegistrator( SINGLE_INSTANCE_ONLY, STARTUP_SECTION );

extern void Touch_WindowSection();

#define QUERY_MT_INFO_MAGIC			1
#define LAUNCH_THREADS_MAGIC		2
#define LAUNCH_WINDOW_SECTION_MAGIC	3
#define CREATE_WINDOW_MAGIC			4
#define FIRST_WORKER_MAGIC			1000

#define TEST_WINDOW_WIDTH			640
#define TEST_WINDOW_HEIGHT			480
#define TEST_CHUNK_SIZE				32

CTest3Section::CTest3Section( long in_SectionID ) :
	CSection( in_SectionID )
	,m_WindowSectionID(SECTION_ID_ERROR)
	,m_bWindowCreated(false)
	,m_NWorkers(0)
	,m_NProcessors(0)
	,m_bWorking(false)
{
	RegisterReaction( GetID((CCommand_SectionStartup*)NULL),			CTConform<CTest3Section,CCommand_SectionStartup>::Conform );
	RegisterReaction( GetID((CCommand_MTInfo*)NULL),					CTConform<CTest3Section,CCommand_MTInfo>::Conform );
	RegisterReaction( GetID((CCommand_LaunchThreadsResult*)NULL),		CTConform<CTest3Section,CCommand_LaunchThreadsResult>::Conform );
	RegisterReaction( GetID((CCommand_LaunchSectionResult*)NULL),		CTConform<CTest3Section,CCommand_LaunchSectionResult>::Conform );
	RegisterReaction( GetID((CCommand_FindSectionResult*)NULL),			CTConform<CTest3Section,CCommand_FindSectionResult>::Conform );
	RegisterReaction( GetID((CWindowSection_CreateWindowResult*)NULL),	CTConform<CTest3Section,CWindowSection_CreateWindowResult>::Conform );
	RegisterReaction( GetID((CWindowSection_CloseWindowResult*)NULL),	CTConform<CTest3Section,CWindowSection_CloseWindowResult>::Conform );
	RegisterReaction( GetID((CWindowSection_WindowParamsResult*)NULL),	CTConform<CTest3Section,CWindowSection_WindowParamsResult>::Conform );
	RegisterReaction( GetID((CWindowSection_WindowClosed*)NULL),		CTConform<CTest3Section,CWindowSection_WindowClosed>::Conform );
	RegisterReaction( GetID((CWindowSection_Char*)NULL),				CTConform<CTest3Section,CWindowSection_Char>::Conform );
	RegisterReaction( GetID((CWorkerSection_SetupSceneComplete*)NULL),	CTConform<CTest3Section,CWorkerSection_SetupSceneComplete>::Conform );
	RegisterReaction( GetID((CWorkerSection_PictureRendered*)NULL),		CTConform<CTest3Section,CWorkerSection_PictureRendered>::Conform );
}

CTest3Section::~CTest3Section()
{
	Touch_WindowSection();
}

void
CTest3Section::Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )
{
	CTCommandSender<CCommand_QueryMTInfo>::SendCommand( SECTION_ID_SYSTEM, CCommand_QueryMTInfo(QUERY_MT_INFO_MAGIC) );
}

void
CTest3Section::Reaction( long in_SrcSectionID, const CCommand_MTInfo& in_rMTInfoCommand )
{
	CLog::Print("CTest3Section::Reaction( const CCommand_MTInfo& in_rMTInfoCommand )\n");
	assert(in_rMTInfoCommand.m_Magic==QUERY_MT_INFO_MAGIC);
	m_NProcessors = in_rMTInfoCommand.m_NProcessors;
	CLog::Print("  m_NProcessors = %ld\n",m_NProcessors);
	{
		extern const char* g_CmdLine;
		if(g_CmdLine)
		{
			long N = atol(g_CmdLine);
			if(N>0)
				m_NWorkers = N;
			else
				m_NWorkers = in_rMTInfoCommand.m_NProcessors;
		}
		else
			m_NWorkers = in_rMTInfoCommand.m_NProcessors;
	}
	long NThreadsToRun = (m_NWorkers+1)<=in_rMTInfoCommand.m_MaxThreads ? m_NWorkers+1:in_rMTInfoCommand.m_MaxThreads;
	CLog::Print("  m_NWorkers = %ld\n",m_NWorkers);
	CLog::Print("  NThreadsToRun = %ld\n",NThreadsToRun);
	CTCommandSender<CCommand_LaunchThreads>::SendCommand(
		SECTION_ID_SYSTEM,
		CCommand_LaunchThreads(NThreadsToRun,LAUNCH_THREADS_MAGIC)
	);
	CLog::Print("CTest3Section::Reaction( const CCommand_MTInfo& in_rMTInfoCommand ) end\n");
}

void
CTest3Section::Reaction( long in_SrcSectionID, const CCommand_LaunchThreadsResult& in_rResult )
{
	CLog::Print("CTest3Section::Reaction( const CCommand_LaunchThreadsResult& in_rResult )\n");
	CLog::Print("  in_rResult.m_Result = %ld\n",in_rResult.m_Result);
	assert(in_rResult.m_Magic==LAUNCH_THREADS_MAGIC);
	assert(in_rResult.m_Result==TLR_OK);
	CTCommandSender<CCommand_LaunchSection>::SendCommand(
		SECTION_ID_SYSTEM,
		CCommand_LaunchSection(
			CWindowSection_CLSID,
			LAUNCH_WINDOW_SECTION_MAGIC,
			SF_BOUND|SF_EXCLUSIVE
		)
	);
	CWorkerMap_by_Magic& rIndexMagic = m_WorkersMap.get<Tag_Magic>();
	for( long i=0; i<(long)m_NWorkers; ++i )
	{
		long Magic = i + FIRST_WORKER_MAGIC;
		CTCommandSender<CCommand_LaunchSection>::SendCommand(
			SECTION_ID_SYSTEM,
			CCommand_LaunchSection(
				CWorkerSection_CLSID,
				Magic
			)
		);
		rIndexMagic.insert( CWorkerInfo(Magic) );
	}
	CLog::Print("CTest3Section::Reaction( const CCommand_LaunchThreadsResult& in_rResult ) end\n");
}

void
CTest3Section::Reaction( long in_SrcSectionID, const CCommand_LaunchSectionResult& in_rResult )
{
	CLog::Print("CTest3Section::Reaction( const CCommand_LaunchSectionResult& in_rResult )\n");
	CLog::Print("  in_rResult.m_ErrorCode = %ld\n",in_rResult.m_ErrorCode);
	assert(in_rResult.m_ErrorCode==SLR_OK);
	if(in_rResult.m_Magic==LAUNCH_WINDOW_SECTION_MAGIC) // window section
	{
		CLog::Print("  window section launched\n");
		m_WindowSectionID = in_rResult.m_NewSectionID;
		assert(m_WindowSectionID>=SECTION_ID_FIRST_AVAILABLE);
		char Buffer[1024];
		sprintf(Buffer,"%ld processor(s), %ld worker(s). Rendering...",m_NProcessors,m_NWorkers);
		CTCommandSender<CWindowSection_CreateWindow>::SendCommand(
			m_WindowSectionID,
			CWindowSection_CreateWindow(
				WS_CONSTANT,
				TEST_WINDOW_WIDTH,
				TEST_WINDOW_HEIGHT,
				Buffer,
				CREATE_WINDOW_MAGIC
			)
		);
	}
	else // workers
	{
		CLog::Print("  worker launched\n");
		CWorkerMap_by_Magic& rIndexMagic = m_WorkersMap.get<Tag_Magic>();
		CWorkerMap_by_Magic::iterator It = rIndexMagic.find( in_rResult.m_Magic );
		assert( It != rIndexMagic.end() );
		AssignSectionIDAndRunningFlag Functor(in_rResult.m_NewSectionID);
		rIndexMagic.modify( It, Functor );
		CTCommandSender<CCommand_FindSection>::SendCommand(
			SECTION_ID_SYSTEM,
			CCommand_FindSection( CWorkerSection_CLSID, 666 )
		);
	}
	CheckWorkersAndWindow();
	CLog::Print("CTest3Section::Reaction( const CCommand_LaunchSectionResult& in_rResult ) end\n");
}

void
CTest3Section::Reaction( long in_SrcSectionID, const CCommand_FindSectionResult& in_rResult )
{
	CLog::Print("CTest3Section::Reaction( const CCommand_FindSectionResult& in_rResult )\n");
	assert( in_rResult.m_Magic == 666 );
	for( std::vector<long>::const_iterator It=in_rResult.m_SectionIDs.begin(); It!=in_rResult.m_SectionIDs.end(); ++It )
	{
		CLog::Print("  found: %ld\n",*It);
	}
	CLog::Print("CTest3Section::Reaction( const CCommand_FindSectionResult& in_rResult ) end\n");
}

void
CTest3Section::Reaction( long in_SrcSectionID, const CWindowSection_CreateWindowResult& in_rResult )
{
	CLog::Print("CTest3Section::Reaction( const CWindowSection_CreateWindowResult& in_rResult )\n");
	CLog::Print("  in_rResult.m_Result = %ld\n",in_rResult.m_Result);
	assert(in_rResult.m_MagicID==CREATE_WINDOW_MAGIC);
	assert(in_rResult.m_Result==CWR_OK);
	m_bWindowCreated = true;
	CheckWorkersAndWindow();
	CLog::Print("CTest3Section::Reaction( const CWindowSection_CreateWindowResult& in_rResult ) end\n");
}

void
CTest3Section::Reaction( long in_SrcSectionID, const CWindowSection_CloseWindowResult& in_rResult )
{
	assert(in_rResult.m_MagicID==CREATE_WINDOW_MAGIC);
	assert(in_rResult.m_Result==CLWR_OK);
}

void
CTest3Section::Reaction( long in_SrcSectionID, const CWindowSection_WindowParamsResult& in_rResult )
{
	assert(in_rResult.m_MagicID==CREATE_WINDOW_MAGIC);
	assert(in_rResult.m_Result==WPR_OK);
}

void
CTest3Section::Reaction( long in_SrcSectionID, const CWindowSection_WindowClosed& in_rCommand )
{
	CLog::Print("CTest3Section::Reaction( const CWindowSection_WindowClosed& in_rCommand )\n");
	assert(in_rCommand.m_MagicID==CREATE_WINDOW_MAGIC);
	CTCommandSender<CCommand_QuitApplication>::SendCommand(
		SECTION_ID_SYSTEM,
		CCommand_QuitApplication(0)
	);
	CLog::Print("CTest3Section::Reaction( const CWindowSection_WindowClosed& in_rCommand ) end\n");
}

bool SizeFlag = true;
long TitleCounter = 1;
void
CTest3Section::Reaction( long in_SrcSectionID, const CWindowSection_Char& in_rCommand )
{
	assert(false);
	switch(in_rCommand.m_Ch)
	{
		case 'q':
		case 'Q':
			// close window
			CTCommandSender<CWindowSection_CloseWindow>::SendCommand(
				m_WindowSectionID,
				CWindowSection_CloseWindow(CREATE_WINDOW_MAGIC,GetThisID())
			);
			break;
		case 'n':
		case 'N':
			// window title
//			{
//				char Buffer[1024];
//				sprintf(Buffer,"Test3. %ld",TitleCounter++);
//				CTCommandSender<CWindowSection_WindowName>::SendCommand(
//					m_WindowSectionID,
//					CWindowSection_WindowName(std::string(Buffer))
//				);
//			}
			break;
		case 'f':
		case 'F':
			// test
//			CTCommandSender<CWindowSection_TestCommand>::SendCommand(
//				m_WindowSectionID,
//				CWindowSection_TestCommand()
//			);
			break;
		case 's':
		case 'S':
			//
//			SizeFlag = !SizeFlag;
//			CTCommandSender<CWindowSection_WindowParams>::SendCommand(
//				m_WindowSectionID,
//				CWindowSection_WindowParams(
//					SizeFlag ? WS_MINMAX : WS_CONSTANT,
//					SizeFlag ? 512 : 640,
//					SizeFlag ? 384 : 480,
//					CREATE_WINDOW_MAGIC,
//					GetThisID()
//				)
//			);
			break;
		default:
			break;
	}
}

void
CTest3Section::CheckWorkersAndWindow()
{
	if(m_bWindowCreated)
	{
		CWorkerMap_by_Running& rIndexRunning = m_WorkersMap.get<Tag_Running>();
		if(!rIndexRunning.empty())
		{
			std::pair<CWorkerMap_by_Running::iterator,CWorkerMap_by_Running::iterator> RangeNotRunning =
				rIndexRunning.equal_range( false );
			if(RangeNotRunning.first == RangeNotRunning.second)
			{
				StartWorking();
			}
		}
	}
}

void
CTest3Section::StartWorking()
{
	CLog::Print("  CTest3Section::StartWorking()\n");
	if(!m_bWorking)
	{
		CLog::Print("    yep\n");
		m_bWorking = true;

		CWorkerMap_by_SectionID& rIndexWorkers = m_WorkersMap.get<Tag_SectionID>();
		CWorkerMap_by_SectionID::iterator It = rIndexWorkers.begin();
		assert( It != rIndexWorkers.end() );
		CTCommandSender<CWorkerSection_SetupScene>::SendCommand(
			(*It).m_SectionID,
			CWorkerSection_SetupScene(TEST_WINDOW_WIDTH,TEST_WINDOW_HEIGHT)
		);
	}
	CLog::Print("  CTest3Section::StartWorking() end\n");
}

void
CTest3Section::Reaction( long in_SrcSectionID, const CWorkerSection_SetupSceneComplete& in_rCommand )
{
	CLog::Print("CTest3Section::Reaction( const CWorkerSection_SetupSceneComplete& in_rCommand )\n");
	long nChunksDX = TEST_WINDOW_WIDTH/TEST_CHUNK_SIZE;
	long nChunksDY = TEST_WINDOW_HEIGHT/TEST_CHUNK_SIZE;
	long nChunks = nChunksDX*nChunksDY;

	std::list<CChunkPosition> SrcChunks;
	for(long j=0; j<nChunksDY; ++j)
	{
		for(long i=0; i<nChunksDX; ++i)
		{
			SrcChunks.push_back( CChunkPosition(i*TEST_CHUNK_SIZE,j*TEST_CHUNK_SIZE) );
		}
	}

	m_Chunks.clear();
	srand( (unsigned)time( NULL ) );
	for(long i=0; i<nChunks; ++i)
	{
		assert( !SrcChunks.empty() );
		unsigned long Q = rand()%nChunks;
		unsigned long S = (unsigned long)SrcChunks.size();
		Q = Q % S;
		std::list<CChunkPosition>::iterator It = SrcChunks.begin();
		for( unsigned long ii=0; ii<Q; ++ii )
			++It;
		assert( It != SrcChunks.end() );
		m_Chunks.push_back( *It );
		SrcChunks.erase( It );
	}

	m_StartTime = GetTickCount();

	CWorkerMap_by_SectionID& rIndexWorkers = m_WorkersMap.get<Tag_SectionID>();
	CWorkerMap_by_SectionID::iterator It = rIndexWorkers.begin();
	for( CWorkerMap_by_SectionID::iterator It = rIndexWorkers.begin(); It!=rIndexWorkers.end(); ++It)
	{
		assert( !m_Chunks.empty() );
		CChunkPosition& rChunk = m_Chunks.front();
		CTCommandSender<CWorkerSection_RenderPicture>::SendCommand(
			(*It).m_SectionID,
			CWorkerSection_RenderPicture(
				rChunk.m_X,
				rChunk.m_Y,
				TEST_CHUNK_SIZE,
				TEST_CHUNK_SIZE,
				TEST_WINDOW_WIDTH,
				TEST_WINDOW_HEIGHT,
				m_WindowSectionID,
				false
			)
		);
		m_Chunks.pop_front();
	}
	CLog::Print("CTest3Section::Reaction( const CWorkerSection_SetupSceneComplete& in_rCommand ) end\n");
}

void
CTest3Section::Reaction( long in_SrcSectionID, const CWorkerSection_PictureRendered& in_rCommand )
{
	CLog::Print("CTest3Section::Reaction( const CWorkerSection_PictureRendered& in_rCommand )\n");
	if(in_rCommand.m_bLast)
	{
		CLog::Print("  last\n");
		assert( m_Chunks.empty() );
		char Buffer[1024];
		DWORD StopTime = GetTickCount();
		unsigned long TimeElapsed = StopTime-m_StartTime;
		unsigned long Miliseconds = TimeElapsed % 1000;
		TimeElapsed /= 1000;
		unsigned long Minutes = TimeElapsed / 60;
		unsigned long Seconds = TimeElapsed % 60;
//		sprintf( Buffer, "Render time: %ldmin %ld.%ldsec", Minutes, Seconds, Miliseconds );
		sprintf( Buffer,"%ld processor(s), %ld worker(s). Time: %ldmin %ldsec %ldms", m_NProcessors, m_NWorkers, Minutes, Seconds, Miliseconds);
		CTCommandSender<CWindowSection_WindowName>::SendCommand(
			m_WindowSectionID,
			CWindowSection_WindowName(std::string(Buffer))
		);
	}
	if( !m_Chunks.empty() )
	{
		CChunkPosition& rChunk = m_Chunks.front();
		CTCommandSender<CWorkerSection_RenderPicture>::SendCommand(
			in_rCommand.m_WorkerID,
			CWorkerSection_RenderPicture(
				rChunk.m_X,
				rChunk.m_Y,
				TEST_CHUNK_SIZE,
				TEST_CHUNK_SIZE,
				TEST_WINDOW_WIDTH,
				TEST_WINDOW_HEIGHT,
				m_WindowSectionID,
				m_Chunks.size()==1
			)
		);
		m_Chunks.pop_front();
	}
	CLog::Print("CTest3Section::Reaction( const CWorkerSection_PictureRendered& in_rCommand ) end\n");
}










