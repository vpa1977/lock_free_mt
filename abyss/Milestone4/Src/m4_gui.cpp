#include "../Include/m4_gui.h"

#include <stdio.h>
#include <io.h>
#include <time.h>



void CGuiManager::InitWindows(long inSrcID, long inPreloaderID, long vfs)
{
	m_ThisID = inSrcID;
	m_PreloaderID = inPreloaderID;
	m_VFSID = vfs;

	m_bPlayRequested = false;
	m_bSeekRequested = false;
	m_bPlaying = false;
	

	WindowManager* pManager = WindowManager::getSingletonPtr();
	WindowManager& wmgr = *pManager;
	myRoot = wmgr.createWindow( "DefaultWindow", "root" );
	System::getSingleton().setGUISheet( myRoot );

    PushButton* btn = static_cast<PushButton*>(wmgr.createWindow("TaharezLook/Button", "OpenButton"));
    myRoot->addChildWindow(btn);
    btn->setText("Open Model");
    btn->setPosition(UVector2(cegui_reldim(0.035f), cegui_reldim( 0.0f)));
    btn->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.056f)));

	Event::Subscriber subscriber(&CGuiManager::handleOpen, this);

    btn->subscribeEvent(PushButton::EventClicked, subscriber);
    btn->setAlwaysOnTop(true);


	btn = static_cast<PushButton*>(wmgr.createWindow("TaharezLook/Button", "OpenAnimButton"));
    myRoot->addChildWindow(btn);
    btn->setText("Open Animation");
    btn->setPosition(UVector2(cegui_reldim(0.235f), cegui_reldim( 0.0f)));
    btn->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.056f)));
   btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CGuiManager::handleOpenAnimation, this));
    btn->setAlwaysOnTop(true);

	//// OPEN MODEL DIALOG INITIALIZATION
	{
		pModelLoadWindow = static_cast<FrameWindow*>(wmgr.createWindow("TaharezLook/FrameWindow", "OpenModelWindow"));
		pModelLoadWindow->setText("Open Model");
		pModelLoadWindow->setSizingEnabled(false);
		pModelLoadWindow->setCloseButtonEnabled(false);
		pModelLoadWindow->setPosition(UVector2(cegui_reldim(0.0f), cegui_reldim(  0.057f)));
		pModelLoadWindow->setSize(UVector2(cegui_reldim(0.5f), cegui_reldim(  0.5f)));

		pModelList = static_cast<Listbox*>(wmgr.createWindow("TaharezLook/Listbox", "OpenModelList"));		
		pModelList->setMultiselectEnabled(false);
		
		pModelList->setPosition(UVector2(cegui_reldim(0.0f), cegui_reldim( 0.1f)));
		pModelList->setSize(UVector2(cegui_reldim(0.9f), cegui_reldim( 0.8f)));
		pModelLoadWindow->addChildWindow(pModelList);
		pModelList->setAlpha(0.5f);
		PopulateModelList();
		
		btn = static_cast<PushButton*>(wmgr.createWindow("TaharezLook/Button", "OpenModelButton"));
		pModelLoadWindow->addChildWindow(btn);
		btn->setText("Open");
		btn->setPosition(UVector2(cegui_reldim(0.6f), cegui_reldim( 0.9f)));
		btn->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.1f)));
		btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CGuiManager::handleOpenModelButton, this));

		btn = static_cast<PushButton*>(wmgr.createWindow("TaharezLook/Button", "CancelModelButton"));
		pModelLoadWindow->addChildWindow(btn);
		btn->setText("Cancel");
		btn->setPosition(UVector2(cegui_reldim(0.8f), cegui_reldim( 0.9f)));
		btn->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.1f)));
		btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CGuiManager::handleOpen, this));
	}


	//// OPEN Animation DIALOG INITIALIZATION
	{
		pAnimationLoadWindow = static_cast<FrameWindow*>(wmgr.createWindow("TaharezLook/FrameWindow", "OpenAnimWindow"));
		pAnimationLoadWindow->setText("Open Model");
		pAnimationLoadWindow->setSizingEnabled(false);
		pAnimationLoadWindow->setCloseButtonEnabled(false);
		pAnimationLoadWindow->setPosition(UVector2(cegui_reldim(0.0f), cegui_reldim(  0.057f)));
		pAnimationLoadWindow->setSize(UVector2(cegui_reldim(0.5f), cegui_reldim(  0.5f)));

		pAnimationList = static_cast<Listbox*>(wmgr.createWindow("TaharezLook/Listbox", "OpenAnimList"));		
		pAnimationList->setMultiselectEnabled(false);
		
		pAnimationList->setPosition(UVector2(cegui_reldim(0.0f), cegui_reldim( 0.1f)));
		pAnimationList->setSize(UVector2(cegui_reldim(0.9f), cegui_reldim( 0.8f)));
		pAnimationLoadWindow->addChildWindow(pAnimationList);
		pAnimationList->setAlpha(0.5f);
		//PopulateModelList();
		
		btn = static_cast<PushButton*>(wmgr.createWindow("TaharezLook/Button", "OpenAnimationButton"));
		pAnimationLoadWindow->addChildWindow(btn);
		btn->setText("Open");
		btn->setPosition(UVector2(cegui_reldim(0.6f), cegui_reldim( 0.9f)));
		btn->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.1f)));
		btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CGuiManager::handleOpenModelAnimationButton, this));

		btn = static_cast<PushButton*>(wmgr.createWindow("TaharezLook/Button", "CancelAnimButton"));
		pAnimationLoadWindow->addChildWindow(btn);
		btn->setText("Cancel");
		btn->setPosition(UVector2(cegui_reldim(0.8f), cegui_reldim( 0.9f)));
		btn->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.1f)));
		btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CGuiManager::handleOpenAnimation, this));
	}


	m_Zslider= static_cast<Slider*>(wmgr.createWindow("TaharezLook/Slider", "ZSlider"));
	m_Zslider->setPosition(UVector2(cegui_reldim(0.96f), cegui_reldim( 0.0f)));
	m_Zslider->setSize(UVector2(cegui_reldim(0.02f), cegui_reldim( 1.0f)));
	m_Zslider->subscribeEvent(Slider::EventValueChanged, Event::Subscriber(&CGuiManager::handleZScroll, this));
	
	m_Zslider->setMaxValue(10.0f);
	m_Zslider->setCurrentValue(1.0f);
    m_Zslider->setClickStep(0.1f);
	myRoot->addChildWindow(m_Zslider);


	m_XRSlider= static_cast<Scrollbar*>(wmgr.createWindow("TaharezLook/HorizontalScrollbar", "m_XRSlider"));
	m_XRSlider->setPosition(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.72f)));
	m_XRSlider->setSize(UVector2(cegui_reldim(0.6f), cegui_reldim( 0.02f)));
	m_XRSlider->subscribeEvent(Scrollbar::EventScrollPositionChanged, Event::Subscriber(&CGuiManager::handleXRotate, this));
	
	m_XRSlider->setDocumentSize(360.0f);
	m_XRSlider->setScrollPosition(0);
	m_XRSlider->setPageSize(1);
	m_XRSlider->setStepSize(1);
	myRoot->addChildWindow(m_XRSlider);

	m_YRSlider= static_cast<Scrollbar*>(wmgr.createWindow("TaharezLook/HorizontalScrollbar", "m_YRSlider"));
	m_YRSlider->setPosition(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.76f)));
	m_YRSlider->setSize(UVector2(cegui_reldim(0.6f), cegui_reldim( 0.02f)));
	
	m_YRSlider->subscribeEvent(Scrollbar::EventScrollPositionChanged, Event::Subscriber(&CGuiManager::handleYRotate, this));
	m_YRSlider->setTooltipText("Y");
	m_YRSlider->setDocumentSize(360.0f);
	m_YRSlider->setScrollPosition(0);
	m_YRSlider->setPageSize(1);
	m_YRSlider->setStepSize(1);
	myRoot->addChildWindow(m_YRSlider);

	m_ZRSlider= static_cast<Scrollbar*>(wmgr.createWindow("TaharezLook/HorizontalScrollbar", "m_ZRSlider"));
	m_ZRSlider->setPosition(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.8f)));
	m_ZRSlider->setSize(UVector2(cegui_reldim(0.6f), cegui_reldim( 0.02f)));
	m_ZRSlider->subscribeEvent(Scrollbar::EventScrollPositionChanged, Event::Subscriber(&CGuiManager::handleZRotate, this));
	
	m_ZRSlider->setDocumentSize(360.0f);
	m_ZRSlider->setScrollPosition(0);
	m_ZRSlider->setPageSize(1);
	m_ZRSlider->setStepSize(1);
	myRoot->addChildWindow(m_ZRSlider);

	m_AnimationSlider = static_cast<Scrollbar*>(wmgr.createWindow("TaharezLook/HorizontalScrollbar", "m_AnimationSlider"));
	m_AnimationSlider->setPosition(UVector2(cegui_reldim(0.12f), cegui_reldim( 0.9f)));
	m_AnimationSlider->setSize(UVector2(cegui_reldim(0.8f), cegui_reldim( 0.02f)));
	m_AnimationSlider->setScrollPosition(0);
	m_AnimationSlider->setPageSize(1);
	m_AnimationSlider->setStepSize(1);


	m_AnimationSlider->subscribeEvent(Scrollbar::EventScrollPositionChanged, Event::Subscriber(&CGuiManager::handleAnimationMove, this));
	myRoot->addChildWindow(m_AnimationSlider);

	btn = static_cast<PushButton*>(wmgr.createWindow("TaharezLook/Button", "PlayAnimationButton"));
    myRoot->addChildWindow(btn);
    btn->setText("Play");
    btn->setPosition(UVector2(cegui_reldim(0.01f), cegui_reldim( 0.887f)));
    btn->setSize(UVector2(cegui_reldim(0.1f), cegui_reldim( 0.056f)));
    btn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CGuiManager::handlePlayAnimation, this));
    btn->setAlwaysOnTop(true);
	m_PlayButton = btn;


}
bool CGuiManager::handlePlayAnimation(const CEGUI::EventArgs& e)
{
	m_bPlaying = !m_bPlaying;
	if (m_bPlaying) 
		m_PlayButton->setText("Stop");
	else
		m_PlayButton->setText("Play");
	m_bPlayRequested = true;
	return false;
}

