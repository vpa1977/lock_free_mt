#include "../Include/impl/stdafx.h"
#include "../Include/impl/mp_manager.h"
#include <time.h>
#include "../Include/impl/system_section.h"
#include "../Include/impl/thread_synch_commands.h"
#include "../include/impl/mutex.h"

#include <map> 
#include <deque>
#include "../Include/Log.h"
#define HIBERNATE_SECONDS 0

bool ServiceMessage(thread* pThread,const message& m, const section_map* pMap) 
{
	static long thread_synch_id = GetID<CCommand_ThreadSync_SectionQuit>( (CCommand_ThreadSync_SectionQuit*)NULL);
	static long start_section_synch_id = GetID<CCommand_ThreadSync_SectionStart>( (CCommand_ThreadSync_SectionStart*)NULL);
	/*if (m.m_type == start_section_synch_id) 
	{
		section* pSystemSection = pMap->find(SYSTEM_SECTION_ID)->second;
		
		CCommand_ThreadSync_SectionStart* pCommand = (CCommand_ThreadSync_SectionStart*)m.pCommand;

		CCommand_ThreadSync_SectionStartResponse* command = new ((CCommand_ThreadSync_SectionStartResponse*) pThread->m_thread_allocator.allocate(sizeof(CCommand_ThreadSync_SectionStartResponse ))) 
																		CCommand_ThreadSync_SectionStartResponse();
		command->src_id = pCommand->src_id;
		command->m_psection = pCommand->m_psection;
		command->m_Magic= pCommand->m_Magic;
		//command->m_thread_id = pCommand->m_thread_id;
		m.pAllocator->Decrease();

		message newMessage;
		newMessage.m_destination = SYSTEM_SECTION_ID;
		newMessage.m_source = -1;
		newMessage.m_type = GetID<CCommand_ThreadSync_SectionStartResponse>( (CCommand_ThreadSync_SectionStartResponse*) NULL);
		newMessage.pAllocator = pThread->m_thread_allocator.get_chunk();
		newMessage.pCommand = command;
		
		pThread->schedule(newMessage, pSystemSection);
		
		return true;

	}
	else*/
	if (m.m_type == thread_synch_id ) 
	{
		section* pSystemSection = pMap->find(SYSTEM_SECTION_ID)->second;
		
		CCommand_ThreadSync_SectionQuit* pCommand = (CCommand_ThreadSync_SectionQuit*)m.pCommand;

		CCommand_ThreadSync_SectionQuitResponse* command = new ((CCommand_ThreadSync_SectionQuitResponse*) pThread->m_thread_allocator.allocate(sizeof(CCommand_ThreadSync_SectionQuitResponse ))) 
																		CCommand_ThreadSync_SectionQuitResponse();
		command->m_psection = pCommand->m_psection;
		command->m_thread_id = pCommand->m_thread_id;
		m.pAllocator->Decrease();

		message newMessage;
		newMessage.m_destination = SYSTEM_SECTION_ID;
		newMessage.m_source = -1;
		newMessage.m_type = GetID<CCommand_ThreadSync_SectionQuitResponse>( (CCommand_ThreadSync_SectionQuitResponse*) NULL);
		newMessage.pAllocator = pThread->m_thread_allocator.get_chunk();
		newMessage.pCommand = command;
		pSystemSection->m_psection_queue->enqueue(newMessage, pThread->m_thread_allocator);
		//pThread->schedule(newMessage, pSystemSection);
		return true;
	}
	return false;
}

int HasSomethingToExecute(thread* pThread, int index) 
{
	int res = 0;
	const lurking_sections* pLurkers = pThread->m_lurking_sections_subscriber.get_property();
	if (!pLurkers->size() ) 
	{
		return false;
	}

	const section_map* pMap = pThread->m_section_map_subscriber.get_property();
	int i =0;
	for (i = index ; i < pLurkers->size() ; i ++ ) 
	{
		section* pLurker = (*pLurkers)[i];
		assert(pLurker->GetThisID() != SYSTEM_SECTION_ID);
		//section_map::const_iterator it =  pMap->find(  lurk_id);
		//if (it == pMap->end() ) 
		//{
		//	continue;
	//	}
		section* sec= pLurker;
		
		volatile long runflag = 0;
		if (!sec->m_running_flag.get())
		{
			continue;
		}
		volatile long busy_flag = 0;
		InterlockedExchange(&busy_flag,sec->m_busy_flag);
		if (busy_flag)
		{
			////CLog::Println("Checking Queue of %s" , sec->GetName().c_str() );
			continue;
		}
		if (sec->m_psection_queue.get()->m_size.get() > 0)
		{
			res ++;
		}
		//if (res ) 
		//	break;
	}
	if (!res) 
	{
		for (i = 0 ; i <index && i < pLurkers->size() ; i ++ ) 
		{

			section* pLurker = (*pLurkers)[i];
			assert(pLurker->GetThisID() != SYSTEM_SECTION_ID);
			//section_map::const_iterator it =  pMap->find(  lurk_id);
			//if (it == pMap->end() ) 
			//{
			//	continue;
		//	}
			section* sec= pLurker;
			
			volatile long runflag = 0;
			if (!sec->m_running_flag.get())
			{
				continue;
			}
			volatile long busy_flag = 0;
			InterlockedExchange(&busy_flag,sec->m_busy_flag);
			if (busy_flag)
			{
				continue;
			}
			if (sec->m_psection_queue.get()->m_size.get() > 0)
			{
				res ++;
			}

		}

	}
    return res;
}


