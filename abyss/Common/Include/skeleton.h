#ifndef SKELETON_HPP_INCLUDED
#define SKELETON_HPP_INCLUDED

#include "../../Render_DX8/Include/algebra.h"
#include "element.h"
#include "../../Objects/Include/object_defines.h"

#include <vector>
#include <map>

class CBonePosition : public boost::noncopyable
{
public:
	CBonePosition( const std::pair< CQuaternion, CVector >& in_rPosition ) {
		m_Position = in_rPosition.first.ToMatrix();
		*reinterpret_cast<CVector*>(&(m_Position.m[12])) = in_rPosition.second;
	}
	CBonePosition( const CMatrix& in_rM ) {
		m_Position = in_rM;
	}

	void SetPosition( const CQuaternion& in_rRotation, const CVector& in_rOffset ) {
		m_Position = in_rRotation.ToMatrix();
		*reinterpret_cast<CVector*>(&(m_Position.m[12])) = in_rOffset;
	}
	void SetPosition( const CMatrix& in_rM ) {
		m_Position = in_rM;
	}

	const CMatrix& GetPosition() const { return m_Position; }

private:
	CMatrix m_Position;
};

class CBone :
	public CElementOwner,
	public boost::noncopyable
{
public:
	CBone( unsigned long in_BoneID, const std::pair< CQuaternion, CVector >& in_rPosition, const std::vector< CBone* >& in_rChildren, const std::string& in_rName="" ) :
		m_Position(in_rPosition),
		m_Children(in_rChildren),
		m_BoneID(in_BoneID),
		m_Name(in_rName)
		
	{
		m_Position.Init(this);
	}
	CBone( unsigned long in_BoneID, const CMatrix& in_rM, const CMatrix& in_rInverseBoneSkin, const std::vector< CBone* >& in_rChildren, const std::string& in_rName="" ) :
		m_Position(in_rM),
		m_Children(in_rChildren),
		m_BoneID(in_BoneID),
		m_Name(in_rName),
		m_InvBoneSkin(in_rInverseBoneSkin)
	{
		m_Position.Init(this);
	}
	~CBone() {
		// TODO: delete bones ?
	}

	CElementWrapper< CBonePosition, MAX_SHADOW_COPIES > m_Position;
	CMatrix m_InvBoneSkin;
	const std::vector< CBone* >& GetChildren() const { return m_Children; }
	unsigned long GetID() const { return m_BoneID; }
	const std::string& GetName() const { return m_Name; }

private:
	std::vector< CBone* > m_Children;
	unsigned long m_BoneID;
	std::string m_Name;
};

class CSkeleton : public boost::noncopyable
{
public:
	CSkeleton( CBone* in_pRootBone ) : m_pRootBone(in_pRootBone) {
		if( m_pRootBone ) {
			CIDMap IDMap;
			if( AddBoneToMap( m_pRootBone, IDMap ) ) {
				// check, if bone IDs are sequential
				unsigned long ExpectedID = 0;
				for( CIDMap::const_iterator It=IDMap.begin(); It!=IDMap.end(); ++It,++ExpectedID ) {
					long res = It->first;
					if( It->first != ExpectedID )
						break;
				}
				if( ExpectedID == IDMap.size() ) {
					// create m_BoneIDs
					m_BoneIDs.resize( IDMap.size() );
					for( CIDMap::const_iterator It=IDMap.begin(); It!=IDMap.end(); ++It ) {
						m_BoneIDs[It->first] = It->second;
					}
				}
			}
		}
	}
	~CSkeleton() {
		// TODO: delete bones ?
	}

	CBone* GetRootBone() const { return m_pRootBone; }
	std::vector< CBone* >& GetBoneIDs() { return m_BoneIDs; }

private:
	CBone* m_pRootBone;
	std::vector< CBone* > m_BoneIDs;

	typedef std::map< unsigned long, CBone* > CIDMap;
	bool AddBoneToMap( CBone* in_pBone, CIDMap& in_rMap ) {
		// TODO
		if(in_pBone) {
			CIDMap::const_iterator It = in_rMap.find( in_pBone->GetID() );
			if( It != in_rMap.end() ) {
				// already have this bone ID in map
				return false;
			}
			in_rMap.insert( CIDMap::value_type( in_pBone->GetID(), in_pBone ) );
			const std::vector< CBone* >& rChildren( in_pBone->GetChildren() );
			for( std::vector< CBone* >::const_iterator It = rChildren.begin(); It != rChildren.end(); ++It ) {
				if( !AddBoneToMap( *It, in_rMap ) )
					return false;
			}
		}
		return true;
	}
};

#endif


























