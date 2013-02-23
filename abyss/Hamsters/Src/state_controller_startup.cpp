#include "../Include/hamsters.h"
#include "../include/state_controller_section.h"


using namespace CEGUI;


#define CEGUI_LAUNCH 999999123



void
CSectionController::StartTheGame()
{

	RTASSERT( SECTION_ID_ERROR != GetPreloaderSectionID() );
	RTASSERT( SECTION_ID_ERROR != GetVFSSectionID() );
	RTASSERT( SECTION_ID_ERROR != GetWindowSectionID() );
	RTASSERT( SECTION_ID_ERROR != GetRenderSectionID() );

	// TODO: perform config preloading/parsing here
	CVFSSection_MountRequest req;
	req.m_Magic = 1;
	req.m_pRoot = ".";
	CTCommandSender<CVFSSection_MountRequest>::SendCommand(GetVFSSectionID() , req);

}

void CSectionController::Reaction(long in_SrcSectionID, const CEGUI_Render_InitResponse& in ) 
{
	Rect r;
	r.d_left = 0;
	r.d_right = TEST_WINDOW_WIDTH;
	r.d_top = 0;
	r.d_bottom = TEST_WINDOW_HEIGHT;
	m_pProxy = InitProxy();
	
	m_pProxy->SetRenderArea(r);
	//// Init some CEGUI stuff
	new CEGUI::System( m_pProxy );
	System::getSingleton().getRenderer()->setQueueingEnabled(true);
	CEGUI::SchemeManager::getSingleton().loadScheme( "TaharezLook.scheme" );

	// load in a font.  The first font loaded automatically becomes the default font.
	if(! CEGUI::FontManager::getSingleton().isFontPresent( "Commonwealth-10" ) )
	CEGUI::FontManager::getSingleton().createFont( "Commonwealth-10.font" );
	//System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );
	
	RunFirstState();
	//m_pManager->InitWindows(GetThisID(), GetPreloaderSectionID(), GetVFSSectionID());
	// start main loop
	CTCommandSender<CRenderSection_PresentAndClear>::SendCommand(GetRenderSectionID() , CRenderSection_PresentAndClear());
//	OneFrame();
}


void CSectionController::Received_SectionLaunchResult(const CCommand_LaunchSectionResult& res)
{
	if (res.m_Magic == CEGUI_LAUNCH) {
		assert( res.m_ErrorCode == SLR_OK );
		assert( res.m_NewSectionID != SECTION_ID_ERROR );

		CEGUI_Render_InitRequest req;
		req.m_SrcID = GetThisID();
		req.m_VFSSectionID = GetVFSSectionID();
		req.m_PreloaderSectionID = GetPreloaderSectionID();
		req.m_RenderSectionID = GetRenderSectionID();
		m_ProxySectionID =  res.m_NewSectionID;
		CTCommandSender< CEGUI_Render_InitRequest >::SendCommand( res.m_NewSectionID , req);
		return;
	}

};


void CSectionController::Reaction( long in_SrcSectionID, const CVFSSection_MountResponse& in_rMountResponse)
{
		CTCommandSender< CCommand_LaunchSection >::SendCommand(
				SECTION_ID_SYSTEM,
				CCommand_LaunchSection( CCEGUI_RenderSection_CLSID, CEGUI_LAUNCH )
		);

}

void CSectionController::Reaction( long in_SrcSectionID, const CState_InitResponse& in_rResponse)
{
}