bool FindSomethingToExecute(thread* pThread,int& index, message& m, section* sec)
{
	bool res = false;
	const lurking_sections* pLurkers = pThread->m_lurking_sections_subscriber.get_property();
	if (!pLurkers->size() ) 
	{
		return false;
	}

	const section_map* pMap = pThread->m_section_map_subscriber.get_property();
	int i =0;
	for (i = index ; i < pLurkers->size() ; i ++ ) 
	{
		section* pLurker = (*pLurkers)[i];
		assert(pLurker->GetThisID() != SYSTEM_SECTION_ID);
		//section_map::const_iterator it =  pMap->find(  lurk_id);
		//if (it == pMap->end() ) 
		//{
		//	continue;
	//	}
		section* sec= pLurker;
		
		volatile long runflag = 0;
		if (!sec->m_running_flag.get())
		{
			continue;
		}
		//CLog::Println("Checking Queue of %d" , i );
		if (!sec->m_psection_queue.get()->m_size.get())
		{
			//CLog::Println("Queue of %s empty" , sec->GetName().c_str() );
			continue;
		}
/*		if (sec->m_psection_queue.get()->m_size.get() > 100) 
		{
			assert(0);
		}*/
		if (InterlockedCompareExchange(&sec->m_busy_flag,pThread->m_id, 0 ))
		{
			continue;
		}
		res = sec->m_psection_queue.get()->dequeue(m);
		if (res ) {
			i++;
			//CLog::Println("Found Something for %s" , sec->GetName().c_str() );
			break;
		}
		else
			InterlockedExchange(&sec->m_busy_flag, 0 );
	}
	if (!res) 
	{
		for (i = 0 ; i <=index && i < pLurkers->size() ; i ++ ) 
		{

			section* pLurker = (*pLurkers)[i];
			assert(pLurker->GetThisID() != SYSTEM_SECTION_ID);
			//section_map::const_iterator it =  pMap->find(  lurk_id);
			//if (it == pMap->end() ) 
			//{
			//	continue;
		//	}
			section* sec= pLurker;
			
			volatile long runflag = 0;
			if (!sec->m_running_flag.get())
			{
				continue;
			}
			//CLog::Println("Checking Queue of %d" , i );
			if (!sec->m_psection_queue.get()->m_size.get())
			{
				//CLog::Println("Queue of %s empty" , sec->GetName().c_str() );
				continue;
			}
		/*	if (sec->m_psection_queue.get()->m_size.get() > 100) 
			{
				assert(0);
			}*/


			if (InterlockedCompareExchange(&sec->m_busy_flag,pThread->m_id, 0 ))
			{
				continue;
			}
			res = sec->m_psection_queue.get()->dequeue(m);
			if (res ) {
				i++;
				//CLog::Println("Found Something for %s" , sec->GetName().c_str() );
				break;
			}
			else
				InterlockedExchange(&sec->m_busy_flag, 0 );

		}

	}
	index = i;
    return res;
}

void KickThread(thread* t ) 
{
	SetEvent( t->m_handle);
}


/***
  main thread procedure
 */
