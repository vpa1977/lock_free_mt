#include "../Include/test5_section.h"
#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/section_registrator.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

static CTSectionRegistrator<CTest5Section_CLSID,CTest5Section> g_CTest5SectionRegistrator( SINGLE_INSTANCE_ONLY, STARTUP_SECTION );

extern void Touch_WindowSection();
extern void Touch_RenderSection();

#define QUERY_MT_INFO_MAGIC					1
#define LAUNCH_THREADS_MAGIC				2
#define LAUNCH_WINDOW_SECTION_MAGIC			3
#define CREATE_WINDOW_MAGIC					4
#define LAUNCH_RENDER_SECTION_MAGIC			5

#define TEST_WINDOW_WIDTH			640
#define TEST_WINDOW_HEIGHT			480

CTest5Section::CTest5Section( long in_SectionID ) :
	CSection( in_SectionID )
	,m_WindowSectionID(SECTION_ID_ERROR)
	,m_RenderSectionID(SECTION_ID_ERROR)
	,m_HWnd(NULL)
	,m_NFramesToRender(0)
	,m_Color(0)
	,m_LastTimeFrame(0)
	,m_NFrames(0)
	,m_Alpha(0.0f)
	,m_VBHandle(0)
	,m_PrimitiveType(0)
	,m_NVertices(0)
	,m_NPrimitives(0)
{
	//
	RegisterReaction( GetID((CCommand_SectionStartup*)NULL),			CTConform<CTest5Section,CCommand_SectionStartup>::Conform );
	RegisterReaction( GetID((CCommand_MTInfo*)NULL),					CTConform<CTest5Section,CCommand_MTInfo>::Conform );
	RegisterReaction( GetID((CCommand_LaunchThreadsResult*)NULL),		CTConform<CTest5Section,CCommand_LaunchThreadsResult>::Conform );
	RegisterReaction( GetID((CCommand_LaunchSectionResult*)NULL),		CTConform<CTest5Section,CCommand_LaunchSectionResult>::Conform );
	//
	RegisterReaction( GetID((CWindowSection_CreateWindowResult*)NULL),	CTConform<CTest5Section,CWindowSection_CreateWindowResult>::Conform );
	RegisterReaction( GetID((CWindowSection_WindowClosed*)NULL),		CTConform<CTest5Section,CWindowSection_WindowClosed>::Conform );
	//
	RegisterReaction( GetID((CRenderSection_BoundToWindow*)NULL),		CTConform<CTest5Section,CRenderSection_BoundToWindow>::Conform );
	RegisterReaction( GetID((CRenderSection_3DEnvironmentInfo*)NULL),	CTConform<CTest5Section,CRenderSection_3DEnvironmentInfo>::Conform );
	RegisterReaction( GetID((CRenderSection_InitRenderResult*)NULL),	CTConform<CTest5Section,CRenderSection_InitRenderResult>::Conform );
	RegisterReaction( GetID((CRenderSection_CloseRenderResult*)NULL),	CTConform<CTest5Section,CRenderSection_CloseRenderResult>::Conform );
	RegisterReaction( GetID((CRenderSection_NewFrame*)NULL),			CTConform<CTest5Section,CRenderSection_NewFrame>::Conform );
	RegisterReaction( GetID((CRenderSection_CreateVertexBuffer_Response*)NULL),			CTConform<CTest5Section,CRenderSection_CreateVertexBuffer_Response>::Conform );
	//
	RegisterReaction( GetID((CTest5Section_Render*)NULL),				CTConform<CTest5Section,CTest5Section_Render>::Conform );

//	RegisterReaction( GetID((CWindowSection_Activated*)NULL),			CTConform<CTest5Section,CWindowSection_Activated>::Conform );
//	RegisterReaction( GetID((CWindowSection_WindowSize*)NULL),			CTConform<CTest5Section,CWindowSection_WindowSize>::Conform );
//	RegisterReaction( GetID((CRenderSection_WindowClosedCleanupComplete*)NULL),	CTConform<CTest5Section,CRenderSection_WindowClosedCleanupComplete>::Conform );
}

CTest5Section::~CTest5Section()
{
	Touch_WindowSection();
	Touch_RenderSection();
}

