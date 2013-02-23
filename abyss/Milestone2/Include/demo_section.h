#ifndef DEMO_SECTION_HPP_INCLUDED
#define DEMO_SECTION_HPP_INCLUDED

#define _STLP_VERBOSE_AUTO_LINK 1

#include "../../Game_Base/Include/gamebase_section.h"
#include "demo_section_interface.h"

#include <vector>
#include <map>

class CDemoSection :
	public CGameBaseSection
	// system
	//,public CTCommandSender< CCommand_QuitApplication >
	,public CTCommandSender< CCommand_QuitSystem >
	,public CTCommandSender<CCommand_SetTimer_Request>
	// vfs
	,public CTCommandSender< CVFSSection_MountRequest >
	,public CTCommandSender< CVFSSection_UMountRequest >
	,public CTCommandSender< CVFSSection_ReadRequest >
	,public CTCommandSender< CVFSSection_OpenRequest >
	,public CTCommandSender< CVFSSection_CloseRequest >
	// window
	,public CTCommandSender< CWindowSection_WindowName >
	// render
	,public CTCommandSender< CRenderSection_PresentAndClear >
	,public CTCommandSender< CRenderSection_SetCamera >
	,public CTCommandSender< CRenderSection_SetAmbient >
	,public CTCommandSender< CRenderSection_SetDirectionalLight >
	,public CTCommandSender< CRenderSection_SetPointLight >
	,public CTCommandSender< CRenderSection_RenderVB >
	,public CTCommandSender< CRenderSection_RenderString >
	,public CTCommandSender< CRenderSection_RenderDot >
	// self
	,public CTCommandSender< CDemoSection_WorkerInit_Request >
	,public CTCommandSender< CDemoSection_WorkerCameraInform >
	,public CTCommandSender< CDemoSection_StartRender >
	,public CTCommandSender< CDemoSection_WorkerShutdown_Request >
{
public:
	CDemoSection( long in_SectionID );
	~CDemoSection();
	std::string GetName()
	{
		return "CDemoSection( Milestone 2 )";
	}

	// system
	void Reaction( long in_SrcSectionID, const CCommand_SetTimer_Response& );
	void Reaction( long in_SrcSectionID, const CCommand_Timer_Event& );
	// vfs
	void Reaction( long in_SrcSectionID, const CVFSSection_MountResponse& in_rMountResponse);
	void Reaction( long in_SrcSectionID, const CVFSSection_UMountResponse& in_rUMountResponse);
	void Reaction( long in_SrcSectionID, const CVFSSection_ReadResponse& in_rReadResponse);
	void Reaction( long in_SrcSectionID, const CVFSSection_OpenResponse& in_rOpenResponse);
	void Reaction( long in_SrcSectionID, const CVFSSection_CloseResponse& in_rCloseResponse);
	// window
	// render
	void Reaction( long in_SrcSectionID, const CRenderSection_NewFrame& in_rCmd );
	// self
	void Reaction( long in_SrcSectionID, const CDemoSection_WorkerInit_Response& in_rResponse);
	void Reaction( long in_SrcSectionID, const CDemoSection_WorkerUpdate& in_rUpdate);
	void Reaction( long in_SrcSectionID, const CDemoSection_WorkerCameraInform_Response& in_rResponse);
	void Reaction( long in_SrcSectionID, const CDemoSection_StartRender& in_rCmd);
	void Reaction( long in_SrcSectionID, const CDemoSection_WorkerShutdown_Response& in_rResponse);

	void Reaction( long in_SrcSectionID, const CInputConsoleSection_KeyboardUpdate& update );
	void Reaction(long in_SrcSectionID , const CInputConsoleSection_MouseUpdate& update ) ;

protected:
	void Received_MTInfo(const CCommand_MTInfo&);
	long GetNThreadsToRun();
	bool GetNeedVFS();
	bool GetNeedPreloader();
	void Received_SectionLaunchResult(const CCommand_LaunchSectionResult&);
	void GetGameWindowParams( CWindowParam& );
	void Received_3DInfo( const CRenderSection_3DEnvironmentInfo& );
	void GetRenderInitParams( CRenderSection_InitRender& );
	void Received_InitRenderResult( INIT_RENDER_RESULT in_bResult );
	void Received_WindowClosed();
	void StartTheGame();
private:
	long m_NThreadsToRun;
	CRenderSection_3DEnvironmentInfo m_3DInfo;

	long m_ConfigFileHandle;
	std::vector<unsigned char> m_ConfigData;
	enum E_TYPE
	{
		ET_UNKNOWN,
		ET_STARS,
		ET_SUN,
		ET_PLANET
	};
	struct CConfigInfo
	{
		CConfigInfo( E_TYPE in_Type=ET_UNKNOWN, const std::string& in_FileName=std::string("") ) : m_Type(in_Type), m_FileName(in_FileName) {}
		E_TYPE m_Type;
		std::string m_FileName;
	};
	std::vector< CConfigInfo > m_ConfigFileNames;
	void ParseConfig();

	enum E_INIT_STAGE
	{
		EIS_BEFORE_WORKERS_LAUNCH	= 0,
		EIS_AWAITING_WORKERS_LAUNCH	= 1,
		EIS_AWAITING_WORKERS_INIT	= 2,
		EIS_DONE					= 3,
		EIS_SHUTDOWN				= 4
	};
	E_INIT_STAGE m_InitStage;

	struct CObjectSectionInfo
	{
		CObjectSectionInfo( long in_Magic=0, long in_SectionID=SECTION_ID_ERROR ) : m_Magic(in_Magic), m_SectionID(in_SectionID), m_bAlive(true) {}
		long m_Magic;
		long m_SectionID;
		bool m_bAlive;
	};
	std::vector<CObjectSectionInfo> m_ObjectSectionsInfo;
	unsigned long m_NPendingObjectSections;

	struct CRenderable
	{
		CRenderable(	unsigned long in_VBHandle=0,
						unsigned long in_PSHandle=0,
						unsigned long in_PrimitiveType=PRIM_TRIANGLE_LIST,
						unsigned long in_PrimitiveCount=0,
						CMatrix in_Position=CMatrix(),
						OBJECT_TYPE in_Type=OT_SOLID
		) :
			m_VBHandle(in_VBHandle)
			,m_PSHandle(in_PSHandle)
			,m_PrimitiveType(in_PrimitiveType)
			,m_PrimitiveCount(in_PrimitiveCount)
			,m_Position(in_Position)
			,m_Type(in_Type)
		{}
		unsigned long m_VBHandle;
		unsigned long m_PSHandle;
		unsigned long m_PrimitiveType;
		unsigned long m_PrimitiveCount;
		CMatrix       m_Position;
		OBJECT_TYPE   m_Type;
	};
	// key - sectionID, value - array of renderables for given section
	typedef std::map< long, std::vector<CRenderable> > CStoredInfo;
	CStoredInfo m_StoredInfo;

	void SendWorkerInitCommands();

	// camera-related
	long m_SunSectionID;
	CVector m_Up;
	CVector m_Right;
	unsigned long m_LastCameraUpdateTime;

	unsigned long m_NFramesToRender;
	unsigned long m_FirstTimeFrame;
	unsigned long m_LastTimeFrame;
	unsigned long m_NFrames;
	void RenderFrame();


	char m_pChars[1024];
	int m_pCharPointer;
};

#endif








