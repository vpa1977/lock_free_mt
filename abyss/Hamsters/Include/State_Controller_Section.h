#ifndef STATE_CONTROLLER_SECTION

#define STATE_CONTROLLER_SECTION

#include "hamsters.h"
#include "state_controller_commands.h"




typedef  std::map<std::string, std::string> trans_map;
struct CStateConfiguration
{
	std::string m_StartState;
	trans_map m_TransitionMap;
	const std::string& GetNextState(const std::string& state, const std::string& transition) const;
	void AddTransition(const std::string& state, const std::string& transition, const std::string& nextState);
};


class CSectionController : public CGameBaseSection
	,public CTCommandSender<CState_AllocateResourcesRequest>
	,public CTCommandSender<CState_UpdateRequest>
	,public CTCommandSender<CState_Init>
	,public CTCommandSender<CState_ReleaseResourcesRequest>
	,public CTCommandSender< CVFSSection_MountRequest >
	// window
	,public CTCommandSender< CWindowSection_WindowName >
	// render
	,public CTCommandSender< CRenderSection_PresentAndClear >
	,public CTCommandSender< CEGUI_Render_InitRequest >
	,public CTCommandSender<CEGUI_UpdateRequest>

{
public:
	CSectionController(long inSectionID);
	
	virtual void InitStates(CStateConfiguration& in) = 0;
	virtual void TransitionUpdate(); // send commands to finalize transition render
	virtual void PrepareTransition(const std::string& transition) = 0;
	virtual void ReleaseTransition(const std::string& transition) = 0;
	virtual CObjectRendererProxy* InitProxy() = 0; // initialize m_pProxy
	
public:
	void RunFirstState();
	void PrepareTransitionDone();

	void Reaction(long inSectionID, const CState_InitResponse& init_response);// startup
	void Reaction(long inSectionID, const CState_AllocateResourcesResponse& state_ready); // allocate resources answer
	void Reaction(long inSectionID, const CState_UpdateResponse& resp);
	void Reaction(long inSectionID, const CState_EndState& endState); // state being ended
	void Reaction( long in_SrcSectionID, const CVFSSection_MountResponse& in_rMountResponse);
	void Reaction(long in_SrcSectionID, const CEGUI_Render_InitResponse& in ) ;
	void Reaction( long in_SrcSectionID, const CRenderSection_NewFrame& in_rCmd );
	
	void Reaction(long inSectionID , const CEGUI_UpdateResponse& resp);

protected:
	virtual float GetTopFrameRate() { return 60.0f; }
	void StartTheGame();
	void Received_SectionLaunchResult(const CCommand_LaunchSectionResult& res);
	const std::string& GetCurrentTransition() const;
	long m_ProxySectionID;
	CObjectRendererProxy* m_pProxy;
	std::string m_CurrentTransition;
private:
	long m_CurrentState;
	unsigned long m_NFramesToRender;
	unsigned long m_FirstTimeFrame;
	unsigned long m_LastTimeFrame;
	unsigned long m_NFrames;
	void OneFrame();

private:
	void StartState(long in_SectionID);
	long NextState(const std::string& inPrevState,const std::string& inSignal); // what state to choose if we ended
	CStateConfiguration m_StateConfiguration;
	bool m_bInTransition;

};


class CState : public CSection
	,public CTCommandSender<CState_EndState>
	,public CTCommandSender<CState_AllocateResourcesResponse>
	,public CTCommandSender<CState_UpdateResponse>
{
	// flow 
	/**
		Controller ---- CState_Init ->> State // load data which is required for all time

		// on transition into state	
		Controller --CState_AllocateResources --> State // allocate temporary resources
												  State -- > Preloader, VFS etc
		Controller	<------	StartState------	  State	
		Controller  ---- CState_UpdateRequest --> State
		Controller  <--- CState_UpdateResponse--  State
							End_State ----------  State					
		

		// on controller exit
		Controller -------CState_ReleaseResources->State
		Controller<-CState_ReleaseResourcesResponce--State 	

	*/
public:
	CState(long inSectionID) : CSection(inSectionID) 
	{
		REGISTER_REACTION(CState, CState_UpdateRequest);
		REGISTER_REACTION(CState, CState_Init);
		REGISTER_REACTION(CState, CState_AllocateResourcesRequest);
		REGISTER_REACTION(CState, CState_ReleaseResourcesRequest);
	}
	void Reaction(long inSectionID, const CCommand_SectionStartup& in )
	{
		
	}
	/*
		Initializes state
	*/
	
	// update requests are recieved after state has 
	virtual void Reaction(long inSectionID,const CState_UpdateRequest& update);
	virtual void Reaction(long inSectionID,const CState_Init& init);
	virtual void Reaction(long inSectionID,const CState_AllocateResourcesRequest& allocate);
	virtual void Reaction(long inSectionID,const CState_ReleaseResourcesRequest& release);


protected:
	/**
		Indicates that state has completed execution 
		and ready to be hibernated
	*/
	void EndState(std::string signal);
	/**
		Indicate to controller that state has started execution
	*/
	void StartState();
	void SendUpdate();

	/*
		Perform current update of the state.
	*/
	virtual void UpdateState(long inSectionID,const CState_UpdateRequest& update)=0;
	/** 
		Initialization of resources which are needed for lifetime is done here
	*/
	virtual void InitState(long inSectionID,const CState_Init& update)=0;
	/**
		Allocate temporary resources, free them before invoking EndState
	*/
	virtual void Allocate(long inSectionID,const CState_AllocateResourcesRequest& update)=0;
	/**
		Invoked when we dispose of section
	*/
	virtual void Release(long inSectionID,const CState_ReleaseResourcesRequest& update)=0;
private:
	long m_OwnerSection;
};


#endif