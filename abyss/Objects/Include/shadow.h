#ifndef SHADOW_HPP_INCLUDED
#define SHADOW_HPP_INCLUDED

#include "object.h"
#include "entity_types.h"
#include "../../Render_DX8/Include/algebra.h"
#include <vector>
#include <assert.h>

//
// Shadow of CElementWrapper<T,N>
// 
template< typename T >
class CElementShadow : public CDeathNotificator<1>, public CLinkTarget {
public:
	CElementShadow() : m_bDirty(true) {
	}
	CElementShadow( const T& in_rValue ) : m_Value(in_rValue) {
	}
	~CElementShadow() {
	}
	const T& Get() const { return m_Value; }
	CElementShadow<T>& operator = ( const T& in_rValue ) { m_Value = in_rValue; m_bDirty = false; return *this; }
	bool IsDirty() const { return m_bDirty; }
protected:
	void SomethingLinkedToYou( CLinkOrigin* in_pOrigin ) {}
	void SomethingUnlinkedFromYou( CLinkOrigin* in_pOrigin ) {}
	void NotificatorDied( CLinkOrigin* in_pOrigin ) {}
	void KickMe() {
		m_bDirty = true;
	}
private:
	T m_Value;
	bool m_bDirty;
};


struct CShadow {
public:
	virtual unsigned long GetType() const = 0;
	CElementShadow<CMatrix> m_Position;
	std::vector<CShadow*> m_Children;
protected:
	virtual ~CShadow() {}
	friend class CShadowCreator;
};


class CShadowCreator : public virtual boost::noncopyable
{
public:
	virtual ~CShadowCreator() {}
	virtual CShadow* CreateShadow( CObject* ) = 0;
	virtual void ReclaimShadow( CShadow* ) = 0;
	virtual void DeleteAllShadowsForObject( CObject* ) = 0;
protected:
	void DeleteShadow( CShadow* in_pShadow ) { delete in_pShadow; }
};

#endif