void ThreadProc(void* pArgs)
{
	
	thread* pThread = (thread*)pArgs;
	long idle = clock();
	message newMessage;
	int index = 0;
	
	while (true) 
	{
		
	
		bool res = false;
		bool bNeedReadQueue = true;
		section* tmpSection = NULL;
		if (InterlockedCompareExchange(&pThread->m_stopped, 1 ,1 ) ) 
		{
			CloseHandle( pThread->m_handle);
			delete pThread;
			return;
		}
		InterlockedExchange(&pThread->m_searching, 1 );

		volatile section* pExclusive = NULL;
		InterlockedExchangePointer(&pExclusive, pThread->m_pexclusive_section);
		if (pExclusive) 
		{
			const section* p = (const section*)pExclusive;
			if (p->m_psection_queue->m_size.get())
			{
				InterlockedExchange(&pExclusive->m_busy_flag,1);
				res = p->m_psection_queue->dequeue(newMessage);
			}
		}
		else
		{
			
			res = FindSomethingToExecute(pThread, index , newMessage, tmpSection);
			if (res) 
			{
				std::vector<int> sleepers;
				int NeedMoreThreads = HasSomethingToExecute(pThread, index);
				const std::vector<thread*>& threads = mp_manager::get_instance()->get_threads();
				if (NeedMoreThreads > 0 ) 
				{
					for (int i = 0 ; i < threads.size() ; i ++ ) 
					{
						if (!InterlockedCompareExchangePointer((volatile PVOID*)&threads[i]->m_pexclusive_section, NULL, NULL ))
						{
							volatile long sleep_flag = 0;
							InterlockedExchange(&sleep_flag,threads[i]->m_sleeping); 
							if (sleep_flag )
							{
								sleepers.push_back(i);
								//KickThread(threads[i]);
								//break;
							}
						}
					}
					int ActiveThreads = threads.size() - sleepers.size();

					if (ActiveThreads < NeedMoreThreads) 
					{
						for (int i = 0 ; i < min(sleepers.size() , NeedMoreThreads - ActiveThreads) ; i ++ ) 
						{
							//printf("Need moar thread");
							KickThread( threads[ sleepers[i] ] );
						}
					}
				}
			}
			

		}
		InterlockedExchange(&pThread->m_searching, 0 );

		
		if (res ) 
		{
				mp_manager::get_instance()->m_active_commands.dec();
				const section_map* pMap = pThread->m_section_map_subscriber.get_property(); // we have read section map
				if (!pMap) 
				{
					//pThread->m_thread_queue.enqueue(newMessage, pThread->m_thread_allocator);
					//InterlockedExchange(&pSection->m_busy_flag,0);
					continue;
				}

				section_map::const_iterator it = pMap->find(newMessage.m_destination);
				if (it != pMap->end() )  // section is present
				{
					section* pSection = it->second;
					// section is bound to quit
				
					ConformFunc func = pSection->GetReaction(newMessage.m_type);
					if (func) 
					{
						pSection->setThread(pThread);
						(*func)(pSection, newMessage.m_source,newMessage.pCommand);
						pSection->setThread(NULL);
						
					}
					else
					{
						// alarm and put back on queue
						//printf("HUITA");

					}
					//pThread->m_thread_queue.m_size.dec();
					
					idle  = clock();
					pThread->m_stats.m_wait_time += (idle- newMessage.m_clock);
					pThread->m_stats.m_wait_time = pThread->m_stats.m_wait_time/2;
					//pThread->m_stat_publisher.set_value(pThread->m_stats);
					newMessage.pAllocator->Decrease();
					
					/**
						dummy cycle to update subscription status
					*/
					InterlockedExchange(&pSection->m_busy_flag,0);
				}
				else // section not present. push back on queue
					// alarm and push back on queue
				{
					//pThread->m_thread_queue.enqueue(newMessage, pThread->m_thread_allocator);
					newMessage.pAllocator->Decrease();
					// release busy flag
					if (tmpSection) 
					{
						InterlockedExchange( &tmpSection->m_busy_flag, 0);
					}
				}
				//FilterMessages(pThread, accepted_messages);
				if (pExclusive) InterlockedExchange(&pExclusive->m_busy_flag,0);
				
		}
		else
		{
			if (pExclusive) InterlockedExchange(&pExclusive->m_busy_flag,0);
			InterlockedExchange( &pThread->m_sleeping , 1);
			//CLog::Print("Thread %d Hibernates\n" , pThread->m_id);
			long diff = clock() - idle;
			if (diff/CLOCKS_PER_SEC > HIBERNATE_SECONDS ) 
			{
					IdleProc pProc = NULL;
					section* pSection = NULL;
					InterlockedExchangePointer(&pSection, pThread->m_pexclusive_section);
					InterlockedExchangePointer(&pProc, pThread->m_pIdleProc);

				if (pSection && pProc ) 
				{
				
					pSection->setThread(pThread);
					(*pProc)(pThread,pSection);
					//pSection->setThread(NULL);// dont set thread after
					//InterlockedExchange(&pThread->m_sleeping, 0);
				}
				else
				{
					//InterlockedExchange( &pThread->m_sleeping , 1);
					WaitForSingleObject(pThread->m_handle, INFINITE); // wait
					ResetEvent(pThread->m_handle); // reset event just in case.
			
				}
			}
			//CLog::Print("Thread %d Wakes up\n" , pThread->m_id);
			InterlockedExchange(&pThread->m_sleeping, 0);
		}
	}

};