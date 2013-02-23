#include "../Include/impl/stdafx.h"
#include "../Include/impl/system_section.h"
#include "../Include/impl/thread_synch_commands.h"
#include <time.h>


static CTSectionRegistrator<SYSTEM_CLSID, system_section> gSystemRegistrator(false);

// declarations from winbase


// end declarations from winbase
void system_section::send_timer_command(const system_section::CTimerInfo& info) 
{
	CCommand_Timer_Event command(info.m_Magic);
	CTCommandSender<CCommand_Timer_Event>::SendCommand(info.m_SectionID , command);
}
void SystemSectionIdle( thread* pThread, section* pSystemSection) 
{

	system_section* sec = (system_section*) pSystemSection;

	std::map< HANDLE, system_section::CTimerInfo>::iterator it = sec->m_assigned_timers.begin();
	sec->m_timer_handles.clear();
	while (it != sec->m_assigned_timers.end() ) 
	{
		sec->m_timer_handles.push_back(it->first);
		it ++;
	}
	std::vector<HANDLE> wait_objects;
	wait_objects.push_back( pThread->m_handle);
	if (sec->m_timer_handles.size() ) 
	{
		wait_objects.insert(wait_objects.end() , sec->m_timer_handles.begin() , sec->m_timer_handles.end());
	}
	DWORD WaitResult = WaitForMultipleObjects( wait_objects.size() , &wait_objects[0], FALSE, INFINITE);
	DWORD SignaledIndex =WaitResult-WAIT_OBJECT_0;
	assert( 0 <= SignaledIndex                            );
//	assert(      SignaledIndex < rIndexTimerHandle.size() );
	if (SignaledIndex > 0 ) 
	{
		SignaledIndex --;
		HANDLE SignalledHandle = sec->m_timer_handles[SignaledIndex];
		std::map< HANDLE, system_section::CTimerInfo>::iterator it = sec->m_assigned_timers.find(SignalledHandle);
		const system_section::CTimerInfo& info = it->second;
		sec->send_timer_command(info);
		sec->m_assigned_timers.erase(it);
		CloseHandle(SignalledHandle);
		
	}
	else
	{
		ResetEvent(pThread->m_handle);
	}
	
}

void SystemSectionKick(thread* pThread, section* pSystemSection)
{
	system_section* sec = (system_section*) pSystemSection;
	SetEvent( pThread->m_handle);
}

bool system_section::add_timer(long in_SrcSectionID, const CCommand_SetTimer_Request& in_rTimerRequest) 
{
        if (!m_timer_handles.size() > MAXIMUM_WAIT_OBJECTS-2 ) 
		{
			return false;
		}
		HANDLE new_handle = CreateWaitableTimer(NULL, FALSE, NULL );
		if( !new_handle )
		{
			return false;
		}
		LARGE_INTEGER DueTime;
		DueTime.QuadPart = ((LONGLONG)-10)*((LONGLONG)in_rTimerRequest.m_TimeMicroseconds); // SetWaitableTimer expects time in 100ns units.
		//DueTime.QuadPart = -100000000;
		BOOL bSetResult = SetWaitableTimer( new_handle, &DueTime, 0, NULL, NULL, 0 );
		if( 0==bSetResult )
		{
			return false;
		}
		CTimerInfo info( new_handle, in_SrcSectionID, in_rTimerRequest.m_MagicID);
		//m_timer_handles.push_back(new_handle);
		m_assigned_timers[new_handle] = info;
		return true;
}


void system_section::Reaction(long in_SrcSectionID, const CCommand_SetTimer_Request& in_rRequest) 
{
	bool result = add_timer(in_SrcSectionID, in_rRequest);
}

