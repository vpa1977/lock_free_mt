#ifndef SHARED_VARIABLES_HPP_INCLUDED
#define SHARED_VARIABLES_HPP_INCLUDED

#include "atomic.h"

#include <boost/utility.hpp>
#include <assert.h>

// default deleter
template<typename T>
void DefaultDeleterFunc(T* ptr)
{
	if(ptr)
		delete ptr;
}

// forward declarations
template<typename T> struct StoredType;
template<typename T> class Handle;

/*    
*    
*    lock-free
*/
template<typename T>
class Store : public boost::noncopyable
{
public:
	typedef Handle<T> Handle;

	typedef void(*DeleterType)(T*);

	//explicit Store(T* obj);
	explicit Store(T* obj, DeleterType deleter = DefaultDeleterFunc<T> ) : m_CountedPtr((__int64)obj), m_Deleter(deleter)
	{
		assert(obj);
		assert(m_Deleter);
	}

	Handle GetHandle() const
	{
		__int64 ret = atomic::ExchangeAdd(&m_CountedPtr, 0x0000000100000000LL);
		assert( (StoredType<T>*)ret );
		return Handle((StoredType<T>*)ret);
	}

	/*
		Set new object to own.
		Old one is deleted immediately or when last Handle is destroyed.
	*/
	void ResetObject(T* obj)
	{
		assert(obj);
		__int64 dest = (__int64)(unsigned)StoredType<T>::Create(obj, m_Deleter);
		dest = atomic::Exchange(&m_CountedPtr, dest);
		StoredType<T>* pDest = (StoredType<T>*)dest;
		if(pDest)
			pDest->Remove( *(reinterpret_cast<int*>(&dest) + 1) );
	}

	~Store()
	{
		__int64 dest = atomic::Exchange( &m_CountedPtr, (__int64)0 );
		StoredType<T>* pDest = (StoredType<T>*)dest;
		assert(pDest);
		pDest->Remove( *(reinterpret_cast<int*>(&dest) + 1) );
	}

private:
	DeleterType m_Deleter;

	/*	Readers counter + object pointer.
		High dword - counter,
		Low dword - pointer. */
	volatile mutable __int64 m_CountedPtr;
};












/**********************************************************************************
	RAII-wrapper over the object pointer that you get from the storage.
	Calls Decrement() for object, when destroyed.
	Shares object like shared_ptr.
***********************************************************************************/
template<typename T>
class Handle
{
public:
	/*	Get access to stored object.
		You cannot change the object because multiple threads can access the object simultaneously.
		You cant use the pointer beyong Handle's lifetime.
	*/
	operator const T* () const
	{
		assert(m_Ptr);
		return (const T*)(m_Ptr->Get());
	}

	Handle(Handle& other)
	{
		m_Ptr = other.m_Ptr;
		m_Ptr->Increment();
	}

	Handle& operator = (Handle& other)
	{
		if(m_Ptr)
			m_Ptr->Decrement();
		m_Ptr = other.m_Ptr;
		m_Ptr->Increment();
	}

	~Handle()
	{
		if(m_Ptr)
			m_Ptr->Decrement();
	}

private:
	volatile StoredType<T>* m_Ptr;

	explicit Handle(volatile StoredType<T>* ptr) : m_Ptr(ptr)
	{
		assert(m_Ptr);
	}

	/* Store calls constructor */
	friend class Store<T>;
};










/**********************************************************************************
	Wrapper over the stored object.
	Contains reference counter.
***********************************************************************************/
template<typename T>
struct StoredType : public boost::noncopyable
{
	T* m_pObject;

	/*	Reference counter + flag indicating that object was removed from the storage.
		High dword - counter,
		Low dword - flag. */
	volatile __int64 m_CountRemoved;

	typedef void(*DeleterType)(T*);

	/**    object "remover" */
	DeleterType m_Deleter;

	StoredType(T* obj, DeleterType deleter) :
		m_pObject(obj)
		,m_Deleter(deleter)
		,m_CountRemoved( 0x0000000000000000LL ) // Count==0, Removed==0
	{
		assert(m_pObject);
		assert(m_Deleter);
	}

	static StoredType* Create(T* obj, DeleterType deleter)
	{
		try
		{
			return new StoredType(obj, deleter);
		}
		catch (...)
		{
			deleter(obj);
			throw;
		}
	}

	~StoredType()
	{
		m_Deleter(m_pObject);
	}

	void* Get() volatile
	{
		return m_pObject;
	}

	void Decrement() volatile
	{
		Check(atomic::ExchangeAdd(&m_CountRemoved, -0x0000000100000000LL) - 0x0000000100000000LL);
	}

	void Increment() volatile
	{
		atomic::ExchangeAdd(&m_CountRemoved, 0x0000000100000000LL);
	}

	/*
		Object removal processing.
		Called when object in storage is replaced with the new one.
		count - counter next to object pointer, at the time of replacement.
	*/
	void Remove(int count) volatile
	{
		__int64 val = ((__int64)count << 32) + 1;
		Check( atomic::ExchangeAdd(&m_CountRemoved, val) + val );
	}

	// check if object must be deleted
	void Check(__int64 value) volatile
	{
		if( !*(reinterpret_cast<int*>(&value) + 1)
			&& (int)value
		)
		{
			delete this;
		}
	}
};

#endif
