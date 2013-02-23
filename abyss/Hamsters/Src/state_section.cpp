
#include "../include/hamsters.h"
#include "../include/state_controller_section.h"


void CState::Reaction(long inSectionID,const CState_UpdateRequest& update)
{
	UpdateState(inSectionID, update);
	
}
void CState::Reaction(long inSectionID,const CState_Init& init)
{
	InitState(inSectionID, init);
}
void CState::Reaction(long inSectionID,const CState_AllocateResourcesRequest& allocate)
{
	m_OwnerSection = inSectionID;
	Allocate(inSectionID, allocate);
}
void CState::Reaction(long inSectionID,const CState_ReleaseResourcesRequest& release)
{
	Release(inSectionID, release);
}

/**
	Indicates that state has completed execution 
	and ready to be hibernated
*/
void CState::EndState(std::string signal)
{

	CState_EndState endState;
	endState.m_Signal = signal;
	CTCommandSender<CState_EndState>::SendCommand(m_OwnerSection, endState);
}
void CState::SendUpdate()
{
	CTCommandSender<CState_UpdateResponse>::SendCommand(m_OwnerSection, CState_UpdateResponse());
}
/**
	Indicate to controller that state has started execution
*/
void CState::StartState()
{
	CState_AllocateResourcesResponse res;
	res.m_bComplete = true;
	CTCommandSender<CState_AllocateResourcesResponse>::SendCommand(m_OwnerSection, res);
}
