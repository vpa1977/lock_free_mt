#include "../include/state_controller_section.h"



const std::string& CStateConfiguration::GetNextState(const std::string& state, const std::string& transition) const
{
	trans_map::const_iterator it = m_TransitionMap.find(state+ transition);
	if (it == m_TransitionMap.end() )
	{
		return "";
	}
	return it->second;
}
void CStateConfiguration::AddTransition(const std::string& state, const std::string& transition, const std::string& nextState)
{
	m_TransitionMap[state+transition] = nextState;
}



CSectionController::CSectionController(long inSectionID) : 
		CGameBaseSection(inSectionID)
		,m_bInTransition(true)
		,m_NFramesToRender(2)
		,m_FirstTimeFrame(0)
		,m_LastTimeFrame(0)
		,m_NFrames(0) 
		,m_pProxy(NULL)
{

	REGISTER_REACTION(CSectionController,CState_InitResponse);
	REGISTER_REACTION(CSectionController,CState_AllocateResourcesResponse);
	REGISTER_REACTION(CSectionController,CState_UpdateResponse);
	REGISTER_REACTION(CSectionController,CState_EndState);
	REGISTER_REACTION(CSectionController,CVFSSection_MountResponse);
	REGISTER_REACTION(CSectionController,CEGUI_Render_InitResponse);
	REGISTER_REACTION(CSectionController,CRenderSection_NewFrame);
	
	REGISTER_REACTION(CSectionController,CEGUI_UpdateResponse);
	

	
}

const std::string& CSectionController::GetCurrentTransition() const
{
	return m_CurrentTransition;
}

void CSectionController::RunFirstState()
{	
	InitStates(m_StateConfiguration);
	//
	sub_section_type::const_iterator it = GetSubSections().begin();
	for (;it != GetSubSections().end() ; it ++ ) 
	{
		if (it->second->GetName() == m_StateConfiguration.m_StartState)
		{

			//
			StartState(it->second->GetThisID());
			return;
		}
	}
	CLog::Println("Start State was not found");
}

void CSectionController::PrepareTransitionDone()
{
}

void CSectionController::StartState(long inSectionID)
{
	m_bInTransition = true;
	m_CurrentState = inSectionID;
	m_CurrentTransition = TRANSITION_START_GAME;
	CTCommandSender<CState_AllocateResourcesRequest>::SendCommand(inSectionID, CState_AllocateResourcesRequest());
}

void CSectionController::Reaction(long inSectionID, const CState_AllocateResourcesResponse& in ) 
{
	if (in.m_bComplete) 
	{
		m_bInTransition = false;
		ReleaseTransition(GetCurrentTransition());
		m_CurrentTransition = "";
		//CTCommandSender<CState_UpdateRequest>::SendCommand(	inSectionID, CState_UpdateRequest() );	
	}
	else
	// process callback
	{

	}
}


void CSectionController::Reaction(long inSectionID, const CState_EndState& endState)
{
	m_bInTransition =true;
	m_CurrentTransition = endState.m_Signal;

	sub_section_type::const_iterator it = GetSubSections().begin();
	for (;it != GetSubSections().end() ; it ++ ) 
	{
		if (it->second->GetThisID() == inSectionID)
		{
			long result = NextState( it->second->GetName() , endState.m_Signal);
			if (result > 0 ) 
			{
				StartState(result);
			}
			return;
		}
	}
	CLog::Println("No transition found");
}


long CSectionController::NextState(const std::string& inPrevState,const std::string& inSignal)
{
	const std::string& next = m_StateConfiguration.GetNextState(inPrevState, inSignal);
	sub_section_type::const_iterator it = GetSubSections().begin();
	for (;it != GetSubSections().end() ; it ++ ) 
	{
		if (it->second->GetName() == next)
		{
			
			return it->second->GetThisID();
		}
	}
	return -1;
}// what state to choose if we ended
