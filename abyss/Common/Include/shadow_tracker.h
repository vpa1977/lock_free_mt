#ifndef SHADOW_TRACKER_HPP_INCLUDED
#define SHADOW_TRACKER_HPP_INCLUDED

#include "link.h"
#include "boost/array.hpp"
#include <assert.h>

template< size_t N >
class CTShadowTracker {
public:
	CTShadowTracker() : m_NShadows(0) {}
	bool NoShadowsAlive() const {
		//return false;
		return 0==m_NShadows;
	}
	void Connected( CLinkTarget* in_pShadowAsTarget, CLinkOrigin* in_pShadowAsOrigin ) {
		assert(in_pShadowAsTarget);
		assert(in_pShadowAsOrigin);
		size_t i=0;
		for( ; i<m_NShadows; ++i ) {
			if( in_pShadowAsOrigin == m_ShadowStates[i].m_pShadow )
				break;
		}
		if( i>=m_NShadows ) {
			assert( i==m_NShadows );
			assert( m_NShadows < N );
			CShadowState& rNew( m_ShadowStates[m_NShadows] );
			rNew.m_pElement = in_pShadowAsTarget;
			rNew.m_pShadow = in_pShadowAsOrigin;
			rNew.m_bUnlinked = false;
			rNew.m_bDied = false;
			m_NShadows++;
		}
	}
	CLinkTarget* ShadowDisconnected( CLinkOrigin* in_pShadow ) {
		assert(in_pShadow);
		assert(m_NShadows>0);
		size_t i=0;
		for( ; i<m_NShadows; ++i ) {
			if( in_pShadow == m_ShadowStates[i].m_pShadow )
				break;
		}
		assert(i<m_NShadows);
		CShadowState& rFound( m_ShadowStates[i] );
		assert( rFound.m_bUnlinked == false );
		rFound.m_bUnlinked = true;
		if( rFound.m_bUnlinked && rFound.m_bDied ) {
			CLinkTarget* pRet = rFound.m_pElement;
			Remove(i);
			return pRet;
		}
		return NULL;
	}
	CLinkTarget* ShadowDied( CLinkOrigin* in_pShadow ) {
		assert(in_pShadow);
		assert(m_NShadows>0);
		size_t i=0;
		for( ; i<m_NShadows; ++i ) {
			if( in_pShadow == m_ShadowStates[i].m_pShadow )
				break;
		}
		assert(i<m_NShadows);
		CShadowState& rFound( m_ShadowStates[i] );
		assert( rFound.m_bDied == false );
		rFound.m_bDied = true;
		if( rFound.m_bUnlinked && rFound.m_bDied ) {
			CLinkTarget* pRet = rFound.m_pElement;
			Remove(i);
			return pRet;
		}
		return NULL;
	}
private:
	struct CShadowState {
		CLinkTarget* m_pElement;
		CLinkOrigin* m_pShadow;
		bool m_bUnlinked;
		bool m_bDied;
	};
	boost::array<CShadowState,N> m_ShadowStates;
	size_t m_NShadows;
	inline void Remove(size_t i) {
		assert(i<m_NShadows);
		CShadowState& rFound( m_ShadowStates[i] );
		assert( rFound.m_pShadow && rFound.m_bDied && rFound.m_bUnlinked );
		for( size_t Next=i+1; Next<m_NShadows; ++Next ) {
			m_ShadowStates[Next-1] = m_ShadowStates[Next];
		}
		m_NShadows--;
	}
};


template< size_t N >
class CTShadowTrackerSimple {
public:
	CTShadowTrackerSimple() : m_NShadows(0) {}
	bool NoShadowsAlive() const {
		//return false;
		return 0==m_NShadows;
	}
	void Connected( CLinkOrigin* in_pShadowAsOrigin ) {
		assert(in_pShadowAsOrigin);
		size_t i=0;
		for( ; i<m_NShadows; ++i ) {
			if( in_pShadowAsOrigin == m_ShadowStates[i].m_pShadow )
				break;
		}
		if( i>=m_NShadows ) {
			assert( i==m_NShadows );
			assert( m_NShadows < N );
			CShadowState& rNew( m_ShadowStates[m_NShadows] );
			rNew.m_pShadow = in_pShadowAsOrigin;
			rNew.m_bUnlinked = false;
			rNew.m_bDied = false;
			m_NShadows++;
		}
	}
	void ShadowDisconnected( CLinkOrigin* in_pShadow ) {
		assert(in_pShadow);
		assert(m_NShadows>0);
		size_t i=0;
		for( ; i<m_NShadows; ++i ) {
			if( in_pShadow == m_ShadowStates[i].m_pShadow )
				break;
		}
		assert(i<m_NShadows);
		CShadowState& rFound( m_ShadowStates[i] );
		assert( rFound.m_bUnlinked == false );
		rFound.m_bUnlinked = true;
		if( rFound.m_bUnlinked && rFound.m_bDied ) {
			Remove(i);
		}
	}
	void ShadowDied( CLinkOrigin* in_pShadow ) {
		assert(in_pShadow);
		assert(m_NShadows>0);
		size_t i=0;
		for( ; i<m_NShadows; ++i ) {
			if( in_pShadow == m_ShadowStates[i].m_pShadow )
				break;
		}
		assert(i<m_NShadows);
		CShadowState& rFound( m_ShadowStates[i] );
		assert( rFound.m_bDied == false );
		rFound.m_bDied = true;
		if( rFound.m_bUnlinked && rFound.m_bDied ) {
			Remove(i);
		}
	}
private:
	struct CShadowState {
		CLinkOrigin* m_pShadow;
		bool m_bUnlinked;
		bool m_bDied;
	};
	boost::array<CShadowState,N> m_ShadowStates;
	size_t m_NShadows;
	inline void Remove(size_t i) {
		assert(i<m_NShadows);
		CShadowState& rFound( m_ShadowStates[i] );
		assert( rFound.m_pShadow && rFound.m_bDied && rFound.m_bUnlinked );
		for( size_t Next=i+1; Next<m_NShadows; ++Next ) {
			m_ShadowStates[Next-1] = m_ShadowStates[Next];
		}
		m_NShadows--;
	}
};


#endif


