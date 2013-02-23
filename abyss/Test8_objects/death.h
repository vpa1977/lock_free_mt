#ifndef DEATH_HPP_INCLUDED
#define DEATH_HPP_INCLUDED

#include "link.h"
#include <stdio.h>

class CDeathNotificatorBase
{
protected:
	virtual ~CDeathNotificatorBase() {}
	void CallDeathNotification( CLinkTarget* in_pTarget ) {
		in_pTarget->NotificatorDied( this );
	}
};

template< size_t N >
class CDeathNotificator : public CDeathNotificatorBase, public CLinkArray<N>
{
public:
	CDeathNotificator() {
		printf("CDeathNotificator(%p)::CDeathNotificator()\n",this);
	}
	virtual ~CDeathNotificator() {
		for( size_t i=0; i<GetLinksSize(); ++i ) {
			CallDeathNotification( GetNthLink(i) );
		}
		printf("CDeathNotificator(%p)::~CDeathNotificator()\n",this);
	}
protected:
	void vLinkConnected( CLinkTarget* in_pTarget ) {
		printf("CDeathNotificator(%p)::vLinkConnected( %p )\n",this,in_pTarget);
	}
	void vLinkDisconnected( CLinkTarget* in_pTarget ) {
		printf("CDeathNotificator(%p)::vLinkDisconnected( %p )\n",this,in_pTarget);
	}
};


/*
template< size_t N >
class CDeathNotificator :
	public CLinkArray<N>
{
public:
	CDeathNotificator( bool in_bNotifyTargetsAboutMyDeath, bool in_bAllowedToDieOnlyWhenNotLinkedTo ) :
		CLinkArray<N>(),
		m_bNotifyTargets(in_bNotifyTargetsAboutMyDeath),
		m_bDieOnlyNotLinked(in_bAllowedToDieOnlyWhenNotLinkedTo)
	{
		// TODO
	}
	virtual ~CDeathNotificator() {
		if(m_bDieOnlyNotLinked) {
			assert( 0==GetLinksSize() );
		}
		if(m_bNotifyTargets) {
			for( size_t i=0; i<GetLinksSize(); ++i ) {
				CLink* pLink = GetNthLink(i);
				assert(pLink);
				DisconnectAndNotifyDeath(this,pLink);
			}
		}
	}
private:
	bool m_bNotifyTargets;
	bool m_bDieOnlyNotLinked;
};
*/

#endif



















