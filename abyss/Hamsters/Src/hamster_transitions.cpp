#include "../include/hamsters.h"
#include "../include/hamster_section.h"
#include "../include/main_menu.h"
#include "../include/game_section.h"


void CHamsterSection::InitStates(CStateConfiguration& in)
{
	in.AddTransition("", TRANSITION_START_GAME, "MAINMENU");
	in.AddTransition("MAINMENU", "GAME", "GAME");
	in.m_StartState = "MAINMENU";
	{
		section* ptr =(section*) GetSubSections().find(1)->second;
		((CMainMenuSection*)ptr)->SetRenderSectionID(GetRenderSectionID());
	}
	{
		section* ptr =(section*) GetSubSections().find(2)->second;
		((CGameSection*)ptr)->SetRenderSectionID(GetRenderSectionID());
		((CGameSection*)ptr)->SetPreloaderID( GetPreloaderSectionID());
		((CGameSection*)ptr)->SetVFSID( GetVFSSectionID());
	}

}



void CHamsterSection::PrepareTransition(const std::string& transition)
{
	if (!m_bTransitionImageReady) 
	{
	//	AddTransitionImage();
		m_bTransitionImageReady = true;
	}
	TransitionUpdate();
}

void CHamsterSection::ReleaseTransition(const std::string& transition)
{
	m_bTransitionImageReady = false;
	//ReleaseTransitionImage();

}


void CHamsterSection::AddTransitionImage()
{
	WindowManager* pManager = WindowManager::getSingletonPtr();
	WindowManager& wmgr = *pManager;
	Window* myRoot = wmgr.createWindow( "DefaultWindow", "_TransitionRoot" );
	System::getSingleton().setGUISheet( myRoot );
	
    PushButton* btn = static_cast<PushButton*>(wmgr.createWindow("TaharezLook/Button", "_TransitionBackground"));
	btn->setSize(UVector2(cegui_reldim(1.0f),cegui_reldim(1.0f)));
	btn->setPosition(UVector2(cegui_reldim(0.0f),cegui_reldim(0.0f)));
	btn->setText("LOADING");
	btn->setVisible(true);
    myRoot->addChildWindow(btn);


}

void CHamsterSection::ReleaseTransitionImage()
{
	WindowManager* pManager = WindowManager::getSingletonPtr();
	WindowManager& wmgr = *pManager;
	//System::getSingleton().setGUISheet(NULL);
	wmgr.destroyWindow("_TransitionBackground");
	wmgr.destroyWindow("_TransitionRoot");

}

