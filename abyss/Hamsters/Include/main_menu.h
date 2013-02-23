#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP
#include "hamsters.h"
#include "hamster_section_interface.h"
#include "state_controller_section.h"


enum MAIN_MENU_COMMANDS {
	UNDEFINED, 
	NEW_GAME,
	LOAD_GAME,
	OPTIONS,
	QUIT
};

class CMainMenuSection : public CState
	,public CTCommandSender<CHamster_InitMainMenuRequest>
	,public CTCommandSender<CHamster_HideMainMenuRequest>
	
{
public:
	CMainMenuSection( long inSectionID) : CState(inSectionID) 
	{
		REGISTER_REACTION(CMainMenuSection, CHamster_InitMainMenuResponse);
		REGISTER_REACTION(CMainMenuSection, CHamster_HideMainMenuResponse);
		REGISTER_REACTION(CMainMenuSection,CHamster_NewGame);
		m_Command = UNDEFINED;
	}
	void SetRenderSectionID(long id){
		m_RenderSectionID = id;
	}
	virtual std::string GetName()
	{
		return "MAINMENU";
	}
	void Reaction(long srcId, const CHamster_HideMainMenuResponse& out);
	void Reaction(long srcId, const CHamster_InitMainMenuResponse& out);
	void Reaction(long srcID, const CHamster_NewGame& in ) ;
protected:
	virtual void UpdateState(long inSectionID,const CState_UpdateRequest& update);
	/** 
		Initialization of resources which are needed for lifetime is done here
	*/
	virtual void InitState(long inSectionID,const CState_Init& update);
	/**
		Allocate temporary resources, free them before invoking EndState
	*/
	virtual void Allocate(long inSectionID,const CState_AllocateResourcesRequest& update);
	/**
		Invoked when we dispose of section
	*/
	virtual void Release(long inSectionID,const CState_ReleaseResourcesRequest& update);
private:
	long m_RenderSectionID;
	MAIN_MENU_COMMANDS m_Command;
};

#endif