#include "../Include/render_shadow_creator.h"

#include "../../Objects/Include/e_static_mesh.h"

#include "../Include/es_static_mesh.h"
#include "../../Objects/Include/es_dummy.h"

#include "../../MT_Core/Include/log.h"
#include <assert.h>

CRenderDX8_ShadowCreator::CRenderDX8_ShadowCreator()
{
}

CRenderDX8_ShadowCreator::~CRenderDX8_ShadowCreator() {
	assert( m_ShadowRecordMap.empty() );
}

void
UpdatePositionsRecurse( CShadow* in_pShadow, CObject* in_pObject ) {
	assert( in_pShadow );
	assert( in_pObject );
	if(in_pShadow->m_Position.IsDirty()) {
		in_pShadow->m_Position = in_pObject->m_Position;
	}
	assert( in_pShadow->m_Children.size() == in_pObject->GetChildren().size() );
	for( size_t N=0; N<in_pShadow->m_Children.size(); ++N ) {
		UpdatePositionsRecurse( in_pShadow->m_Children[N], in_pObject->GetChildren()[N] );
	}
}

void
UpdateFullBoneMatrixRecurse( CBone_DX8* in_pBone, const CMatrix& in_rPM ) {
	if(in_pBone) {
		in_pBone->m_FullCurrentPosition = in_pBone->m_CurrentPosition.Get()*in_rPM;
		for( size_t i=0; i<in_pBone->m_Children.size(); ++i ) {
			UpdateFullBoneMatrixRecurse( in_pBone->m_Children[i], in_pBone->m_FullCurrentPosition );
		}
	}
}

void
UpdateSkeletonsRecurse( CShadow* in_pShadow, CObject* in_pObject ) {
	assert( in_pShadow );
	assert( in_pObject );
	CEntity* pEntity = in_pObject->GetEntity();
	if( pEntity ) {
		if( pEntity->GetType() == ET_STATIC_MESH ) {
			assert( in_pShadow->GetType() == ET_STATIC_MESH );

			CEntity_StaticMesh* pSrcMesh = (CEntity_StaticMesh*)pEntity;
			CSkeleton& rSrcSkeleton = pSrcMesh->GetSkeleton();
			
			CEntityShadow_StaticMesh* pDstShadow = (CEntityShadow_StaticMesh*)in_pShadow;
			CSkeleton_DX8& rDstSkeleton = pDstShadow->m_pStaticMeshDX8->m_Skeleton;

			assert( rSrcSkeleton.GetBoneIDs().size() == rDstSkeleton.m_BoneIDs.size() );

			for( size_t i=0; i<rDstSkeleton.m_BoneIDs.size(); ++i ) {
				if( rDstSkeleton.m_BoneIDs[i]->m_CurrentPosition.IsDirty() ) {
					CBone_DX8* pDstBone = rDstSkeleton.m_BoneIDs[i];
					CBone* pSrcBone = rSrcSkeleton.GetBoneIDs()[i];
					const CBonePosition& rPosition = pSrcBone->m_Position;
					pDstBone->m_CurrentPosition = rPosition.GetPosition();
				
				}
			}
			UpdateFullBoneMatrixRecurse( rDstSkeleton.m_pRootBone, CMatrix() );
		}
	}
	assert( in_pShadow->m_Children.size() == in_pObject->GetChildren().size() );
	for( size_t N=0; N<in_pShadow->m_Children.size(); ++N ) {
		UpdateSkeletonsRecurse( in_pShadow->m_Children[N], in_pObject->GetChildren()[N] );
	}
}

CShadow*
CRenderDX8_ShadowCreator::CreateShadow( CObject* in_pObject ) {
//	CLog::Print("--CRenderDX8_ShadowCreator::CreateShadow( %p )\n",in_pObject);
	assert( in_pObject );
	//assert( in_pObject->GetChildren().empty() );
	//assert( in_pObject->GetEntity() );
	//assert( in_pObject->GetEntity()->GetType()==ET_STATIC_MESH );

	CheckChildrenNotInShadowMap( in_pObject, false );

	CShadowRecordMap_by_Object& rIndexByObject( m_ShadowRecordMap.get<Tag_Object>() );
	CShadowRecordMap_by_Object::iterator ItObject = rIndexByObject.find(in_pObject);
	CShadowRecordMap_by_Object::iterator ItExistingShadow = rIndexByObject.end();
	if( ItObject != rIndexByObject.end() ) {
//		CLog::Print("----already have this object in map\n");
		ItExistingShadow = ItObject;
		size_t N=0;
		for( ; N<MAX_SHADOW_COPIES && ItObject!=rIndexByObject.end(); ++N,++ItObject ) {
//			CLog::Print("----N==%lu\n",N);
			assert( ItObject->m_pShadow );
			if( ItObject->m_pObject != in_pObject ) {
//				CLog::Print("----not this object\n");
				break;
			}
			if( ItObject->m_bInUse == false ) {
//				CLog::Print("----not in use\n");
				break;
			}
		}
		bool WeAlreadyHaveMaxAllowedShadowsForThisObject = ( N >= MAX_SHADOW_COPIES );
		assert( !WeAlreadyHaveMaxAllowedShadowsForThisObject );
	}
	CShadow* pResult = NULL;
	if( ItObject!=rIndexByObject.end() && ItObject->m_pObject==in_pObject && ItObject->m_bInUse==false ) {
//		CLog::Print("----reuse shadow\n");
		assert(ItObject->m_pShadow);
		rIndexByObject.modify( ItObject, CAssignUse(true) );
		pResult = ItObject->m_pShadow;
	}
	else {
//		CLog::Print("----create new shadow\n");
		CShadow* pPreviousShadow = (ItExistingShadow==rIndexByObject.end()) ? NULL : ItExistingShadow->m_pShadow;

		pResult = CreateShadowRecurse( in_pObject, pPreviousShadow );
		std::pair<CShadowRecordMap_by_Object::iterator,bool> Where( rIndexByObject.insert( CShadowRecord(in_pObject,pResult,true) ) );
		assert(Where.second);
	}

	UpdatePositionsRecurse( pResult, in_pObject );
	UpdateSkeletonsRecurse( pResult, in_pObject );

//	CLog::Print("----return %p\n",pResult);
	return pResult;
}

