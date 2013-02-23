#include "../Include/m4_section.h"
#include "../../MT_Core/Include/log.h"
#include "../../MT_Core/Include/rtassert.h"
#include <assert.h>

#include "CEGUI.h"
#include "CEGUIBase.h"
#include "CEGUIRenderer.h"
#include "CEGUITexture.h"

using namespace CEGUI;


#define CEGUI_LAUNCH 999999123



void
CM4Section::StartTheGame()
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

void CM4Section::Reaction(long in_SrcSectionID, const CEGUI_Render_InitResponse& in ) 
{
	Rect r;
	r.d_left = 0;
	r.d_right = TEST_WINDOW_WIDTH;
	r.d_top = 0;
	r.d_bottom = TEST_WINDOW_HEIGHT;
	
	m_pProxy = new CObjectRendererProxy();
	m_pProxy->SetRenderArea(r);
	//// Init some CEGUI stuff
	new CEGUI::System( m_pProxy );
	System::getSingleton().getRenderer()->setQueueingEnabled(true);
	CEGUI::SchemeManager::getSingleton().loadScheme( "TaharezLook.scheme" );

	// load in a font.  The first font loaded automatically becomes the default font.
	if(! CEGUI::FontManager::getSingleton().isFontPresent( "Commonwealth-10" ) )
	CEGUI::FontManager::getSingleton().createFont( "Commonwealth-10.font" );
	//System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );

	m_pManager->InitWindows(GetThisID(), GetPreloaderSectionID(), GetVFSSectionID());
	// start main loop
	OneFrame();
	OneFrame();
}


void CM4Section::Received_SectionLaunchResult(const CCommand_LaunchSectionResult& res)
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


void CM4Section::Reaction( long in_SrcSectionID, const CVFSSection_MountResponse& in_rMountResponse)
{
		CTCommandSender< CCommand_LaunchSection >::SendCommand(
				SECTION_ID_SYSTEM,
				CCommand_LaunchSection( CCEGUI_RenderSection_CLSID, CEGUI_LAUNCH )
		);

/*	long magic = 1;
	

	CPreloaderSection_ModelLoadRequest req;
	req.m_Magic = 1;
	req.m_VFSSection = GetVFSSectionID();
	req.m_FileName = "test";

	CTCommandSender<CPreloaderSection_ModelLoadRequest>::SendCommand( GetPreloaderSectionID(), req);*/
}
void CM4Section::Reaction(long inSrcSectionID, const CPreloaderSection_MatrixLoadResponse& resp)
{
	m_pController = resp.m_pController;
	if (m_pO && m_pManager->IsRunning() ) 
	{
		m_pManager->setMaxFrame(m_pController->GetFrameCount());
		m_pO->AddController(m_pController);
		//m_pController->Play(0,true);
	}
/*	m_NFramesToRender = 2;
	m_FirstTimeFrame = GetTickCount();
	m_fScale = 0.001;
	m_ObjectTransform  =  CMatrix().ConstructScaling(CVector(m_fScale, m_fScale, m_fScale));;
	OneFrame();
	OneFrame();*/

}

void CM4Section::Reaction(long inSrcSectionID, const CPreloaderSection_ModelLoadResponse& resp)
{
	m_pLoadedObject = resp.m_pObject;
	RTASSERT( GetHWND()!=NULL );

/*	CPreloaderSection_MatrixLoadRequest req;
	req.m_Magic = 2;
	req.m_VFSSection = GetVFSSectionID();
	req.m_FileName = "test.matrix";

	CTCommandSender<CPreloaderSection_MatrixLoadRequest>::SendCommand( GetPreloaderSectionID(), req);
*/

}

void CM4Section::Reaction(long inSrcSectionID, const CPreloaderSection_DecodeError& resp)
{
	m_pManager->ShowError();
}