void
CTest5Section::Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )
{
	CLog::Print("CTest5Section::Reaction( const CCommand_SectionStartup& in_rStartupCommand )\n");
	CLog::Print("  ID = %ld\n",GetThisID());
	CTCommandSender<CCommand_QueryMTInfo>::SendCommand( SECTION_ID_SYSTEM, CCommand_QueryMTInfo(QUERY_MT_INFO_MAGIC) );
	CLog::Print("CTest5Section::Reaction( const CCommand_SectionStartup& in_rStartupCommand ) end\n");
}

void
CTest5Section::Reaction( long in_SrcSectionID, const CCommand_MTInfo& in_rMTInfoCommand )
{
	CLog::Print("CTest5Section::Reaction( const CCommand_MTInfo& in_rMTInfoCommand )\n");
	assert(in_rMTInfoCommand.m_Magic==QUERY_MT_INFO_MAGIC);
	CLog::Print("  m_NProcessors = %ld\n",in_rMTInfoCommand.m_NProcessors);
	long NThreadsToRun = 2;
	CLog::Print("  NThreadsToRun = %ld\n",NThreadsToRun);
	CTCommandSender<CCommand_LaunchThreads>::SendCommand(
		SECTION_ID_SYSTEM,
		CCommand_LaunchThreads(NThreadsToRun,LAUNCH_THREADS_MAGIC)
	);
	CLog::Print("CTest5Section::Reaction( const CCommand_MTInfo& in_rMTInfoCommand ) end\n");
}

void
CTest5Section::Reaction( long in_SrcSectionID, const CCommand_LaunchThreadsResult& in_rResult )
{
	CLog::Print("CTest5Section::Reaction( const CCommand_LaunchThreadsResult& in_rResult )\n");
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
	CLog::Print("CTest5Section::Reaction( const CCommand_LaunchThreadsResult& in_rResult ) end\n");
}

void
CTest5Section::Reaction( long in_SrcSectionID, const CCommand_LaunchSectionResult& in_rResult )
{
	CLog::Print("CTest5Section::Reaction( const CCommand_LaunchSectionResult& in_rResult )\n");
	CLog::Print("  in_rResult.m_ErrorCode = %ld\n",in_rResult.m_ErrorCode);
	CLog::Print("  in_rResult.m_NewSectionID = %ld\n",in_rResult.m_NewSectionID);
	assert(in_rResult.m_ErrorCode==SLR_OK);
	if(in_rResult.m_Magic==LAUNCH_WINDOW_SECTION_MAGIC) // window section
	{
		CLog::Print("  window section launched\n");
		m_WindowSectionID = in_rResult.m_NewSectionID;
		assert(m_WindowSectionID>=SECTION_ID_FIRST_AVAILABLE);
		CTCommandSender<CWindowSection_CreateWindow>::SendCommand(
			m_WindowSectionID,
			CWindowSection_CreateWindow(
				WS_RESIZEABLE, //WS_MINMAX, //WS_CONSTANT,
				TEST_WINDOW_WIDTH,
				TEST_WINDOW_HEIGHT,
				"Render test.",
				CREATE_WINDOW_MAGIC
			)
		);
	}
	else if(in_rResult.m_Magic==LAUNCH_RENDER_SECTION_MAGIC) // render section
	{
		CLog::Print("  render section launched\n");
		m_RenderSectionID = in_rResult.m_NewSectionID;
		CTCommandSender<CRenderSection_BindToWindow>::SendCommand(
			m_RenderSectionID,
			CRenderSection_BindToWindow( m_WindowSectionID, m_HWnd )
		);
	}
	CLog::Print("CTest5Section::Reaction( const CCommand_LaunchSectionResult& in_rResult ) end\n");
}

// ===================================================================================================================

void
CTest5Section::Reaction( long in_SrcSectionID, const CWindowSection_CreateWindowResult& in_rResult )
{
	CLog::Print("CTest5Section::Reaction( const CWindowSection_CreateWindowResult& in_rResult )\n");
	CLog::Print("  in_rResult.m_Result = %ld\n",in_rResult.m_Result);
	assert(in_SrcSectionID==m_WindowSectionID);
	assert(in_rResult.m_MagicID==CREATE_WINDOW_MAGIC);
	assert(in_rResult.m_Result==CWR_OK);
	assert(in_rResult.m_HWnd!=NULL);
	m_HWnd = in_rResult.m_HWnd;
	CTCommandSender<CCommand_LaunchSection>::SendCommand(
		SECTION_ID_SYSTEM,
		CCommand_LaunchSection(
			CRenderSection_CLSID,
			LAUNCH_RENDER_SECTION_MAGIC
		)
	);
	CLog::Print("CTest5Section::Reaction( const CWindowSection_CreateWindowResult& in_rResult ) end\n");
}

