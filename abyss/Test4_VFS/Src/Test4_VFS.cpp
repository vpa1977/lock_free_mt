
#include <vector>
#include "../../MT_Core/Include/command.h"
#include "../../MT_Core/Include/command_sender.h"
#include "../../VFS/Include/section_vfs_interface.h "

#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/section_registrator.h"
#include "../../App_Win32/include/window_section.h"
#include "../../App_Win32/include/window_section_interface.h "

#define QUERY_MT_INFO_MAGIC			1
#define LAUNCH_THREADS_MAGIC		2
#define LAUNCH_WINDOW_SECTION_MAGIC	3
#define CREATE_WINDOW_MAGIC			4
#define FIRST_WORKER_MAGIC			1000
#define VFS_MAGIC                   99

#define TEST_WINDOW_WIDTH			640
#define TEST_WINDOW_HEIGHT			480
#define TEST_CHUNK_SIZE				32

//static CTSectionRegistrator<CVFSSection_CLSID,CVFSSection> g_CVFSSectionRegistrator(false);
//static CTSectionRegistrator<CVFSResource_CLSID,CVFSResource> g_CVFSResourceRegistrator(true);
//static CTSectionRegistrator<CARCResource_CLSID,CArchiveResource> g_CArcResourceRegistrator(true);


class CSectionLink
{
public:
	CSectionLink(const char* inpLinkName, long inSrcSectionID, long inDstSectionID)
	{
		m_SrcSectionID = inSrcSectionID;
		m_DstSectionID = inDstSectionID;
		m_hMutex = CreateMutex(NULL, false, inpLinkName);
	}

	

private:

	long m_ThreadID;
	long m_SrcSectionID;
	long m_DstSectionID;
	HANDLE m_hMutex;
};

extern void Touch_VFSSection();




class TestSection 
	: public CSection 
//	,public CTCommandSender<CBuffer>
//	,public CTCommandSender<CCommand_QuitSection>
	,public CTCommandSender<CCommand_LaunchSection>
	,public CTCommandSender<CCommand_QueryMTInfo>
	,public CTCommandSender<CCommand_LaunchThreads>
	,public CTCommandSender<CWindowSection_CreateWindow>
	,public CTCommandSender<CVFSSection_MountRequest>
	,public CTCommandSender<CVFSSection_UMountRequest>
	,public CTCommandSender<CVFSSection_ReadRequest>
	,public CTCommandSender<CVFSSection_CloseRequest>
	,public CTCommandSender<CVFSSection_OpenRequest>
{
public:
	TestSection( long in_SectionID );
	~TestSection(){};
	std::string GetName() { return "TESTSECTION"; };
	void Reaction(long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand );
	void Reaction(long in_SrcSectionID, const CCommand_MTInfo& in_rMTInfoCommand );
	void Reaction(long in_SrcSectionID, const CCommand_LaunchThreadsResult& in_rResult );
	void Reaction(long in_SrcSectionID, const CCommand_LaunchSectionResult& in_rResult );
	void Reaction(long in_SrcSectionID,const CVFSSection_MountResponse& in_rMountResponse);
	void Reaction(long in_SrcSectionID,const CVFSSection_UMountResponse& in_rMountResponse);
	void Reaction(long in_SrcSectionID,const CVFSSection_ReadResponse& in_rBufferResponse);
	void Reaction(long in_SrcSectionID,const CVFSSection_CloseResponse& in_rCloseResponse){};
	void Reaction(long in_SrcSectionID,const CVFSSection_OpenResponse& in_rCloseResponse);
private: 
	int m_vfsDispatch;
	long m_Handle;
	
};


TestSection::TestSection( long in_SectionID ) :CSection( in_SectionID )
{
	Touch_VFSSection();
	printf("TestSection::TestSection()\n");
	RegisterReaction( GetID((CCommand_SectionStartup*)NULL), CTConform<TestSection,CCommand_SectionStartup>::Conform );
	RegisterReaction( GetID((CCommand_MTInfo*)NULL), CTConform<TestSection,CCommand_MTInfo>::Conform );
	RegisterReaction( GetID((CCommand_LaunchThreadsResult*)NULL), CTConform<TestSection,CCommand_LaunchThreadsResult>::Conform );
	RegisterReaction( GetID((CCommand_LaunchSectionResult*)NULL), CTConform<TestSection,CCommand_LaunchSectionResult>::Conform );
	RegisterReaction( GetID((CVFSSection_MountResponse*)NULL), CTConform<TestSection,CVFSSection_MountResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_UMountResponse*)NULL), CTConform<TestSection,CVFSSection_UMountResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_ReadResponse*)NULL), CTConform<TestSection,CVFSSection_ReadResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_CloseResponse*)NULL), CTConform<TestSection,CVFSSection_CloseResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_OpenResponse*)NULL), CTConform<TestSection,CVFSSection_OpenResponse>::Conform );
	printf("TestSection::TestSection() end\n");
}

