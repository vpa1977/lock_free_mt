#include "../Include/log.h"
#include "../Include/impl/StdAfx.h"
#include "../Include/impl/mp_manager.h"
#include "../Include/impl/system_section.h"
#include <process.h>    /* _beginthread, _endthread */


/* tbb test */




mp_manager* mp_manager::m_pmanager = NULL;

void mp_manager::register_section(unsigned long section_type_id,  section_factory* p_factory)
{
	
	m_section_types[section_type_id] = p_factory;
}

void mp_manager::dump_sections()
{
	std::hash_map<unsigned long,section_factory*>::iterator it = m_section_types.begin();
	while (it != m_section_types.end() ) 
	{
		CLog::Print("%d - %s\n"  , it->first );
		it++;
	}
}

section*  mp_manager::create_section(unsigned long section_type_id ) 
{
	std::hash_map<unsigned long,section_factory*>::iterator it = m_section_types.find(section_type_id);
	if (it != m_section_types.end())
	{
		section_factory* p_factory = it->second;
		
		return p_factory->create(m_section_id++);
	}
	return NULL;
}



bool mp_manager::start_threads(int thread_count)
{
	
	for (int i = 0 ; i < thread_count ; i ++ ) 
	{
		m_threads.push_back( new thread() ) ;
		
	}
	// connecting publishers and subscribers.
	for (int i = 0 ; i < thread_count ; i ++ ) 
	{
	
		for (int j = 0 ; j < thread_count ; j++ ) 
		{
			subscriber<thread_stats> stats;	
			m_threads[j]->m_thread_stats.push_back(stats);
		}
	}
	for (int i = 0 ; i < thread_count ; i ++ ) 
	{
		for (int j = 0 ; j < thread_count ; j ++ ) {
			
			m_threads[i]->m_stat_publisher.add_listener(&m_threads[j]->m_thread_stats[i]);
		}
		m_threads[i]->m_id = i;
		m_threads[i]->m_stat_publisher.set_value(m_threads[i]->m_stats);
	}

	// starting threads
	bool result = true;
	
	return result;
}

void section::RegisterReaction(long in_CommandID, ConformFunc in_pFunc)
{
	m_reactions[in_CommandID] = in_pFunc;
}
void section::InstallThreadProcs(IdleProc idleProc, KickProc kickProc) 
	{
		assert( m_flags & SF_EXCLUSIVE ); // only exclusive section can call it.
//		assert( get_thread()->m_allowed_threads_subscriber.get_property()->find(GetThisID())->second.size() == 1);
		get_thread()->m_pIdleProc =  idleProc;
		get_thread()->m_pKickProc = kickProc;
	}
void section::Quit()
	{
		m_running_flag.set(0);
		// send message to quit
		message m;
		m.m_clock = clock();
		m.m_destination = SYSTEM_SECTION_ID;
		m.m_source = GetThisID();
		m.m_type = GetID<CCommand_SectionQuit>((CCommand_SectionQuit*)NULL);
		CCommand_SectionQuit* tPtr = static_cast<CCommand_SectionQuit*>(get_thread()->m_thread_allocator.allocate( sizeof(CCommand_SectionQuit)));
		m.pCommand = new (tPtr) CCommand_SectionQuit();
		m.pAllocator = get_thread()->m_thread_allocator.get_chunk();
		//const section_map * pMap = get_thread()->m_section_map_subscriber.get_property();
		//pMap->find(SYSTEM_SECTION_ID)->second->m_psection_queue->enqueue(m, get_thread()->m_thread_allocator);
		schedule(m);
	}

void section::schedule(const message& m ) 
{
	const section_map * pMap = get_thread()->m_section_map_subscriber.get_property();
	section_map::const_iterator it = pMap->find(m.m_destination);
	if (it == pMap->end() ) 
	{
		assert(0 && "No destination found" );
		return;
	}
	section* pSection = it->second;
	mp_manager::get_instance()->m_active_commands.inc();
	pSection->m_psection_queue->enqueue(m, get_thread()->m_thread_allocator);
	
	mp_manager::get_instance()->check_threads(pSection);
}


void mp_manager::check_threads(section* pSection)
{
	volatile thread* pBound;
	InterlockedExchangePointer(&pBound , pSection->m_pbound_thread);
	if (pBound ) 
	{
		if (true )
		{
			volatile KickProc pKick;
			InterlockedExchangePointer(&pKick, pBound->m_pKickProc);
			if (pKick && pBound) 
			{
				(*pKick)((thread*) pBound ,(section*) pBound->m_pexclusive_section);
			}
			else
			{
				SetEvent( pBound->m_handle);
			}
		}
	}
	else
	for (int i = 0 ; i < get_threads().size() ; i ++ ) 
	{
		thread* next = get_threads()[i];
		//volatile KickProc pKick;
		volatile section*  pSection;
		//InterlockedExchangePointer(&pKick, next->m_pKickProc);
		InterlockedExchangePointer(&pSection, next->m_pexclusive_section);
		if (pSection) continue;
		if (InterlockedCompareExchange(&next->m_sleeping,0,0)  )
		{
			//printf("Kicking thread  %d" , next->m_id);
			SetEvent( next->m_handle);
			
		}
		break;
	}
}


void mp_manager::run_system()
{
	section* pSection =  mp_manager::create_section(m_startup_section);
	if (!pSection) 
	{
		assert(0 && "no startup section defined " );
		return;
	}
	startup_section_config cfg = pSection->configure();
	start_threads(cfg.m_thread_count);

	system_section bootstrap(0);
	bootstrap.start_self(m_startup_section);
	// system thread occupies thread 0 and we just started processing.
	bool result = true;
	for (int i = 0 ; i < m_threads.size() ; i ++ ) 
	{
		m_threads[i]->m_handle =  CreateEvent( NULL, TRUE, TRUE, NULL );
		// system thread is ... system. we dont need any specific thread to start.
		if (i > 0 ) 
		{
			result = result && _beginthread(&ThreadProc,0, m_threads[i])!=-1; 
		}
	}
	ThreadProc( m_threads[0] ) ;
}

///////////////////////
void section::InitSubSections()
{
	std::hash_map<long, section*>::iterator it =m_sub_sections.begin();
	while (it != m_sub_sections.end() ) 
	{
		section* pSection = it->second;
		pSection->m_running_flag.set(1);
		pSection->m_id = mp_manager::get_instance()->increase_section_id();
		pSection->m_flags = m_flags;
		if (m_flags & SF_EXCLUSIVE ) 
		{
			pSection->m_psection_queue = m_psection_queue;
			pSection->m_pbound_thread = m_pbound_thread;
		}
		it ++ ;
	};
}
void section::RegisterSubSection( long in_CLSID , section* pSection ) 
{
m_sub_sections[in_CLSID] = pSection;
}

void section::setThread(thread* pThread)
{
        m_pthread= pThread;
		/*sub_section_type::iterator it = m_sub_sections.begin();
		while (it != m_sub_sections.end() ) 
		{
			it->second->setThread(pThread);
			it++;
		}*/
		
}


section::~section()
{
/*	sub_section_type::iterator it = m_sub_sections.begin();
	while (it != m_sub_sections.end() ) 
	{
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
		it++;
	}*/

}