void
CTest5Section::Reaction( long in_SrcSectionID, const CWindowSection_WindowClosed& in_rCommand )
{
	CLog::Print("CTest5Section::Reaction( const CWindowSection_WindowClosed& in_rCommand )\n");
	assert(m_HWnd!=NULL);
	assert(in_rCommand.m_HWnd==m_HWnd);
	CLog::Print("  thisID   = %ld\n",GetThisID());
	CLog::Print("  renderID = %ld\n",m_RenderSectionID);
	CTCommandSender<CRenderSection_WindowClosed>::SendCommand(
		m_RenderSectionID,
		CRenderSection_WindowClosed(m_HWnd)
	);
	m_HWnd = NULL;
	CLog::Print("CTest5Section::Reaction( const CWindowSection_WindowClosed& in_rCommand ) end\n");
}

// ===================================================================================================================

void
CTest5Section::Reaction( long in_SrcSectionID, const CRenderSection_BoundToWindow& in_rBound )
{
	CLog::Print("CTest5Section::Reaction( const CRenderSection_BoundToWindow& in_rBound )\n");
	assert( in_rBound.m_HWnd == m_HWnd );
	CTCommandSender<CRenderSection_Get3DEnvironmentInfo>::SendCommand(
		m_RenderSectionID,
		CRenderSection_Get3DEnvironmentInfo()
	);
	CLog::Print("CTest5Section::Reaction( const CRenderSection_BoundToWindow& in_rBound ) end\n");
}

void
CTest5Section::Reaction( long in_SrcSectionID, const CRenderSection_3DEnvironmentInfo& in_rInfo )
{
	CLog::Print("CTest5Section::Reaction( const CRenderSection_3DEnvironmentInfo& in_rInfo )\n");
	CLog::Print("  error ? : %s\n",in_rInfo.m_bError?"YES":"no");
	assert(!in_rInfo.m_bError);
	CLog::Print("  can render windowed   : %s\n",in_rInfo.m_CanRenderWindowed?"YES":"NO");
	CLog::Print("  number of video modes : %lu\n",in_rInfo.m_DisplayModes.size());
	CTCommandSender<CRenderSection_InitRender>::SendCommand(
		m_RenderSectionID,
		CRenderSection_InitRender(
			TEST_WINDOW_WIDTH, TEST_WINDOW_HEIGHT,
			true,
			RDMF_UNKNOWN,
			true,
			false,
			0,
			false
		)
	);
	CLog::Print("CTest5Section::Reaction( const CRenderSection_3DEnvironmentInfo& in_rInfo ) end\n");
}

struct CCustomVertex
{
	float x,y,z;
	unsigned long Color;
};

CCustomVertex vertices[] = {
	{ 0, 0, 0, 0xff0000 },
	{ 5, 0, 0, 0xff0000 },
	{ 0, 0, 0, 0xff00 },
	{ 0, 5, 0, 0xff00 },
	{ 0, 0, 0, 0xff },
	{ 0, 0, 5, 0xff }
};

unsigned long
CalcNPrimitives(unsigned long in_PrimitiveType, unsigned long in_NVertices)
{
	assert(in_PrimitiveType>=PRIM_POINT_LIST && in_PrimitiveType<=PRIM_TRIANGLE_FAN);
	switch(in_PrimitiveType)
	{
		case PRIM_POINT_LIST:
			return in_NVertices;
		case PRIM_LINE_LIST:
			return in_NVertices/2;
		case PRIM_LINE_STRIP:
			return in_NVertices-1;
		case PRIM_TRIANGLE_LIST:
			return in_NVertices/3;
		case PRIM_TRIANGLE_STRIP:
		case PRIM_TRIANGLE_FAN:
			return in_NVertices-2;
	}
	return 0;
}

