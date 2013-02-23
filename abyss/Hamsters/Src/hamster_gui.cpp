#include "../include/hamsters.h"
#include "../Include/hamster_gui.h"
#include "../include/hamster_section.h"

void CHamsterGUI::InitMainMenu(void* ptr)
{
    ImagesetManager::getSingleton().createImagesetFromImageFile("BackgroundImage", "data/GPN-2000-001437.tga");
	
	WindowManager* pManager = WindowManager::getSingletonPtr();
	WindowManager& winMgr = *pManager;

    // here we will use a StaticImage as the root, then we can use it to place a background image
    Window* background = winMgr.createWindow("TaharezLook/StaticImage", "background_wnd");
    // set position and size
    background->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0)));
    background->setSize(UVector2(cegui_reldim(1), cegui_reldim( 1)));
    // disable frame and standard background
    background->setProperty("FrameEnabled", "false");
    background->setProperty("BackgroundEnabled", "false");
    // set the background image
    background->setProperty("Image", "set:BackgroundImage image:full_image");
    // install this as the root GUI sheet
    //System::getSingleton().setGUISheet(background);

    // load the windows for Demo7 from the layout file.
    Window* sheet = winMgr.loadWindowLayout("data/main_menu.layout");
    // attach this to the 'real' root
    background->addChildWindow(sheet);
	System::getSingleton().setGUISheet(background);

	/// add handlers
	
   WindowManager::getSingleton().getWindow("MainMenu/NewGame")->
        subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CHamsterGUI::handleNewGame, this));
}

void CHamsterGUI::HideMainMenu()
{
	WindowManager* pManager = WindowManager::getSingletonPtr();
	WindowManager& winMgr = *pManager;
	System::getSingleton().setGUISheet(NULL);
	winMgr.destroyAllWindows();
	ImagesetManager::getSingleton().destroyImageset("BackgroundImage");


}

bool CHamsterGUI::handleNewGame(const CEGUI::EventArgs& e)
{
	m_bNewGame = true;
	return true;
}