#include "../include/hamsters.h"
#include "../Include/hamster_section.h"
#include "../Include/hamster_section_interface.h"
#include "../include/main_menu.h"
#include "../include/game_section.h"
// } TEST

static CTSectionRegistrator<M4Section_CLSID,CHamsterSection>
g_DemoSectionRegistrator( SINGLE_INSTANCE_ONLY, STARTUP_SECTION );

extern void Touch_CEGUI();

CHamsterSection::CHamsterSection( long in_SectionID ) :
	CSectionController( in_SectionID )
	,m_NThreadsToRun(0)
	,m_bTransitionImageReady(false)
{
	Touch_CEGUI();
	REGISTER_REACTION(CHamsterSection,CHamster_InitMainMenuRequest);
	REGISTER_REACTION(CHamsterSection,CHamster_HideMainMenuRequest);
	REGISTER_REACTION(CHamsterSection,CInputConsoleSection_MouseUpdate);
	RegisterSubSection(1, new CMainMenuSection(0));	
	RegisterSubSection(2, new CGameSection(0));
}

CHamsterSection::~CHamsterSection()
{
}



void CHamsterSection::Reaction(long srcSection, const CHamster_InitMainMenuRequest& in)
{
	((CHamsterGUI*)m_pProxy)->InitMainMenu(this);
	CTCommandSender<CHamster_InitMainMenuResponse>::SendCommand(srcSection, CHamster_InitMainMenuResponse());

}

void CHamsterSection::Reaction(long srcSection, const CHamster_HideMainMenuRequest& in)
{
	((CHamsterGUI*)m_pProxy)->HideMainMenu();
	CTCommandSender<CHamster_HideMainMenuResponse>::SendCommand(srcSection, CHamster_HideMainMenuResponse());

}

bool CHamsterSection::NewGame(const CEGUI::EventArgs& e)
{
	return true;
}




