void
CTest5Section::Reaction( long in_SrcSectionID, const CRenderSection_InitRenderResult& in_rResult )
{
	CLog::Print("CTest5Section::Reaction( const CRenderSection_InitRenderResult& in_rResult )\n");
	CLog::Print("  init %s\n", (in_rResult.m_Result==IRR_OK)? "SUCCEEDED":"FAILED" );
	assert(in_rResult.m_Result==IRR_OK);

	// create VB
//	CVBFormat VBFormat;
//	VBFormat.m_XYZ = true;
//	VBFormat.m_Diffuse = true;
//	CRenderSection_CreateVertexBuffer_Request Cmd( VBFormat, false );
//	Cmd.m_Data.resize( 6*sizeof(CCustomVertex) );
//	memcpy( (unsigned char*)&(Cmd.m_Data[0]), vertices, 6*sizeof(CCustomVertex) );
//	CTCommandSender<CRenderSection_CreateVertexBuffer_Request>::SendCommand(
//		m_RenderSectionID,
//		Cmd
//	);
	CVBFormat VBFormat;
	std::vector<unsigned char> Data;
	m_PrimitiveType = 0;
	bool Success = LoadDataFromFile("cube.txt",VBFormat,Data,m_PrimitiveType,m_NVertices);
	assert(Success);
CLog::Print("  m_PrimitiveType=%lu\n",m_PrimitiveType);
CLog::Print("  m_NVertices=%lu\n",m_NVertices);
	m_NPrimitives = CalcNPrimitives(m_PrimitiveType,m_NVertices);
CLog::Print("  m_NPrimitives=%lu\n",m_NPrimitives);
	CRenderSection_CreateVertexBuffer_Request Cmd( VBFormat, false );
	Cmd.m_Data = Data;
	CTCommandSender<CRenderSection_CreateVertexBuffer_Request>::SendCommand(
		m_RenderSectionID,
		Cmd
	);

	CLog::Print("CTest5Section::Reaction( const CRenderSection_InitRenderResult& in_rResult ) end\n");
}

void
CTest5Section::Reaction( long in_SrcSectionID, const CRenderSection_CreateVertexBuffer_Response& in_rCmd )
{
	CLog::Print("CTest5Section::Reaction( const CRenderSection_CreateVertexBuffer_Response& in_rCmd )\n");
	CLog::Print("  VB handle = %lu\n",in_rCmd.m_VBHandle);
	assert( in_rCmd.m_VBHandle > 0 );
	m_VBHandle = in_rCmd.m_VBHandle;
	// set lights
	CTCommandSender<CRenderSection_SetAmbient>::SendCommand(
		m_RenderSectionID,
		CRenderSection_SetAmbient( 0x404040 )
	);
	CTCommandSender<CRenderSection_SetDirectionalLight>::SendCommand(
		m_RenderSectionID,
		CRenderSection_SetDirectionalLight( CVector(-1.0f,-1.0f,-1.0f), 1.0f, 1.0f, 1.0f )
	);
	// set object positions
	m_M0.ConstructScaling( CVector(2,2,2) );
	m_M1.ConstructRotationY( 0 );
	m_M1 *= CMatrix().ConstructTranslation( CVector(4,0,0) );
	m_M2.ConstructRotationY( CONST_PI_2 );
	m_M2 *= CMatrix().ConstructTranslation( CVector(0,0,4) );
	m_M3.ConstructRotationY( CONST_PI_2*2 );
	m_M3 *= CMatrix().ConstructTranslation( CVector(-4,0,0) );
	m_M4.ConstructRotationY( CONST_PI_2*3 );
	m_M4 *= CMatrix().ConstructTranslation( CVector(0,0,-4) );
	// start rendering
	m_NFramesToRender = 2;
	RenderFrame();
	CTCommandSender<CTest5Section_Render>::SendCommand(
		GetThisID(),
		CTest5Section_Render()
	);
	CLog::Print("CTest5Section::Reaction( const CRenderSection_CreateVertexBuffer_Response& in_rCmd ) end\n");
}

void
CTest5Section::Reaction( long in_SrcSectionID, const CRenderSection_CloseRenderResult& in_rResult )
{
	CLog::Print("CTest5Section::Reaction( const CRenderSection_CloseRenderResult& in_rResult )\n");
	CLog::Print("  close %s\n",in_rResult.m_Ok?"SUCCESS":"FAIL");
	CLog::Print("CTest5Section::Reaction( const CRenderSection_CloseRenderResult& in_rResult ) end\n");
}

