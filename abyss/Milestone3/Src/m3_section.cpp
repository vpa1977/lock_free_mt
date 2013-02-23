#include "../Include/m3_section.h"
#include "../../MT_Core/Include/section_registrator.h"
#include "../../MT_Core/Include/log.h"

#include "../Include/intersect.h"

static CTSectionRegistrator<M3Section_CLSID,CM3Section>
g_DemoSectionRegistrator( SINGLE_INSTANCE_ONLY, STARTUP_SECTION );

CRenderProxy* CRenderProxy::m_pInstance = NULL;
CLoadProxy* CLoadProxy::m_pInstance = NULL;

CM3Section::CM3Section( long in_SectionID ) :
	CGameBaseSection( in_SectionID )
	,m_NThreadsToRun(0)
	,m_NFramesToRender(0)
	,m_FirstTimeFrame(0)
	,m_LastTimeFrame(0)
	,m_NFrames(0) 
	,m_pWorld(NULL)
	,m_bPreloadSuccessfull(false)
	,m_bMouseButtonArmed(false)
{
	CRenderProxy::setInstance(this);
	CLoadProxy::setInstance(this);
	// system
	RegisterReaction( GetID((CCommand_SetTimer_Response*)NULL),	CTConform<CM3Section,CCommand_SetTimer_Response>::Conform );
	RegisterReaction( GetID((CCommand_Timer_Event*)NULL),	CTConform<CM3Section,CCommand_Timer_Event>::Conform );
	// vfs
	RegisterReaction( GetID((CVFSSection_MountResponse*)NULL),	CTConform<CM3Section,CVFSSection_MountResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_UMountResponse*)NULL),	CTConform<CM3Section,CVFSSection_UMountResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_ReadResponse*)NULL),	CTConform<CM3Section,CVFSSection_ReadResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_CloseResponse*)NULL),	CTConform<CM3Section,CVFSSection_CloseResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_OpenResponse*)NULL),	CTConform<CM3Section,CVFSSection_OpenResponse>::Conform );
	// vfs errors
	RegisterReaction( GetID((CVFSSection_Error<CVFSSection_MountResponse>*)NULL),	CTConform<CM3Section,CVFSSection_Error<CVFSSection_MountResponse> >::Conform );
	RegisterReaction( GetID((CVFSSection_Error<CVFSSection_UMountResponse>*)NULL),	CTConform<CM3Section,CVFSSection_Error<CVFSSection_UMountResponse> >::Conform );
	RegisterReaction( GetID((CVFSSection_Error<CVFSSection_ReadResponse>*)NULL),	CTConform<CM3Section,CVFSSection_Error<CVFSSection_ReadResponse> >::Conform );
	RegisterReaction( GetID((CVFSSection_Error<CVFSSection_OpenResponse>*)NULL),	CTConform<CM3Section,CVFSSection_Error<CVFSSection_OpenResponse> >::Conform );
	RegisterReaction( GetID((CVFSSection_Error<CVFSSection_CloseResponse>*)NULL),	CTConform<CM3Section,CVFSSection_Error<CVFSSection_CloseResponse> >::Conform );
	// preloader
	RegisterReaction( GetID((CPreloaderSection_DecodeError*)NULL),	CTConform<CM3Section,CPreloaderSection_DecodeError>::Conform );
	RegisterReaction( GetID((CPreloaderSection_DecodeResponse*)NULL),	CTConform<CM3Section,CPreloaderSection_DecodeResponse>::Conform );
	// window
	// render
	RegisterReaction( GetID((CRenderSection_NewFrame*)NULL),	CTConform<CM3Section,CRenderSection_NewFrame>::Conform );
	RegisterReaction( GetID((CRenderSection_CreateVertexBuffer_Response*)NULL),	CTConform<CM3Section,CRenderSection_CreateVertexBuffer_Response>::Conform );
	RegisterReaction( GetID((CRenderSection_UpdateVertexBuffer_Response*)NULL),	CTConform<CM3Section,CRenderSection_UpdateVertexBuffer_Response>::Conform );
	RegisterReaction( GetID((CRenderSection_CreateTexture_Response*)NULL),	CTConform<CM3Section,CRenderSection_CreateTexture_Response>::Conform );
	RegisterReaction( GetID((CRenderSection_CreatePixelShader_Response*)NULL),	CTConform<CM3Section,CRenderSection_CreatePixelShader_Response>::Conform );
	RegisterReaction( GetID((CRenderSection_CreateFont_Response*)NULL),	CTConform<CM3Section,CRenderSection_CreateFont_Response>::Conform );
	// self
	RegisterReaction( GetID((CM3Section_WaitingForPreloadToComplete*)NULL),	CTConform<CM3Section,CM3Section_WaitingForPreloadToComplete>::Conform );
	RegisterReaction( GetID((CM3Section_StartRender*)NULL),	CTConform<CM3Section,CM3Section_StartRender>::Conform );
	// input
	RegisterReaction( GetID((CInputConsoleSection_KeyboardUpdate*)NULL),	CTConform<CM3Section,CInputConsoleSection_KeyboardUpdate>::Conform );
	RegisterReaction( GetID((CInputConsoleSection_MouseUpdate*)NULL),	CTConform<CM3Section,CInputConsoleSection_MouseUpdate>::Conform );
}

CM3Section::~CM3Section()
{
}





















