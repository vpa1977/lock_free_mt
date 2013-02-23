#ifndef M4_GUI_HPP

#define M4_GUI_HPP

#include "CEGUI.h"

#include "../../MT_Core/Include/log.h"
#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../MT_Core/Include/command_sender.h"
#include "../../Preloader/Include/section_preloader_interface.h"

#include<vector>

using namespace CEGUI;

class CGuiManager 
	
/*	:
	public CSubSection , 
	CTCommandSender<CPreloaderSection_ModelLoadRequest>,
	CTCommandSender<CPreloaderSection_MatrixLoadRequest>
*/

{

public:
	CGuiManager() 
	{
		m_bModelWindowShown = false;
		m_bAnimationWindowShown = false;
		m_Scale = 1;
		m_XRotate = 0;
		m_YRotate = 0;
		m_ZRotate = 0;
		m_ThisID = 0;
	}

public:
	std::vector<CPreloaderSection_MatrixLoadRequest> m_AnimationLoad;
	std::vector<CPreloaderSection_ModelLoadRequest>	 m_ModelLoad;

	void InitWindows(long inSrcID, long inPreloaderID, long inVFSID);
	//{
	//}

	void ShowError() {};

			
	bool handleOpen(const CEGUI::EventArgs& e);
	bool handleOpenAnimation(const CEGUI::EventArgs& e);

	bool handleOpenModelButton(const CEGUI::EventArgs& e);
	bool handleOpenModelAnimationButton(const CEGUI::EventArgs& e);

	bool handleZScroll(const CEGUI::EventArgs& e);
	
	bool handleZRotate(const CEGUI::EventArgs& e);
	bool handleYRotate(const CEGUI::EventArgs& e);
	bool handleXRotate(const CEGUI::EventArgs& e);

	bool handleAnimationMove(const CEGUI::EventArgs& e);
	bool handlePlayAnimation(const CEGUI::EventArgs& e);

	
	bool IsRunning() { return m_ThisID != 0; }
	float getScale() { return m_Scale; }

	float m_ZRotate, m_XRotate, m_YRotate;
	
	bool m_bPlaying;
	bool m_bPlayRequested;
	bool m_bSeekRequested;


	
	float m_CurrentFrame;
	
	void DoScroll(float f ) 
	{
		m_AnimationSlider->setScrollPosition(f);
	}
	void setMaxFrame(float f ) 
	{
		m_AnimationSlider->setDocumentSize(f);
	}
private:
	void PopulateModelList();
	void PopulateAnimationList();


	long m_PreloaderID;
	long m_VFSID;
	Listbox* pModelList;
	Listbox* pAnimationList;
private:
	float m_Scale;
	long m_ThisID;
	Window* myRoot;
	FrameWindow* pModelLoadWindow;
	FrameWindow* pAnimationLoadWindow;
	bool m_bModelWindowShown;
	bool m_bAnimationWindowShown;
	Slider* m_Zslider ;
	Scrollbar* m_XRSlider, *m_YRSlider , *m_ZRSlider;
	Scrollbar* m_AnimationSlider;
	PushButton* m_PlayButton;

	std::vector<ListboxTextItem*> m_ModelListElements;
	std::vector<ListboxTextItem*> m_AnimListElements;

};

#endif