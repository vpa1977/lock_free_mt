#ifndef STATIC_MESH_DX8_HPP_INCLUDED
#define STATIC_MESH_DX8_HPP_INCLUDED

#include "../../Objects/Include/e_static_mesh.h"
#include "renderable_dx8.h"
#include "skeleton_dx8.h"
#include <assert.h>

struct CStaticMesh_DX8 {
	CStaticMesh_DX8(CEntity_StaticMesh* in_pMesh);

	CEntity_StaticMesh* m_pStaticMesh; // source of all information

	//std::vector< CRenderable_DX8 > m_Renderables;
	std::vector< CRenderable_MaterialDX8 > m_Materials;
	std::vector< CRenderable_BoneRemapDX8 > m_BoneRemaps;

	CSkeleton_DX8 m_Skeleton;

	CBone_DX8* CreateBoneShadowRecurse( CBone* pSrcBone, const CBone* pSrcParentBone );
};

#endif





















