#ifndef PUBLISHERS_HPP 

#define PUBLISHERS_HPP
#include "stdafx.h"
#include "allocator.h"
#include <vector>

// this is an event subscriber. 
// it should be used by one single thread. 
// 
template <class T> 
struct subscriber 
{
	subscriber() : m_version(0) , m_active_version(0)
	{
	}
	// called from external thread
	inline void set_property(volatile T* new_value, volatile long new_version)
	{
		InterlockedExchangePointer((PVOID volatile*)&m_external_pointer, (PVOID)new_value);
		InterlockedExchange(&m_version, new_version );
		
	}
	inline long get_version() 
	{
		volatile long version;
		InterlockedExchange(&version, m_active_version);
		return version;
	}
	inline const T* get_property() const
	{
		volatile T* tmp_ptr;
		InterlockedExchangePointer(&tmp_ptr, m_external_pointer);
		if (tmp_ptr != m_internal_pointer) 
		{
			InterlockedExchangePointer(&m_internal_pointer, tmp_ptr);
			InterlockedExchange((volatile long*)&m_active_version,m_version);
		}
		const T* result = (const T*)m_internal_pointer;
		return result;
	}
private:	
	volatile T* m_external_pointer;
	volatile T* m_internal_pointer;
	volatile LONG m_version;
	volatile LONG m_active_version;
};

//
template <class T> 
struct list_entry
{
	long version;
	T* ptr;
	chunk* m_chunk;
	chunk* m_ptr_chunk;
	list_entry<T> * next;
};
// this class is updated only in one thread. 
template <class T> 
struct publisher : public chain_allocator
{
	publisher() : chain_allocator(16*1024)
	{
		m_head = NULL;
		m_tail = NULL;
		m_version = 0;

	}
	void set_value(const T& in)
	{
		// clean up unused copies.
		long max_version = 0;
		m_version ++;
		T* newPtr = new ((T*)allocate(sizeof(T))) T(in);
		chunk* p = get_chunk();
		if (!m_head ) 
		{
			m_head  = new ((list_entry<T>*)allocate(sizeof(list_entry<T>))) list_entry<T>();
			m_head->m_chunk = get_chunk();
			m_head->m_ptr_chunk = p;
			m_tail = m_head;
			m_head->ptr = newPtr;
			m_head->version = m_version;
			m_head->next = NULL;
		}
		else
		{
			list_entry<T>* new_tail = new ((list_entry<T>*)allocate(sizeof(list_entry<T>))) list_entry<T>();
			new_tail->m_chunk = get_chunk();
			new_tail->ptr = newPtr;
			new_tail->m_ptr_chunk = p;
			new_tail->version = m_version;
			new_tail->next = NULL;
			m_tail->next = new_tail;
			m_tail = new_tail;
		}
		//m_values.push_back(newPtr);
		
		for (unsigned int i = 0 ; i < m_listeners.size() ; i ++ ) 
		{
			long v = m_listeners[i]->get_version();
			if (v >max_version ) 
			{
				max_version =v;
			}
			m_listeners[i]->set_property(newPtr, m_version);
		}
		// purge unused copies
		while (m_head->next && m_head->version < max_version) 
		{
			list_entry<T>* to_kill = m_head;
			to_kill->m_chunk->Decrease();
			to_kill->m_ptr_chunk->Decrease();
			m_head = m_head->next;
		}

	}
	void add_listener(subscriber<T>* t) 
	{
		m_listeners.push_back(t);
		if (m_values.size())
		t->set_property(m_values[ m_values.size() -1 ], m_version );
	}

private:
	long m_version;
	list_entry<T>* m_head;
	list_entry<T>* m_tail;
	std::vector<subscriber<T>* > m_listeners;
	std::vector<T*> m_values;
};


#endif 