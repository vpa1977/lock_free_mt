#include "../Include/static_mesh_dx8.h"

CStaticMesh_DX8::CStaticMesh_DX8(CEntity_StaticMesh* in_pMesh) :
	m_pStaticMesh(in_pMesh)
{
	assert(m_pStaticMesh);

	// construct shadow for skeleton

	const std::vector< CBone* >& rSrcBones = m_pStaticMesh->GetSkeleton().GetBoneIDs();
	m_Skeleton.m_BoneIDs.resize( rSrcBones.size() );
	for( size_t i=0; i<rSrcBones.size(); ++i )
		m_Skeleton.m_BoneIDs[i] = NULL;

	CBone* pSrcRootBone = m_pStaticMesh->GetSkeleton().GetRootBone();
	if(pSrcRootBone) {
		
		m_Skeleton.m_pRootBone = CreateBoneShadowRecurse( pSrcRootBone, NULL );
		
		assert( !rSrcBones.empty() );
		for( size_t i=0; i<rSrcBones.size(); ++i ) {
			assert( m_Skeleton.m_BoneIDs[i] != NULL );
			if( m_Skeleton.m_BoneIDs[i]->m_BoneID == pSrcRootBone->GetID() ) {
				m_Skeleton.m_pRootBone = m_Skeleton.m_BoneIDs[i];
			}
		}
	}
}

CBone_DX8*
CStaticMesh_DX8::CreateBoneShadowRecurse( CBone* pSrcBone, const CBone* pSrcParentBone ) {
	assert( pSrcBone );

	CBone_DX8* pResult;
	const CBonePosition& rPosition = pSrcBone->m_Position;
	if( pSrcParentBone ) {
		const CBonePosition& rParentPosition = pSrcParentBone->m_Position;
		pResult = new CBone_DX8( rPosition.GetPosition(), rParentPosition.GetPosition(), pSrcBone->m_InvBoneSkin );
	} else {
		pResult = new CBone_DX8( rPosition.GetPosition(), CMatrix(), pSrcBone->m_InvBoneSkin );
	}

	pResult->m_BoneID = pSrcBone->GetID();
	m_Skeleton.m_BoneIDs[ pResult->m_BoneID ] = pResult;

	pResult->m_Children.resize( pSrcBone->GetChildren().size() );
	for( size_t i=0; i<pSrcBone->GetChildren().size(); ++i ) {
		pResult->m_Children[i] = CreateBoneShadowRecurse( pSrcBone->GetChildren()[i], pSrcBone );
	}

	pSrcBone->m_Position.ConnectLink( &(pResult->m_CurrentPosition) );
	pResult->m_CurrentPosition.ConnectLink( &(pSrcBone->m_Position) );

	return pResult;
}



















