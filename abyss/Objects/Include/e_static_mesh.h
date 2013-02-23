#ifndef ENTITY_STATIC_MESH_HPP_INCLUDED
#define ENTITY_STATIC_MESH_HPP_INCLUDED

#include "../../Common/Include/skeleton.h"
//#include "../../Common/Include/link.h"
#include "entity.h"
#include "e_render.h"
//#include "../../Common/Include/shadow_tracker.h"
#include "object_defines.h"
#include "render/renderable.h"
#include "render/material.h"
#include <vector>

//=======================================================================================

class CEntity_StaticMesh :
	public CEntity_RenderDomain
	,public CLinkTarget
{
public:
	CEntity_StaticMesh();
	CEntity_StaticMesh(
		const std::vector< CRenderable_AbstractMaterial > in_Materials
		,const std::vector< CRenderable_BoneRemap >& in_RemappedGeometries
		,CBone* in_pRootBone = NULL
	);
	~CEntity_StaticMesh();

	unsigned long GetType() const { return ET_STATIC_MESH; }

	CSkeleton& GetSkeleton() { return m_Skeleton; }
	const std::vector< CRenderable_BoneRemap >& GetRemappedGeometries() const { return m_RemappedGeometries; }
	const std::vector< CRenderable_AbstractMaterial >& GetMaterials() const { return m_Materials; }

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

	const CBox& vGetBox() const { return m_Box; }

private:
	CSkeleton m_Skeleton;
	std::vector< CRenderable_BoneRemap >		m_RemappedGeometries;
	std::vector< CRenderable_AbstractMaterial >	m_Materials;

	CBox m_Box;
	void CalcBox();

	CTShadowTrackerSimple<MAX_SHADOW_COPIES> m_ShadowTracker;
};

#endif