void system_section::Reaction(long in_SrcSectionID, const CCommand_ThreadSync_SectionStartResponse& in_rStart) 
{
	CCommand_LaunchSectionResult result(in_rStart.m_Magic, SLR_OK ,in_rStart.m_psection->GetThisID());
	CCommand_SectionStartup c(GetThisID() ,in_rStart.m_psection->GetThisID());
	CTCommandSender<CCommand_SectionStartup>::SendCommand(in_rStart.m_psection->GetThisID(), c);
	CTCommandSender<CCommand_LaunchSectionResult>::SendCommand(in_rStart.src_id, result);
}
/*
std::vector<int> system_section::get_available_threads()
{
	std::vector<int> new_vector;
	for (int i = 0; i < m_bound_threads.size() ; i ++ ) 
	{
		new_vector.push_back(m_bound_threads[i].thread_id);
	}
	new_vector.insert(new_vector.end() , m_free_threads.begin(), m_free_threads.end());
	return new_vector;
}


//long index = 0;
bool system_section::reallocate_threads( allowed_threads& old_config, allowed_threads& new_config)
{
	//index = 0;
	std::vector<int> new_vector = get_available_threads();
	if (!new_vector.size() ) 
		return false;
	allowed_threads::iterator it = old_config.begin();
	while (it != old_config.end() ) 
	{
		int section_id = it->first;
		section_map::iterator itSM = m_section_map.find(section_id);
		if (itSM != m_section_map.end()) 
		{
			if (itSM->second->m_running_flag.get())
			{
				long flags = itSM->second->m_flags;
				// normal section
				if (!(flags& SF_EXCLUSIVE) && !(flags & SF_BOUND) && !(flags & SF_PARTNER) )
				{
			
					new_config[section_id ] = new_vector;
			//		shift_vector(new_vector);
				}
				else
				{
					new_config[section_id ] = it->second;
				}
			}
		}
		it++;
	}
	return true;
}
*/

/*
bool system_section::allocate_threads(const section* pSection, long partner ) 
{
	// if pSection is a normal section
	long flags = pSection->m_flags;
	allowed_threads new_config;

	if (! (flags& SF_EXCLUSIVE) && !(flags & SF_BOUND) && !(flags & SF_PARTNER) )
	{
		new_config = m_thread_config;
		std::vector<int> new_vector;
		for (int i = 0; i < m_bound_threads.size() ; i ++ ) 
		{
			new_vector.push_back(m_bound_threads[i].thread_id);
		}
		new_vector.insert(new_vector.end() , m_free_threads.begin(), m_free_threads.end());
		new_config[ pSection->GetThisID() ] = new_vector;
		//  no reallocate here
	}
	else
	if ((flags & SF_EXCLUSIVE) && !(flags & SF_PARTNER) ) 
	{
		if (!m_free_threads.size() ) return false;
		int thread_id =m_free_threads[0];
		m_free_threads.erase(m_free_threads.begin());
		//m_free_threads.
		// for reschedule all "common" threads on remaining ones
		std::vector<int> m_excl;
		m_excl.push_back(thread_id);
		new_config[ pSection->GetThisID() ] = m_excl;
		if (!reallocate_threads( m_thread_config, new_config))
		{
			return false;
		}
	}
	else
	if (flags & SF_PARTNER) 
	{
		// simple case
		allowed_threads::iterator it = m_thread_config.find(partner);
		if (it == m_thread_config.end() ) return false;
		m_thread_config[ pSection->GetThisID() ] = it->second;
		m_allowed_threads_publisher.set_value(m_thread_config);
		return true;
	}
	else
	if (flags & SF_BOUND ) 
	{
		int thread_id = -1;
		if (!m_free_threads.size()) 
		{
			// 
			int index = -1;
			int assigned = 9999999;
		
			for (int i = 0; i < m_bound_threads.size() ; i ++ ) 
			{
				if (assigned > m_bound_threads[i].sections_assigned ) 
				{
					assigned = m_bound_threads[i].sections_assigned;
					index = i;
				}
			}
			if ( index < 0 ) return false;

			thread_id = m_bound_threads[index].thread_id;
			m_bound_threads[index].sections_assigned ++;

			// no realloc needed here
		}
		else
		{
			thread_id =m_free_threads[0];
			m_free_threads.erase(0);
			thread_info info;
			info.thread_id = thread_id;
			info.sections_assigned = 1;
			m_bound_threads.push_back(info);
		}
		if (thread_id < 0 ) return false;
		std::vector<int> m_excl;
		m_excl.push_back(thread_id);
		new_config[ pSection->GetThisID() ] = m_excl;
	}

	m_thread_config = new_config;
	
	// 


}
*/

