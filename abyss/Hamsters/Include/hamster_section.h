#ifndef M4_SECTION_HPP_INCLUDED
#define M4_SECTION_HPP_INCLUDED






using namespace CEGUI;

#include "hamster_gui.h"
#include "state_controller_section.h"
#include "hamster_section_interface.h"

class CHamsterSection :
	public CSectionController
	, public CTCommandSender<CHamster_InitMainMenuResponse>
	, public CTCommandSender<CHamster_HideMainMenuResponse>
	// self
	,public CTCommandSender<CHamster_NewGame>
{
public:
	CHamsterSection( long in_SectionID );
	~CHamsterSection();
	std::string GetName()
	{
		return "CHamsterSection( Milestone 4 )";
	}

	bool NewGame(const CEGUI::EventArgs& e);

public:
	// vfs
	CObjectRendererProxy* InitProxy() { return new CHamsterGUI(); };
	virtual void InitStates(CStateConfiguration& in);
	void ReleaseTransition(const std::string& transition);
	void PrepareTransition(const std::string& transition);

	void Reaction(long srcSection, const CHamster_InitMainMenuRequest& in);
	void Reaction(long srcSection, const CHamster_HideMainMenuRequest& in);
	void Reaction( long in_SrcSectionID , const CInputConsoleSection_MouseUpdate& update );

protected:
	void Received_MTInfo(const CCommand_MTInfo&);
	long GetNThreadsToRun();
	bool GetNeedVFS();
	bool GetNeedPreloader();
	
	void GetGameWindowParams( CWindowParam& );
	void Received_3DInfo( const CRenderSection_3DEnvironmentInfo& );
	void GetRenderInitParams( CRenderSection_InitRender& );
	void Received_InitRenderResult( INIT_RENDER_RESULT in_bResult );
	void Received_WindowClosed();

private:
	void AddTransitionImage();
	void ReleaseTransitionImage();
	bool m_bTransitionImageReady;
	void StartPreload();

	long m_NThreadsToRun;
	CRenderSection_3DEnvironmentInfo m_3DInfo;


};

#endif