void
CTest5Section::Reaction( long in_SrcSectionID, const CRenderSection_NewFrame& in_rCmd )
{
//	CLog::Print("CTest5Section::Reaction( const CRenderSection_NewFrame& in_rCmd )\n");
	++m_NFramesToRender;
	++m_NFrames;
	if(m_HWnd!=NULL)
	{
		unsigned long ThisFrameTime = GetTickCount();
		if( (ThisFrameTime-m_LastTimeFrame)>=1000 )
		{
			float FPS = 1000.0f*(float(m_NFrames))/(float(ThisFrameTime-m_LastTimeFrame));
			char Buffer[256];
			sprintf(Buffer,"Render test. FPS=%1.0f",FPS);
			CTCommandSender<CWindowSection_WindowName>::SendCommand(
				m_WindowSectionID,
				CWindowSection_WindowName(std::string(Buffer))
			);
			//
			m_LastTimeFrame = ThisFrameTime;
			m_NFrames=0;
		}
		RenderFrame();
	}
//	CLog::Print("CTest5Section::Reaction( const CRenderSection_NewFrame& in_rCmd ) end\n");
}

// ===================================================================================================================

void
CTest5Section::Reaction( long in_SrcSectionID, const CTest5Section_Render& in_rCmd )
{
	//CLog::Print("CTest5Section::Reaction( const CTest5Section_Render& in_rCmd )\n");
	if(m_HWnd!=NULL)
	{
		RenderFrame();
	}
	//CLog::Print("CTest5Section::Reaction( const CTest5Section_Render& in_rCmd ) end\n");
}

// ===================================================================================================================

void
CTest5Section::RenderFrame()
{
	//CLog::Print("CTest5Section::RenderFrame()\n");
	assert(m_HWnd!=NULL);
	assert(m_RenderSectionID!=SECTION_ID_ERROR);
	if(m_NFramesToRender>0)
	{
		//CLog::Print("  m_NFramesToRender=%lu\n",m_NFramesToRender);
		{
			CVector CamOrg( 10.0f*sin(m_Alpha), 5.0f, 10.0f*cos(m_Alpha) );
			CVector CamDir( CVector(0,0,0) - CamOrg );
			CamDir.Normalize();
			CVector Tmp( CVector(0,10,0) - CamOrg );
			CVector CamRight( CamDir*Tmp );
			CamRight.Normalize();
			CTCommandSender<CRenderSection_SetCamera>::SendCommand(
				m_RenderSectionID,
				CRenderSection_SetCamera(	CamOrg,CamDir,CamRight,
											CONST_PI_2, 0.75f*CONST_PI_2,
											0.01f, 100.0f)
			);
			m_Alpha += 0.001f;
			if(m_Alpha>CONST_2PI)
				m_Alpha -= CONST_2PI;
		}
		{
//			CMatrix m;
//			m.ConstructScaling( CVector(1,1,1) );
			CTCommandSender<CRenderSection_RenderVB>::SendCommand(
				m_RenderSectionID,
				CRenderSection_RenderVB(m_M0,m_VBHandle,0,m_PrimitiveType,m_NPrimitives)
			);
			CTCommandSender<CRenderSection_RenderVB>::SendCommand(
				m_RenderSectionID,
				CRenderSection_RenderVB(m_M1,m_VBHandle,0,m_PrimitiveType,m_NPrimitives)
			);
			CTCommandSender<CRenderSection_RenderVB>::SendCommand(
				m_RenderSectionID,
				CRenderSection_RenderVB(m_M2,m_VBHandle,0,m_PrimitiveType,m_NPrimitives)
			);
			CTCommandSender<CRenderSection_RenderVB>::SendCommand(
				m_RenderSectionID,
				CRenderSection_RenderVB(m_M3,m_VBHandle,0,m_PrimitiveType,m_NPrimitives)
			);
			CTCommandSender<CRenderSection_RenderVB>::SendCommand(
				m_RenderSectionID,
				CRenderSection_RenderVB(m_M4,m_VBHandle,0,m_PrimitiveType,m_NPrimitives)
			);
		}
		//++m_Color;
		//if(m_Color>255)
		//	m_Color=0;
		//CLog::Print("  m_RenderSectionID=%ld\n",m_RenderSectionID);
		CTCommandSender<CRenderSection_PresentAndClear>::SendCommand(
			m_RenderSectionID,
			CRenderSection_PresentAndClear( 0 ) //CRenderSection_PresentAndClear( (m_Color<<16)|(m_Color<<8)|(m_Color) )
		);
		--m_NFramesToRender;
	}
}

// ===================================================================================================================



















/*

// ===================================================================================================================

void
CTest5Section::Reaction( long in_SrcSectionID, const CWindowSection_Activated& in_rCommand )
{
}

void
CTest5Section::Reaction( long in_SrcSectionID, const CWindowSection_WindowSize& in_rCommand )
{
}

// ===================================================================================================================

*/





