bool CGuiManager::handleAnimationMove(const CEGUI::EventArgs& e)
{
	m_CurrentFrame = m_XRSlider->getScrollPosition();
	if (!m_bPlaying) 
		m_bSeekRequested = true ;
	return false;
}


bool CGuiManager::handleXRotate(const CEGUI::EventArgs& e)
{
	m_XRotate = m_XRSlider->getScrollPosition();
	return false;
}
bool CGuiManager::handleYRotate(const CEGUI::EventArgs& e)
{
	m_YRotate = m_YRSlider->getScrollPosition();
	return false;
}
bool CGuiManager::handleZRotate(const CEGUI::EventArgs& e)
{
	m_ZRotate = m_ZRSlider->getScrollPosition();
	return false;
}


bool CGuiManager::handleZScroll(const CEGUI::EventArgs& e)
{
	m_Scale = m_Zslider->getCurrentValue();
	return false;
}


bool CGuiManager::handleOpenModelButton(const CEGUI::EventArgs& e)
{
	ListboxItem* item = pModelList->getFirstSelectedItem();
	if (!item ) return false;
	if (m_bModelWindowShown)
	{
		myRoot->removeChildWindow(pModelLoadWindow);
		m_bModelWindowShown = false;
	}
	const char* c = item->getText().c_str();
	std::string filename(c);
	
	CPreloaderSection_ModelLoadRequest req;
	req.m_FileName = filename;
	req.m_VFSSection = m_VFSID;
	m_ModelLoad.push_back(req);
//	CTCommandSender<CPreloaderSection_ModelLoadRequest>::SendCommand(m_PreloaderID, req);
	return false;
}
bool CGuiManager::handleOpenModelAnimationButton(const CEGUI::EventArgs& e)
{
	ListboxItem* item = pAnimationList->getFirstSelectedItem();
	if (!item ) return false;
	if (m_bAnimationWindowShown)
	{
		myRoot->removeChildWindow(pAnimationLoadWindow);
		m_bAnimationWindowShown = false;
	}
	const char* c = item->getText().c_str();
	std::string filename(c);
	
	CPreloaderSection_MatrixLoadRequest req;
	req.m_FileName = filename;
	req.m_VFSSection = m_VFSID;
	req.m_Magic = 999;
	m_AnimationLoad.push_back(req);
//	CTCommandSender<CPreloaderSection_MatrixLoadRequest>::SendCommand(m_PreloaderID, req);
	return false;
}


