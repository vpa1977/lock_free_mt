#ifndef ELEMENT_HPP_INCLUDED
#define ELEMENT_HPP_INCLUDED

#include "death.h"
#include "shadow_tracker.h"
#include <assert.h>
#include "../../MT_Core/Include/log.h"

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
	T& operator =(const T& t) { m_rValue = t; return m_rValue;};

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
class CElementWrapper :
	public CElementWrapperBase
	,public CElementOwner
	,public CLinkArray<N>
	,public CLinkTarget
{
public:
	CElementWrapper() : m_pOwner(NULL) {
		//CLog::Print("CElementWrapper(%p)::CElementWrapper() CLinkOrigin(%p) CLinkTarget(%p)\n",this,(CLinkOrigin*)this,(CLinkTarget*)this);
	}
	template< class K >
	CElementWrapper( const K& in_rInit ) : m_Value(in_rInit), m_pOwner(NULL) {
		//CLog::Print("CElementWrapper(%p)::CElementWrapper(...)\n",this);
	}
	~CElementWrapper() {
		//CLog::Print("CElementWrapper(%p)::~CElementWrapper()\n",this);
		assert( m_ShadowTracker.NoShadowsAlive() );
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
	operator const T& () const { return m_Value; }

// virtual from CLinkArray
protected:
	void vLinkConnected( CLinkTarget* in_pTarget ) {
		CLinkOrigin* pOrigin = dynamic_cast<CLinkOrigin*>(in_pTarget);
		//CLog::Print("CElementWrapper(%p)::vLinkConnected( %p )\n",this,pOrigin);
		m_ShadowTracker.Connected( in_pTarget, pOrigin );
	}
	void vLinkDisconnected( CLinkTarget* in_pTarget ) {
		CLinkOrigin* pOrigin = dynamic_cast<CLinkOrigin*>(in_pTarget);
		//CLog::Print("CElementWrapper(%p)::vLinkDisconnected( %p )\n",this,pOrigin);
		assert( false );
	}

// virtual from CLinkTarget
protected:
	void SomethingLinkedToYou( CLinkOrigin* in_pOrigin ) {
		//CLog::Print("CElementWrapper(%p)::SomethingLinkedToYou( %p )\n",this,in_pOrigin);
		m_ShadowTracker.Connected( dynamic_cast<CLinkTarget*>(in_pOrigin), in_pOrigin );
	}
	void SomethingUnlinkedFromYou( CLinkOrigin* in_pOrigin ) {
		//CLog::Print("CElementWrapper(%p)::SomethingUnlinkedFromYou( %p )\n",this,in_pOrigin);
		CLinkTarget* in_pTarget = m_ShadowTracker.ShadowDisconnected( in_pOrigin );
		if( in_pTarget ) {
			DisconnectLink( in_pTarget, false );
		}
	}
	void NotificatorDied( CLinkOrigin* in_pOrigin ) {
		//CLog::Print("CElementWrapper(%p)::NotificatorDied( %p )\n",this,in_pOrigin);
		CLinkTarget* in_pTarget = m_ShadowTracker.ShadowDied( in_pOrigin );
		if( in_pTarget ) {
			DisconnectLink( in_pTarget, false );
		}
	}
	void KickMe() {
		//CLog::Print("CElementWrapper(%p)::KickMe()\n",this);
	}

private:
	T m_Value;
	CElementOwner* m_pOwner;
	CTShadowTracker<N> m_ShadowTracker;
};




#endif

