void system_section::start_self(long startup_clsid, long flags)
{

	section* newSection = mp_manager::get_instance()->create_section(SYSTEM_CLSID);
	mp_manager::get_instance()->set_section_id(SECTION_ID_FIRST_AVAILABLE);
	int size = mp_manager::get_instance()->get_threads().size();
	
	newSection->m_flags = SF_EXCLUSIVE | SF_BOUND;
	
	

	// temporarry.
	// create an initial thread config
//	for (int i = 1 ; i < size ; i ++ ) ((system_section*)newSection)->m_free_threads.push_back(i);
//	allowed_threads new_config;

	const std::vector<thread*>& threads =mp_manager::get_instance()->get_threads();
	for (unsigned int i = 0 ; i < threads.size() ; i ++ ) 
	{
		((system_section*)newSection)->m_section_map_publisher.add_listener( &threads[i]->m_section_map_subscriber);
		((system_section*)newSection)->m_lurking_sections_publisher.add_listener( &threads[i]->m_lurking_sections_subscriber);
		//((system_section*)newSection)->m_allowed_threads_publisher.add_listener( &threads[i]->m_allowed_threads_subscriber);
	}
	long section_id = newSection->GetThisID();
	
//	std::vector<int> system_thread_vec;
//	system_thread_vec.push_back(0);
//	new_config[section_id] = system_thread_vec;
	
//	((system_section*)newSection)->m_thread_config= new_config;
//	((system_section*)newSection)->m_allowed_threads_publisher.set_value(((system_section*)newSection)->m_thread_config);
	
	
	((system_section*)newSection)->m_section_map[newSection->GetThisID()] = newSection;
	((system_section*)newSection)->m_section_map_publisher.set_value(((system_section*)newSection)->m_section_map);
	((system_section*)newSection)->m_lurking_sections_publisher.set_value(m_lurking_sections);

	if (!((system_section*)newSection)->m_pthread )
		((system_section*)newSection)->m_pthread = mp_manager::get_instance()->get_threads()[0];

	
	CCommand_LaunchSection launch(startup_clsid, 0, flags);
	message m;
	m.m_clock = clock();
	m.m_destination = 0;
	m.m_source = 0;
	m.m_type = GetID(&launch);
	CCommand_LaunchSection* tPtr = static_cast<CCommand_LaunchSection*>(((system_section*)newSection)->get_thread()->m_thread_allocator.allocate( sizeof(CCommand_LaunchSection)));
	m.pCommand = new (tPtr) CCommand_LaunchSection(launch);
	m.pAllocator = ((system_section*)newSection)->get_thread()->m_thread_allocator.get_chunk();
	// install thread ownership and procs.
	((system_section*)newSection)->m_pthread->m_pexclusive_section = newSection;
	((system_section*)newSection)->InstallThreadProcs(&SystemSectionIdle, &SystemSectionKick);
	// end 
	((system_section*)newSection)->m_psection_queue->enqueue(m, ((system_section*)newSection)->get_thread()->m_thread_allocator);
	((system_section*)newSection)->m_pbound_thread = ((system_section*)newSection)->get_thread();
}

