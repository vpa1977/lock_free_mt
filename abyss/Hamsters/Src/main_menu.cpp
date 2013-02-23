#include "../include/hamsters.h"
#include "../include/main_menu.h"



void CMainMenuSection::Reaction(long inSectionID, const CHamster_NewGame& in ) 
{
	if (m_Command == UNDEFINED ) m_Command = NEW_GAME;
}


void CMainMenuSection::UpdateState(long inSectionID,const CState_UpdateRequest& update)
{
//	CRenderSection_SetCamera render(CVector(0,0,0), CVector(0,0,1),,CVector(0,1,0), 1,0,100);
//	CTCommandSender<CRenderSection_SetCamera>::SendCommand( m_RenderSectionID, render);
	if (m_Command == UNDEFINED) 
	{
			// do something 
	}
	else
	
	{
		CTCommandSender<CHamster_HideMainMenuRequest>::SendCommand(inSectionID, CHamster_HideMainMenuRequest());
	}
	SendUpdate();
}
	/** 
		Initialization of resources which are needed for lifetime is done here
	*/
void CMainMenuSection::InitState(long inSectionID,const CState_Init& update)
{
	m_Command = UNDEFINED;
	// do nothing for now.
}
	/**
		Allocate temporary resources, free them before invoking EndState
	*/
void CMainMenuSection::Allocate(long inSectionID,const CState_AllocateResourcesRequest& update)
{
	m_Command = UNDEFINED;
	CTCommandSender<CHamster_InitMainMenuRequest>::SendCommand(inSectionID, CHamster_InitMainMenuRequest());
}

void CMainMenuSection::Reaction(long srcId, const CHamster_InitMainMenuResponse& out)
{
	StartState();
}

void CMainMenuSection::Reaction(long srcId, const CHamster_HideMainMenuResponse& out)
{
	if (m_Command == NEW_GAME) 
	{
		EndState("GAME");
	}
	
}

	/**
		Invoked when we dispose of section
	*/
void CMainMenuSection::Release(long inSectionID,const CState_ReleaseResourcesRequest& update)
{
}