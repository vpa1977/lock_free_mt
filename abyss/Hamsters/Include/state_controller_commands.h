#ifndef STATE_CONTROLLER_COMMANDS_HPP
#define STATE_CONTROLLER_COMMANDS_HPP

#define TRANSITION_START_GAME "START"
#define TRANSITION_STOP_GAME  "STOP"



struct CState_InitResponse : public CCommand
{
};
struct CState_AllocateResourcesResponse : public CCommand
{
	CState_AllocateResourcesResponse() : m_bComplete(true), m_Progress(100)
	{
	}
	char m_Progress;
	bool m_bComplete;
};

struct CState_UpdateResponse :public CCommand
{
};
struct CState_EndState : public CCommand
{
	std::string m_Signal;
};

struct CState_Init : public CCommand
{
};
struct 	CState_UpdateRequest: public CCommand
{
};
struct CState_AllocateResourcesRequest: public CCommand
{
};
struct CState_ReleaseResourcesRequest: public CCommand
{
};

#endif