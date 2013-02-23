#ifndef SYSTEM_SECTION_HPP
#define SYSTEM_SECTION_HPP
#include "section.h"
#include "stdafx.h"
#include "mp_interface.h"
#include "mp_manager.h"
#include "idgen.h"
#include "system_commands.h"
#include "thread_synch_commands.h"

#include <map>


void SystemSectionIdle( thread* pThread, section* pSystemSection);
void SystemSectionKick(thread* pThread, section* pSystemSection);

class system_section : public section, 
	public CTCommandSender<CCommand_LaunchSection>, 
	public CTCommandSender<CCommand_LaunchSectionResult>,
	public CTCommandSender<CCommand_SectionStartup>,
	public CTCommandSender<CCommand_Timer_Event>
{


//	struct thread_info
//	{
//		int sections_assigned;
//		int thread_id;
//		
//	};
	// threads which have bound sections 
//	std::vector<thread_info> m_bound_threads;
	// threads which doesnt have any bound sections
//	std::vector<int> m_free_threads;

	HANDLE m_handles[MAXIMUM_WAIT_OBJECTS];



public:
	struct CTimerInfo
	{
		CTimerInfo( HANDLE in_TimerHandle=NULL, long in_SectionID=SECTION_ID_ERROR, unsigned long in_Magic=0 ) :
			m_TimerHandle(in_TimerHandle)
			,m_SectionID(in_SectionID)
			,m_Magic(in_Magic)
		{}
		HANDLE m_TimerHandle;
		long m_SectionID;
		unsigned long m_Magic;
	};

	std::map< HANDLE, CTimerInfo> m_assigned_timers;
	std::vector< HANDLE > m_timer_handles;

	

	system_section(long id ) : section(id)
	{
		// register reactions.
		RegisterReaction( GetID( (CCommand_SectionStartup*)NULL), CTConform<system_section, CCommand_SectionStartup>::Conform);
		RegisterReaction( GetID((CCommand_LaunchSection*)NULL) , CTConform<system_section, CCommand_LaunchSection>::Conform);
		RegisterReaction( GetID((CCommand_SectionQuit*)NULL) , CTConform<system_section, CCommand_SectionQuit>::Conform);
		RegisterReaction( GetID((CCommand_ThreadSync_SectionQuitResponse*)NULL) , CTConform<system_section, CCommand_ThreadSync_SectionQuitResponse>::Conform);		
		RegisterReaction( GetID((CCommand_ThreadSync_SectionStartResponse*)NULL) , CTConform<system_section, CCommand_ThreadSync_SectionStartResponse>::Conform);		
		RegisterReaction( GetID((CCommand_SetTimer_Request*)NULL) , CTConform<system_section, CCommand_SetTimer_Request>::Conform);		
		RegisterReaction( GetID((CCommand_QuitSystem*)NULL) , CTConform<system_section, CCommand_QuitSystem>::Conform);		
		
	};


	bool add_timer(long in_SrcSection,const CCommand_SetTimer_Request& in_rTimerRequest); 
	void send_timer_command(const CTimerInfo& info);
	


	void Reaction(long in_SrcSectionID, const CCommand_SetTimer_Request& in_rSetTimer);

	void Reaction(long src_id , const CCommand_ThreadSync_SectionQuitResponse& in_rThreadSynch);
	void Reaction(long src_id , const CCommand_LaunchSection& in_LaunchReq);
	void Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand );
	void Reaction( long in_SrcSectionID, const CCommand_SectionQuit& in_rQuit);
	void Reaction(long in_SrcSectionID , const CCommand_ThreadSync_SectionStartResponse& in_rStart);
	void Reaction(long in_Src, const CCommand_QuitSystem& in );
	void start_self(long startup_clsid, long flags=SF_NONE);
private:
//	bool allocate_threads(const section* pSection, long partner = 0) ;
//	bool reallocate_threads( allowed_threads& old_config, allowed_threads& new_config);
//	std::vector<int> get_available_threads();


//	allowed_threads m_thread_config;
//	publisher<allowed_threads> m_allowed_threads_publisher;
	
	section_map m_section_map;
	publisher<section_map> m_section_map_publisher;

	lurking_sections m_lurking_sections;
	publisher<lurking_sections> m_lurking_sections_publisher;
public:

	void Idle(thread* pThread, section* pSection);
	void Kick(thread* pThread, section* pSection);



};






#endif