bool CGuiManager::handleOpenAnimation(const CEGUI::EventArgs& e)
{
	if (m_bAnimationWindowShown) 
	{
		myRoot->removeChildWindow(pAnimationLoadWindow);
		m_bAnimationWindowShown = false;
	}
	else
	{
		PopulateAnimationList();
		myRoot->addChildWindow(pAnimationLoadWindow);
		m_bAnimationWindowShown = true;
	}
	return false;
}

void CGuiManager::PopulateAnimationList()
{
	
	for (int i = 0 ; i < m_AnimListElements.size() ; i ++ )
	{
		pAnimationList->removeItem(m_AnimListElements[i]);
	}
	m_AnimListElements.clear();
	struct _finddata_t c_file;
    long hFile;

    /* Find first .c file in current directory */
    if( (hFile = _findfirst( "*.anim", &c_file )) == -1L )
       return;
   else
   {
	   std::string filename(c_file.name);
	   int pos = filename.rfind(".geom");
	   filename  = filename.substr(0, pos);

	   ListboxTextItem* pItem = new ListboxTextItem(String(filename.c_str()));
	  
		colour red(1.0, 0.0, 0.0, 0.5);
		pItem->setSelectionColours(red, red, red, red);
		// this default image is a simple white rectangle
		pItem->setSelectionBrushImage("TaharezLook", "ListboxSelectionBrush");

	   
	   pAnimationList->addItem(pItem);
	   pAnimationList->setItemSelectState(pItem,true);
	   m_AnimListElements.push_back(pItem);
	   pAnimationList->setEnabled(true);
	   
        /* Find the rest of the .c files */
        while( _findnext( hFile, &c_file ) == 0 )
        {
			filename = c_file.name;
			int pos = filename.rfind(".geom");
			filename  = filename.substr(0, pos);

			ListboxTextItem* pItem = new ListboxTextItem(String(filename.c_str()));
			pItem->setSelectionColours(red, red, red, red);
		// this default image is a simple white rectangle
		pItem->setSelectionBrushImage("TaharezLook", "ListboxSelectionBrush");

			pAnimationList->addItem(pItem);
			
			m_AnimListElements.push_back(pItem);
		}

       _findclose( hFile );
   }
   pAnimationList->setMultiselectEnabled(false);

	
}


