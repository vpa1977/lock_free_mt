#include "../Include/log.h"
#include "../Include/impl/StdAfx.h"
#include "../Include/impl/mp_manager.h"
#include "../Include/impl/system_section.h"
#include <process.h>    /* _beginthread, _endthread */


/*
void thread::schedule( const message& m , const section* s , long tr ) 
{
	int thread_pos = tr;
	const std::vector<thread*>& pthreads = mp_manager::get_instance()->get_threads();
	if (thread_pos < 0 ) 
	{
		const section_map * pMap = this->m_section_map_subscriber.get_property();
		if (pMap) 
		{
			section_map::const_iterator it =pMap->find(m.m_destination);
			if (it != pMap->end() ) 
			{
				const section* pSection = it->second;
				volatile long possThread  =0;
				InterlockedExchange(&possThread, pSection->m_busy_flag);
				if (possThread) 
				{
					thread_pos = possThread;
				}
			}
		}
	}
	if (thread_pos < 0 ) 
	{
		const allowed_threads* p_allowed = this->m_allowed_threads_subscriber.get_property();
		allowed_threads::const_iterator it = p_allowed->find(m.m_destination);
		if (it == p_allowed->end())
		{
			// not valid destination
			CLog::Print("No valid destination for message !\n");
			return;
		}

		const std::vector<int>& candidates = it->second;
		
		if (!candidates.size())
		{
			// no thread to schedule
			assert( candidates.size() );
			return;
		}
		// analyse thread states.
		int candidate = 0;
		int wait = -1;
		for (int i = 0 ; i < candidates.size() ; i ++ ) 
		{
			int size = mp_manager::get_instance()->get_threads()[ candidates[i] ]->m_thread_queue.m_size.get();
			if (wait < 0 ) 
			{
				wait = size;
				candidate = i;
			}
			if (size < wait ) 
			{
				wait = size;
				candidate = i;
			}
		}
		thread_pos = candidates[candidate];
	}
	// put in queue.
	pthreads[thread_pos  ]->m_thread_queue.enqueue(m, this->m_thread_allocator);
	volatile long sleep_flag;
	InterlockedExchange(&sleep_flag, pthreads[ thread_pos ]->m_sleeping);
	volatile KickProc pKick;
	volatile section*  pSection;
	InterlockedExchangePointer(&pKick, pthreads[thread_pos  ]->m_pKickProc);
	InterlockedExchangePointer(&pSection, pthreads[thread_pos  ]->m_pexclusive_section);

	if (sleep_flag) 
	{
		if (pKick && pSection) 
		{
			(*pKick)( pthreads[thread_pos  ] ,(section*) pSection);
		}
		else
		{
			SetEvent( pthreads[ thread_pos ]->m_handle);
		}
	}

}

void section::schedule(const message& m , int tr ) 
{
	m_pthread->schedule(m, this, tr);
};
*/