void system_section::Reaction(long src_id, const CCommand_LaunchSection& in_LaunchReq)
{
	section* newSection = mp_manager::get_instance()->create_section(in_LaunchReq.m_SectionClassID);
	CCommand_LaunchSectionResult result(in_LaunchReq.m_Magic, SLR_OK , newSection->GetThisID());
	long flags = in_LaunchReq.m_Flags;
	newSection->m_flags = flags;

	// exclusive section without partner
	if ((flags & SF_EXCLUSIVE) && !(flags & SF_PARTNER))
	{
		int size = mp_manager::get_instance()->get_threads().size();
		int curr = 0;
		for (curr = 0 ; curr < size ; curr ++ ) 
		{
			thread* currThread = mp_manager::get_instance()->get_threads()[curr];
			if (currThread->m_pexclusive_section) 
			{
				continue;
			}
			else
			{
				break;
			}

		}
		if (curr < size ) 
		{
			InterlockedCompareExchangePointer((volatile PVOID*)&mp_manager::get_instance()->get_threads()[curr]->m_pexclusive_section, newSection, NULL);			
			if (in_LaunchReq.m_pKickProc && in_LaunchReq.m_pIdleProc) 
				{
					InterlockedExchangePointer(&mp_manager::get_instance()->get_threads()[curr]->m_pKickProc, in_LaunchReq.m_pKickProc);
					InterlockedExchangePointer(&mp_manager::get_instance()->get_threads()[curr]->m_pIdleProc, in_LaunchReq.m_pIdleProc);
				}
				InterlockedExchangePointer( & newSection->m_pbound_thread, mp_manager::get_instance()->get_threads()[curr]);
		}
		else
		{
			result.m_ErrorCode = SLR_FLAGS_ARE_TOO_HARD;
			CTCommandSender<CCommand_LaunchSectionResult>::SendCommand(src_id, result);
			return;
		}
	}
	else
	// partner section recieves queue of parent.
	if (flags & SF_PARTNER)
	{
		section_map::iterator it = m_section_map.find( in_LaunchReq.m_Partner);
		if ( it == m_section_map.end() ) 
		{
			result.m_ErrorCode = SLR_FLAGS_ARE_TOO_HARD;
			CTCommandSender<CCommand_LaunchSectionResult>::SendCommand(src_id, result);
			return;

		}
		else
		{
			section * pMaster = it->second;
			newSection->m_psection_queue = pMaster->m_psection_queue;
			InterlockedExchangePointer( & newSection->m_pbound_thread, pMaster->m_pbound_thread);
		}
	}
	//else


	newSection->InitSubSections();
	section::sub_section_type::const_iterator it = newSection->GetSubSections().begin();
	while (it != newSection->GetSubSections().end() ) 
	{
		section* pSection = it->second;
		m_section_map[pSection->GetThisID() ] = (section*)pSection;
		// add subsections to lurkers
		if (pSection->m_flags == SF_NONE ) 
		{
			m_lurking_sections.push_back(pSection);
		}
		it ++ ;
	};
	
	/// create subsections and update the map
	m_section_map[newSection->GetThisID()] = newSection;
	m_section_map_publisher.set_value(m_section_map);
	
	if (flags == SF_NONE ) 
	{
		m_lurking_sections.push_back(newSection);
		assert(newSection->GetThisID() != SYSTEM_SECTION_ID);
		m_lurking_sections_publisher.set_value( m_lurking_sections);
	}
	


	{
		CCommand_SectionStartup c(GetThisID() ,newSection->GetThisID());
		CTCommandSender<CCommand_SectionStartup>::SendCommand(newSection->GetThisID(), c);
		CTCommandSender<CCommand_LaunchSectionResult>::SendCommand(src_id, result);

	}
	if (flags & SF_EXCLUSIVE) // a switch is done 
	{
		SetEvent( newSection->m_pbound_thread->m_handle);
	}
	

}
void system_section::Reaction( long in_SrcSectionID, const CCommand_SectionQuit& in_rQuit)
{
	section_map::iterator it =m_section_map.find(in_SrcSectionID);
	if (it == m_section_map.end() ) 
	{
		assert(0 && "Section not found ");
		return ;
	}
	section* pSection = it->second;
	// remove section from the map and publish
	lurking_sections::iterator lurk;
	for (lurk= m_lurking_sections.begin();lurk != m_lurking_sections.end() ; lurk ++ ) 
	{
        if (*lurk == pSection )
		{
			m_lurking_sections.erase(lurk);
			break;
		}
	}
	m_lurking_sections_publisher.set_value(m_lurking_sections);
	m_section_map.erase(it);
	m_section_map_publisher.set_value(m_section_map);

	
	// if section has bound thread.
	// switch owner to either remaining section
	// or free section from the guilt.
	if (pSection->m_pbound_thread) 
	{
		volatile thread* pBoundThread = pSection->m_pbound_thread;
		volatile section* thread_owner;
		InterlockedExchangePointer(&thread_owner, pSection->m_pbound_thread->m_pexclusive_section);
		if (thread_owner == pSection) 
		{
			thread_owner = NULL;
			// find other thread to take ownership
			section_map::iterator findPartner= m_section_map.begin();
			while (findPartner != m_section_map.end() ) 
			{
				section* partner = findPartner->second;
				if (partner->m_pbound_thread == pSection->m_pbound_thread ) 
				{
					thread_owner = partner;
					break;
				}
				findPartner++;
			}
		}
		InterlockedExchangePointer(&pSection->m_pbound_thread->m_pexclusive_section, thread_owner);
		volatile KickProc proc;
		InterlockedExchangePointer(& proc ,  pBoundThread->m_pKickProc);
		if (proc ) 
		{
			InterlockedExchangePointer(& pBoundThread->m_pKickProc, NULL ) ;
			InterlockedExchangePointer(& pBoundThread->m_pIdleProc , NULL );
			(*proc)( get_thread() , pSection);
		}
		
		
	}
	// make sure all threads are not in search phase
	for (int i = 0 ; i < mp_manager::get_instance()->get_threads().size() ; i ++ ) 
	{
		thread* pThread = mp_manager::get_instance()->get_threads()[i];
		while ( InterlockedCompareExchange( &pThread->m_searching , 0 , 0 ))
		{
		}
	}
	// wait for section to stop work
	while ( InterlockedCompareExchange( &pSection->m_busy_flag , 0 , 0 ))
	{
	}
    delete pSection;	
	// time to quit
	long size = m_section_map.size();
	if (m_section_map.size() == 1 ) 
	{
		for (int i = 0 ; i < mp_manager::get_instance()->get_threads().size() ; i ++ ) 
		{
			thread* pThread = mp_manager::get_instance()->get_threads()[i];
			InterlockedExchange( & pThread->m_stopped , 1 );
			SetEvent(pThread->m_handle);
		}
	}
}
void system_section::Reaction( long in_SrcSectionID, const CCommand_ThreadSync_SectionQuitResponse& in_rSynch )
{
	
}
void system_section::Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )
{
/*	std::vector<thread*>& threads =mp_manager::get_instance()->get_threads();
	for (unsigned int i = 0 ; i < threads.size() ; i ++ ) 
	{
		m_section_map_publisher.add_listener( &threads[i]->m_section_map_subscriber);
		m_allowed_threads_publisher.add_listener( &threads[i]->m_allowed_threads_subscriber);
	}
	//
*/	
}