void TestSection::Reaction(long in_SrcSectionID,const CVFSSection_ReadResponse& in_rReadBuffer)
{
	printf("Read Buffer recieved \n");

	char test[1024];
	memset(test,0,1024);
	int sss = in_rReadBuffer.m_Size;
	if (in_rReadBuffer.m_Size > 1024 ) 
	{
		sss = 1024;
	}
	
	strncpy(test,(const char*) &(in_rReadBuffer.m_pData[0]), sss
		);

	CVFSSection_UMountRequest req;
	CTCommandSender<CVFSSection_UMountRequest>::SendCommand( m_vfsDispatch,  req);


}
void TestSection::Reaction(long in_SrcSectionID,const CVFSSection_UMountResponse& in_rMountResponse)
{
		CTCommandSender<CVFSSection_MountRequest>::SendCommand( m_vfsDispatch ,
			CVFSSection_MountRequest("c:/1")); 

}

void TestSection::Reaction(long in_SrcSectionID,const CVFSSection_OpenResponse& in_rResponse)
{
	printf("Open response recieved \n");
	m_Handle = in_rResponse.m_Handle;

	CVFSSection_ReadRequest req;
	req.m_Handle = m_Handle;
	req.m_Offset = 0;
	req.m_Size = -1;
	CTCommandSender<CVFSSection_ReadRequest>::SendCommand( m_vfsDispatch,  req);

}


void TestSection::Reaction(long in_SrcSectionID,const CVFSSection_MountResponse& in_rMountResponse)
{
	printf("Mount response recieved \n");
	CVFSSection_OpenRequest req;
	req.m_Url = "/logs/history.txt";

	CTCommandSender<CVFSSection_OpenRequest>::SendCommand( m_vfsDispatch,  req);
}


void TestSection::Reaction(long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )
{
	CTCommandSender<CCommand_QueryMTInfo>::SendCommand( SECTION_ID_SYSTEM, CCommand_QueryMTInfo(QUERY_MT_INFO_MAGIC) );
}
void TestSection::Reaction( long in_SrcSectionID,const CCommand_MTInfo& in_rMTInfoCommand )
{

	assert(in_rMTInfoCommand.m_Magic==QUERY_MT_INFO_MAGIC);
	int m_NProcessors = in_rMTInfoCommand.m_NProcessors;

	long NThreadsToRun = in_rMTInfoCommand.m_MaxThreads;
	
	CTCommandSender<CCommand_LaunchThreads>::SendCommand(
		SECTION_ID_SYSTEM,
		CCommand_LaunchThreads(NThreadsToRun,LAUNCH_THREADS_MAGIC)
	);
	

}

void TestSection::Reaction( long in_SrcSectionID,const CCommand_LaunchThreadsResult& in_rResult )
{

	printf("Threads launched " );
	CTCommandSender<CCommand_LaunchSection>::SendCommand( SECTION_ID_SYSTEM, 
		CCommand_LaunchSection( CVFSSection_CLSID,VFS_MAGIC,SF_BOUND|SF_EXCLUSIVE ));
	printf("VFS section launched " );

}
void TestSection::Reaction( long in_SrcSectionID,const CCommand_LaunchSectionResult& in_rResult )
{
	printf("Section launched " );
	assert(in_rResult.m_ErrorCode==SLR_OK);
	m_vfsDispatch = in_rResult.m_NewSectionID;
		

		CTCommandSender<CVFSSection_MountRequest>::SendCommand( m_vfsDispatch ,
			CVFSSection_MountRequest("c:/1")); 


}


// do static registration of startup
#define TestSection_CLSID 432456
static CTSectionRegistrator<TestSection_CLSID,TestSection> g_CSectionARegistrator( true,STARTUP_SECTION );
