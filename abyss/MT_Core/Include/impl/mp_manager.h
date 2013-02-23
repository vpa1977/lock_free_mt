#ifndef MP_MANAGER_HPP

#define MP_MANAGER_HPP
#include "stdafx.h"
#include "mutex.h"
#include <hash_map>
#include <set>
#include "publishers.h"
#include "queue.h"
#include "command.h"
#include "allocator.h"
#include "section.h"
#include <time.h>
#include "system_commands.h"


typedef std::vector<section*> lurking_sections;
//typedef std::hash_map<long , std::vector<int> > allowed_threads;
typedef std::hash_map<unsigned long,section*> section_map;
// thread data structures.
void ThreadProc(void* pArgs);

// thread statistics
struct thread_stats
{
	thread_stats() : m_wait_time(0) , m_queue_size(0) 
	{
	}
	float m_wait_time;
	long  m_queue_size;
};

typedef subscriber<thread_stats> thread_statistics_subscriber;


struct thread
{
	thread() : m_id(0), m_sleeping(0), m_handle(0), m_thread_allocator(128*1024), m_pexclusive_section(0), m_pIdleProc(0), m_pKickProc(0), m_stopped(0), m_searching(0)
	{
	}
	int m_id;
	volatile long m_stopped;
	volatile long m_sleeping;
	volatile long m_searching;
	HANDLE m_handle;
	MSQueue<message> m_thread_queue;
	thread_stats m_stats;
	publisher<thread_stats> m_stat_publisher;
	std::vector< thread_statistics_subscriber > m_thread_stats;
	subscriber<section_map> m_section_map_subscriber;
//	subscriber<allowed_threads> m_allowed_threads_subscriber;
	chain_allocator m_thread_allocator;
	subscriber<lurking_sections> m_lurking_sections_subscriber;
	volatile section* m_pexclusive_section;
	volatile IdleProc m_pIdleProc;
	volatile KickProc m_pKickProc;

///	void schedule( const message& m , const section * s, long dest = -1 );
};






class section_factory 
{
public:
	// instantiate a new section
	virtual section* create(unsigned long id) = 0;
	// retrieve section type information
//	virtual const section_descriptor& get_descriptor() const = 0;
};


class mp_manager
{
public:
	mp_manager() : m_section_id(-1) , m_startup_section(-1), m_active_commands(0)
	{
	}
	static mp_manager* get_instance()
	{
		if (!m_pmanager ) m_pmanager = new mp_manager();
		return m_pmanager;
	}
public:
	atomic_long m_active_commands;
	void set_section_id(long sec_id)
	{
		m_section_id = sec_id;
	}
	std::vector<thread*>& get_threads() { return m_threads; }
	void register_section(unsigned long section_type_id,  section_factory* p_factory);
	section* create_section(unsigned long section_type_id);
	bool stop_section(unsigned long section_id);
	bool start_threads(int thread_count);
	bool kill_thread();
	void run_system();
	void dump_sections();
	
	void set_startup(long cls_id) 
	{
		if (m_startup_section >=0 ) 
		{
			assert(0 && "Cannot register startup section more than once");
		}
		m_startup_section = cls_id;
	}
	long increase_section_id()
	{
		return m_section_id++;
	}
	void check_threads(section* pSection);
private:
	static mp_manager* m_pmanager;

private:
	std::vector<thread*> m_threads;
	std::hash_map<unsigned long,section_factory*>  m_section_types;
	
	long m_section_id;
	long m_startup_section;
};




#endif