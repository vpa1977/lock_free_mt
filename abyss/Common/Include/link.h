#ifndef LINK_HPP_INCLUDED
#define LINK_HPP_INCLUDED

#include "boost/array.hpp"
#include "boost/utility.hpp"
#include <assert.h>

class CLinkOrigin;
class CDeathNotificatorBase;
class CElementWrapperBase;

class CLinkTarget : public virtual boost::noncopyable
{
	friend class CLinkOrigin;
	friend class CDeathNotificatorBase;
	friend class CElementWrapperBase;
public:
	virtual ~CLinkTarget() {}
protected:
	virtual void SomethingLinkedToYou( CLinkOrigin* ) = 0;
	virtual void SomethingUnlinkedFromYou( CLinkOrigin* ) = 0;
	// dirty hack
	virtual void NotificatorDied( CLinkOrigin* ) = 0;
	// another dirty hack
	virtual void KickMe() = 0;
};

class CLinkOrigin : public virtual boost::noncopyable
{
public:
	virtual ~CLinkOrigin() {}
	virtual void ConnectLink( CLinkTarget* in_pTarget ) = 0;
	virtual void DisconnectLink( CLinkTarget* in_pTarget, bool in_bNotify ) = 0;
protected:
	virtual size_t GetLinksSize() const = 0;
	virtual CLinkTarget* GetNthLink( size_t in_N ) const = 0;
	void CallLinkNotification( CLinkTarget* in_pTarget ) {
		in_pTarget->SomethingLinkedToYou( this );
	}
	void CallUnlinkNotification( CLinkTarget* in_pTarget ) {
		in_pTarget->SomethingUnlinkedFromYou( this );
	}
};

template< size_t N >
class CLinkArray : public CLinkOrigin
{
public:
	void ConnectLink( CLinkTarget* in_pTarget ) {
		assert(m_NFilled<N);
		for(size_t i=0; i<m_NFilled; ++i) {
			assert( m_Links[i] != in_pTarget );
		}
		m_Links[m_NFilled] = in_pTarget;
		m_NFilled++;
		CallLinkNotification(in_pTarget);
		vLinkConnected(in_pTarget);
	}
	void DisconnectLink( CLinkTarget* in_pTarget, bool in_bNotify=true ) {
		assert(m_NFilled>0);
		size_t i = 0;
		for( ; i<m_NFilled; ++i) {
			if(m_Links[i] == in_pTarget)
				break;
		}
		bool bLinkFound = i<m_NFilled;
		assert( bLinkFound );
		for( ; i+1<m_NFilled; ++i ) {
			m_Links[i] = m_Links[i+1];
		}
		m_Links[i] = NULL;
		m_NFilled--;
		if(in_bNotify) {
			CallUnlinkNotification(in_pTarget);
			vLinkDisconnected(in_pTarget);
		}
	}
protected:
	CLinkArray() : m_NFilled(0) {}
	virtual ~CLinkArray() {
		// disconnect all
		while( GetLinksSize() ) {
			DisconnectLink( GetNthLink(0) );
		}
		assert( 0 == m_NFilled );
	}
	size_t GetLinksSize() const { return m_NFilled; }
	CLinkTarget* GetNthLink( size_t in_N ) const { assert(in_N<m_NFilled); return m_Links[in_N]; }
	virtual void vLinkConnected( CLinkTarget* in_pTarget ) {}
	virtual void vLinkDisconnected( CLinkTarget* in_pTarget ) {}
private:
	boost::array<CLinkTarget*,N> m_Links;
	size_t m_NFilled;
};

/*
class CLink : public boost::noncopyable
{
	friend void ConnectLinks( CLink* in_pL1, CLink* in_pL2 );
	friend void DisconnectAndNotifyDeath( CLink* in_pThis, CLink* in_pTarget );
protected:
	virtual void LinkDied( CLink* in_pOther ) = 0;
	virtual void LinkConnected( CLink* in_pOther ) = 0;
	virtual void LinkDisconnected( CLink* in_pOther ) = 0;
	virtual size_t GetLinksSize() const = 0;
	virtual CLink* GetNthLink( size_t in_N ) const = 0;
private:
	virtual void ConnectLink( CLink* in_pOther ) = 0;
	virtual void DisconnectLink( CLink* in_pOther ) = 0;
};

template< size_t N >
class CLinkArray : public CLink
{
public:
	CLinkArray() : m_NFilled(0) {
		for(size_t i=0; i<N; ++i)
			m_Links[i] = NULL;
	}
protected:
	size_t GetLinksSize() const { return m_NFilled; }
	CLink* GetNthLink( size_t in_N ) const { assert(in_N<N); return m_Links[in_N]; }
private:
	void ConnectLink( CLink* in_pOther ) {
		assert(m_NFilled<N);
		for(size_t i=0; i<m_NFilled; ++i) {
			assert( m_Links[i] != in_pOther );
		}
		m_Links[m_NFilled] = in_pOther;
		m_NFilled++;
		LinkConnected(in_pOther);
	}
	void DisconnectLink( CLink* in_pOther ) {
		assert(m_NFilled>0);
		size_t i = 0;
		for( ; i<m_NFilled; ++i) {
			if(m_Links[i] == in_pOther)
				break;
		}
		bool bLinkFound = i<m_NFilled;
		assert( bLinkFound );
		for( ; i+1<m_NFilled; ++i ) {
			m_Links[i] = m_Links[i+1];
		}
		m_Links[i] = NULL;
		m_NFilled--;
		LinkDisconnected(in_pOther);
	}
	boost::array<CLink*,N> m_Links;
	size_t m_NFilled;
};

void ConnectLinks( CLink* in_pL1, CLink* in_pL2 )
{
	assert(in_pL1);
	assert(in_pL2);
	in_pL1->ConnectLink(in_pL2);
	in_pL2->ConnectLink(in_pL1);
}

void DisconnectAndNotifyDeath( CLink* in_pThis, CLink* in_pTarget )
{
	assert(in_pThis);
	assert(in_pTarget);
	in_pTarget->LinkDied( in_pThis );
	in_pTarget->DisconnectLink( in_pThis );
}
*/


#endif