void system_section::Reaction(long in_Src, const CCommand_QuitSystem& in ) 
{
	section_map::iterator it = m_section_map.begin();
	for (it = m_section_map.begin();it != m_section_map.end();it++ ) 
	{
		if (it->second->GetThisID()  == SYSTEM_SECTION_ID ) 
		{
			
			continue;
		}
		message m;
		m.m_clock = clock();
		m.m_destination = SYSTEM_SECTION_ID;
		m.m_source = it->second->GetThisID();
		m.m_type = GetID<CCommand_SectionQuit>((CCommand_SectionQuit*)NULL);
		CCommand_SectionQuit* tPtr = static_cast<CCommand_SectionQuit*>(get_thread()->m_thread_allocator.allocate( sizeof(CCommand_SectionQuit)));
		m.pCommand = new (tPtr) CCommand_SectionQuit();
		m.pAllocator = get_thread()->m_thread_allocator.get_chunk();
		//const section_map * pMap = get_thread()->m_section_map_subscriber.get_property();
		//pMap->find(SYSTEM_SECTION_ID)->second->m_psection_queue->enqueue(m, get_thread()->m_thread_allocator);
		schedule(m);
		
	}
}

void system_section::Idle(thread* pThread, section* pSection)
{
	
}

void system_section::Kick(thread* pThread, section* pSection) 
{

}