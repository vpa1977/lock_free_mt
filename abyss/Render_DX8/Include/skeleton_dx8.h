#ifndef SKELETON_DX8_HPP_INCLUDED
#define SKELETON_DX8_HPP_INCLUDED

#include "../../Common/Include/skeleton.h"
#include "../../Objects/Include/shadow.h"
#include <map>
#include <assert.h>

struct CBone_DX8
{
	CBone_DX8( const CMatrix& in_rM, const CMatrix& in_rParent, const CMatrix in_rInverseBoneSkin ) :
		m_CurrentPosition(in_rM)
		, m_InverseBoneSkin(in_rInverseBoneSkin)
		//,m_bPositionChanged(false)
		//,m_ParentPosition(in_rParent)
		,m_InitialPositionInverse( (in_rM*in_rParent).Invert() )
	{}

	std::vector< CBone_DX8* > m_Children;
	unsigned long m_BoneID;

	CElementShadow< CMatrix > m_CurrentPosition;
	CMatrix m_FullCurrentPosition; // including parent bones
	CMatrix m_InverseBoneSkin;
	CMatrix m_InitialPositionInverse;
};

struct CSkeleton_DX8
{
	CSkeleton_DX8() : m_pRootBone(NULL) {}
	CBone_DX8* m_pRootBone;
	std::vector< CBone_DX8* > m_BoneIDs;
	void ConstructArray() {
		assert( m_pRootBone );
		assert( m_BoneIDs.empty() );
		std::map< unsigned long, CBone_DX8* > rBones;
		AddBoneToArray(rBones,m_pRootBone);
		m_BoneIDs.resize( rBones.size() );
		unsigned long ExpectedID = 0;
		for( std::map< unsigned long, CBone_DX8* >::iterator It=rBones.begin(); It!=rBones.end(); ++It,++ExpectedID ) {
			assert( (*It).first == ExpectedID );
		}
	}
private:
	void AddBoneToArray( std::map< unsigned long, CBone_DX8* >& rBones, CBone_DX8* pBone ) {
		assert( pBone );
		assert( rBones.find(pBone->m_BoneID) == rBones.end() );
		rBones.insert( std::pair< unsigned long, CBone_DX8* >(pBone->m_BoneID,pBone) );
		for( size_t i=0; i<pBone->m_Children.size(); ++i ) {
			AddBoneToArray( rBones, pBone->m_Children[i] );
		}
	}
};

#endif

































