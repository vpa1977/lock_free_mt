#include "../Include/demo_section.h"
#include "../../MT_Core/Include/section_registrator.h"
//#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/log.h"

static CTSectionRegistrator<DemoSection_CLSID,CDemoSection>
g_DemoSectionRegistrator( SINGLE_INSTANCE_ONLY, STARTUP_SECTION );

CDemoSection::CDemoSection( long in_SectionID ) :
	CGameBaseSection( in_SectionID )
	,m_NThreadsToRun(0)
	,m_InitStage(EIS_BEFORE_WORKERS_LAUNCH)
	,m_ConfigFileHandle(0)
	,m_NPendingObjectSections(0)
	,m_SunSectionID(SECTION_ID_ERROR)
	,m_LastCameraUpdateTime(0)
	,m_NFramesToRender(0)
	,m_FirstTimeFrame(0)
	,m_LastTimeFrame(0)
	,m_NFrames(0)
	,m_pCharPointer(0)
{
	memset(m_pChars,0,1024);
	// system
	RegisterReaction( GetID((CCommand_SetTimer_Response*)NULL),	CTConform<CDemoSection,CCommand_SetTimer_Response>::Conform );
	RegisterReaction( GetID((CCommand_Timer_Event*)NULL),	CTConform<CDemoSection,CCommand_Timer_Event>::Conform );
	// vfs
	RegisterReaction( GetID((CVFSSection_MountResponse*)NULL),	CTConform<CDemoSection,CVFSSection_MountResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_UMountResponse*)NULL),	CTConform<CDemoSection,CVFSSection_UMountResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_ReadResponse*)NULL),	CTConform<CDemoSection,CVFSSection_ReadResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_CloseResponse*)NULL),	CTConform<CDemoSection,CVFSSection_CloseResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_OpenResponse*)NULL),	CTConform<CDemoSection,CVFSSection_OpenResponse>::Conform );
	// window
	// render
	RegisterReaction( GetID((CRenderSection_NewFrame*)NULL),	CTConform<CDemoSection,CRenderSection_NewFrame>::Conform );
	// self
	RegisterReaction( GetID((CDemoSection_WorkerInit_Response*)NULL),		CTConform<CDemoSection,CDemoSection_WorkerInit_Response>::Conform );
	RegisterReaction( GetID((CDemoSection_WorkerUpdate*)NULL),				CTConform<CDemoSection,CDemoSection_WorkerUpdate>::Conform );
	RegisterReaction( GetID((CDemoSection_WorkerCameraInform_Response*)NULL),	CTConform<CDemoSection,CDemoSection_WorkerCameraInform_Response>::Conform );
	RegisterReaction( GetID((CDemoSection_StartRender*)NULL),				CTConform<CDemoSection,CDemoSection_StartRender>::Conform );
	RegisterReaction( GetID((CDemoSection_WorkerShutdown_Response*)NULL),	CTConform<CDemoSection,CDemoSection_WorkerShutdown_Response>::Conform );


	RegisterReaction( GetID((CInputConsoleSection_KeyboardUpdate*)NULL),	CTConform<CDemoSection,CInputConsoleSection_KeyboardUpdate>::Conform );
	RegisterReaction( GetID((CInputConsoleSection_MouseUpdate*)NULL),	CTConform<CDemoSection,CInputConsoleSection_MouseUpdate>::Conform );
}

CDemoSection::~CDemoSection()
{
}





















