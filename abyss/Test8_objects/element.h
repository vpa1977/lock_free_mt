#ifndef ELEMENT_HPP_INCLUDED
#define ELEMENT_HPP_INCLUDED

#include "death.h"
#include <assert.h>
#include <stdio.h>

class CElementOwner
{
	template< typename T > friend class CElementHandle;
public:
	CElementOwner() : m_Counter() {}
	~CElementOwner() {
		assert( !IsBeingChangedRightNow() );
	}
	bool IsBeingChangedRightNow() const { return 0!=m_Counter; }
private:
	void ElementAccessBegin() {
		m_Counter++;
	}
	void ElementAccessEnd() {
		assert(m_Counter>0);
		m_Counter--;
	}
	long m_Counter;
};

template< typename T, size_t N > class CElementWrapper;

template< typename T >
class CElementHandle
{
	template< typename T, size_t N > friend class CElementWrapper;
public:
	CElementHandle( const CElementHandle& in_rHandle ) :
		m_rValue(in_rHandle.m_rValue)
		,m_pWrapper(in_rHandle.m_pWrapper)
		,m_pWrapperOwner(in_rHandle.m_pWrapperOwner)
	{
		assert(m_pWrapper);
		m_pWrapper->ElementAccessBegin();
		assert(m_pWrapperOwner);
		m_pWrapperOwner->ElementAccessBegin();
	}
	~CElementHandle() {
		assert(m_pWrapper);
		m_pWrapper->ElementAccessEnd();
		assert(m_pWrapperOwner);
		m_pWrapperOwner->ElementAccessEnd();
	}
	operator T& () { return m_rValue; }
	
private:
	CElementHandle( T& in_rValue, CElementOwner* in_pWrapper, CElementOwner* in_pWrapperOwner ) :
		m_rValue(in_rValue)
		,m_pWrapper(in_pWrapper)
		,m_pWrapperOwner(in_pWrapperOwner)
	{
		assert(m_pWrapper);
		m_pWrapper->ElementAccessBegin();
		assert(m_pWrapperOwner);
		m_pWrapperOwner->ElementAccessBegin();
	}
	T& m_rValue;
	CElementOwner* m_pWrapper;
	CElementOwner* m_pWrapperOwner;
};

class CElementWrapperBase
{
protected:
	virtual ~CElementWrapperBase() {}
	void CallKick( CLinkTarget* in_pTarget ) {
		in_pTarget->KickMe();
	}
};

template< typename T, size_t N >
class CElementWrapper : public CElementWrapperBase, public CElementOwner, public CDeathNotificator<N>
{
public:
	CElementWrapper() : m_pOwner(NULL) { printf("CElementWrapper(%p)::CElementWrapper()\n",this); }
	CElementWrapper( const T& in_rInit ) : m_Value(in_rInit), m_pOwner(NULL) { printf("CElementWrapper(%p)::CElementWrapper(...)\n",this); }
	~CElementWrapper() {
		printf("CElementWrapper(%p)::~CElementWrapper()\n",this);
	}
	void Init(CElementOwner* in_pOwner) {
		assert(!m_pOwner);
		assert(in_pOwner);
		m_pOwner = in_pOwner;
	}
	//
	CElementHandle<T> Get() {
		assert(m_pOwner);
		for( size_t i=0; i<GetLinksSize(); ++i ) {
			CallKick( GetNthLink(i) );
		}
		return CElementHandle<T>(m_Value,this,m_pOwner);
	}
	operator const T& () { return m_Value; }
protected:
	void vLinkConnected( CLinkTarget* in_pTarget ) {
	}
	void vLinkDisconnected( CLinkTarget* in_pTarget ) {
	}
private:
	T m_Value;
	CElementOwner* m_pOwner;
};




#endif

























