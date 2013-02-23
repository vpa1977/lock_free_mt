#ifndef OBJECTS_HPP_INCLUDED
#define OBJECTS_HPP_INCLUDED

#include "object_defines.h"
#include "entity.h"

#include "../../Common/Include/link.h"
#include "../../Common/Include/element.h"
#include "../../Common/Include/shadow_tracker.h"
#include "../../Common/Include/controller.h"

#include "../../Common/Include/box.h"

#include "../../Render_DX8/Include/algebra.h"

#include <vector>
#include <string>

class CObject :
	public CElementOwner			// has matrix
	,public CLinkTarget				// targeted by shadows
	,public CControllable< CObject >
{
public:
	CObject(	const std::string& in_Name
				,bool in_bEnabled
				,const CMatrix& in_Position
				,const std::vector<CObject*>& in_Children
				,CEntity* in_pEntity );
	~CObject();

	CObject(	const std::string& in_Name = ""
				,bool in_bEnabled = true
				,const CMatrix& in_Position = CMatrix()
	);
	void AssignInternals( const std::vector<CObject*>& in_Children, CEntity* in_pEntity );
	void FinalizeInternals();

	// access
	const std::string& GetName() const { return m_Name; }
	bool m_bEnabled;
	CElementWrapper<CMatrix,MAX_SHADOW_COPIES> m_Position;
	const std::vector<CObject*>& GetChildren() const { assert(m_bFinalizedInternals); return m_Children; }
	CEntity* const GetEntity() const { assert(m_bFinalizedInternals); return m_pEntity; }

protected:
	void SomethingLinkedToYou( CLinkOrigin* in_pOrigin ) {
		m_ShadowTracker.Connected( in_pOrigin );
	}
	void SomethingUnlinkedFromYou( CLinkOrigin* in_pOrigin ) {
		m_ShadowTracker.ShadowDisconnected( in_pOrigin );
	}
	void NotificatorDied( CLinkOrigin* in_pOrigin ) {
		m_ShadowTracker.ShadowDied( in_pOrigin );
	}
	void KickMe() { assert(false); }

private:
	std::string m_Name;
	std::vector<CObject*> m_Children;
	CEntity* m_pEntity;
	bool m_bFinalizedInternals;
	CTShadowTrackerSimple<MAX_SHADOW_COPIES> m_ShadowTracker;
};


CBox GetBox( const CObject& in_rObject, bool in_bApplyRootMatrixToo = false );

#endif

