void
CRenderDX8_ShadowCreator::ReclaimShadow( CShadow* in_pShadow ) {
//	CLog::Print("--CRenderDX8_ShadowCreator::ReclaimShadow( %p )\n",in_pShadow);
	assert( in_pShadow );
	//assert( in_pShadow->GetType() == ET_STATIC_MESH );

	CShadowRecordMap_by_Shadow& rIndexByShadow( m_ShadowRecordMap.get<Tag_Shadow>() );
	CShadowRecordMap_by_Shadow::iterator ItShadow = rIndexByShadow.find(in_pShadow);
	assert( ItShadow != rIndexByShadow.end() );
	assert( ItShadow->m_bInUse == true );
	rIndexByShadow.modify( ItShadow, CAssignUse(false) );
}

void
CRenderDX8_ShadowCreator::DeleteAllShadowsForObject( CObject* in_pObject ) {
	CLog::Print("--CRenderDX8_ShadowCreator::DeleteAllShadowsForObject( %p )\n",in_pObject);

	CShadowRecordMap_by_Object& rIndexByObject( m_ShadowRecordMap.get<Tag_Object>() );
	for(	CShadowRecordMap_by_Object::iterator ItObject = rIndexByObject.find(in_pObject);
			ItObject != rIndexByObject.end();
			ItObject = rIndexByObject.find(in_pObject)	)
	{
		assert( false == ItObject->m_bInUse );
		DeleteAllShadowsForObjectRecurse( ItObject->m_pShadow );
		rIndexByObject.erase( ItObject );
	}
}

void
CRenderDX8_ShadowCreator::CheckChildrenNotInShadowMap( CObject* in_pObject, bool in_bCheckThisObject ) {
	assert( in_pObject );
	if( in_bCheckThisObject ) {
		CShadowRecordMap_by_Object& rIndexByObject( m_ShadowRecordMap.get<Tag_Object>() );
		bool InMap = ( rIndexByObject.find(in_pObject) != rIndexByObject.end() );
		assert( !InMap );
	}
	const std::vector<CObject*>& rChildren( in_pObject->GetChildren() );
	for( std::vector<CObject*>::const_iterator It=rChildren.begin(); It!=rChildren.end(); ++It ) {
		CheckChildrenNotInShadowMap( *It, true );
	}
}

CShadow*
CRenderDX8_ShadowCreator::CreateShadowRecurse( CObject* in_pObject, CShadow* in_pPreviousShadow ) {
	assert(in_pObject);

	CShadow* pResult = NULL;

	// create shadow itself
	CEntity* pEntity = in_pObject->GetEntity();
	if( !pEntity ) {
		CEntityShadow_Dummy* pShadow = new CEntityShadow_Dummy();
		pShadow->ConnectLink( in_pObject );
		pResult = pShadow;
	} else {
		assert( pEntity->GetType() == ET_STATIC_MESH );
		CEntity_StaticMesh* pMesh = (CEntity_StaticMesh*)pEntity;
		CEntityShadow_StaticMesh* pShadow = NULL;
		if( in_pPreviousShadow ) {
			assert( in_pPreviousShadow->GetType() == ET_STATIC_MESH );
			CEntityShadow_StaticMesh* pExisting = (CEntityShadow_StaticMesh*)in_pPreviousShadow;
			pShadow = new CEntityShadow_StaticMesh( pExisting->m_pStaticMeshDX8 );
		} else {
			pShadow = new CEntityShadow_StaticMesh( boost::shared_ptr<CStaticMesh_DX8>( new CStaticMesh_DX8(pMesh) ) );
		}
		pShadow->ConnectLink( pMesh );
		pShadow->ConnectLink( in_pObject );
		pResult = pShadow;
	}
	in_pObject->m_Position.ConnectLink( &(pResult->m_Position) );
	pResult->m_Position.ConnectLink( &(in_pObject->m_Position) );

	// create children shadows
	const std::vector<CObject*>& rSrcObjectChildren( in_pObject->GetChildren() );
	pResult->m_Children.resize( rSrcObjectChildren.size() );
	if( in_pPreviousShadow ) {
		std::vector<CShadow*>& rSrcShadowChildren( in_pPreviousShadow->m_Children );
		assert( rSrcObjectChildren.size() == rSrcShadowChildren.size() );
		for( size_t N=0; N<rSrcObjectChildren.size(); ++N ) {
			pResult->m_Children[N] = CreateShadowRecurse( rSrcObjectChildren[N], rSrcShadowChildren[N] );
		}
	} else {
		for( size_t N=0; N<rSrcObjectChildren.size(); ++N ) {
			pResult->m_Children[N] = CreateShadowRecurse( rSrcObjectChildren[N], NULL );
		}
	}

	return pResult;
}


void
CRenderDX8_ShadowCreator::DeleteAllShadowsForObjectRecurse( CShadow* in_pShadow ) {
	if(in_pShadow) {
		for( size_t N=0; N<in_pShadow->m_Children.size(); ++N )
			DeleteAllShadowsForObjectRecurse( in_pShadow->m_Children[N] );
		DeleteShadow( in_pShadow );
	}
}