void CGuiManager::PopulateModelList()
{
	
	for (int i = 0 ; i < m_ModelListElements.size() ; i ++ )
	{
		pModelList->removeItem(m_ModelListElements[i]);
	}
	m_ModelListElements.clear();
	struct _finddata_t c_file;
    long hFile;

    /* Find first .c file in current directory */
    if( (hFile = _findfirst( "*.geom", &c_file )) == -1L )
       return;
   else
   {
	   std::string filename(c_file.name);
	   int pos = filename.rfind(".geom");
	   filename  = filename.substr(0, pos);

	   ListboxTextItem* pItem = new ListboxTextItem(String(filename.c_str()));
	  
		colour red(1.0, 0.0, 0.0, 0.5);
		pItem->setSelectionColours(red, red, red, red);
		// this default image is a simple white rectangle
		pItem->setSelectionBrushImage("TaharezLook", "ListboxSelectionBrush");

	   
	   pModelList->addItem(pItem);
	   pModelList->setItemSelectState(pItem,true);
	   m_ModelListElements.push_back(pItem);
	   pModelList->setEnabled(true);
	   
        /* Find the rest of the .c files */
        while( _findnext( hFile, &c_file ) == 0 )
        {
			filename = c_file.name;
			int pos = filename.rfind(".geom");
			filename  = filename.substr(0, pos);

			ListboxTextItem* pItem = new ListboxTextItem(String(filename.c_str()));
			pItem->setSelectionColours(red, red, red, red);
		// this default image is a simple white rectangle
		pItem->setSelectionBrushImage("TaharezLook", "ListboxSelectionBrush");

			pModelList->addItem(pItem);
			
			m_ModelListElements.push_back(pItem);
		}

       _findclose( hFile );
   }
   pModelList->setMultiselectEnabled(false);

	
}
bool CGuiManager::handleOpen(const CEGUI::EventArgs& e ) 
{
	if (m_bModelWindowShown) 
	{
		myRoot->removeChildWindow(pModelLoadWindow);
		m_bModelWindowShown = false;
	}
	else
	{
		PopulateModelList();
		myRoot->addChildWindow(pModelLoadWindow);
		m_bModelWindowShown = true;
	}
	return false;
}