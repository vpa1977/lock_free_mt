#ifndef RENDER_SHADOW_OBJECTS_HPP_INCLUDED
#define RENDER_SHADOW_OBJECTS_HPP_INCLUDED

#include "../../Objects/Include/shadow.h"
#include "../../Objects/Include/object_defines.h"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <assert.h>


using namespace ::boost;
using namespace ::boost::multi_index;


class CRenderDX8_ShadowCreator : public CShadowCreator {
public:
	CRenderDX8_ShadowCreator();
	~CRenderDX8_ShadowCreator();
	CShadow* CreateShadow( CObject* );
	void ReclaimShadow( CShadow* );
	void DeleteAllShadowsForObject( CObject* );
private:
	struct CShadowRecord {
		CShadowRecord( CObject* in_pObject=NULL, CShadow* in_pShadow=NULL, bool in_bInUse=false ) :
			m_pObject(in_pObject), m_pShadow(in_pShadow), m_bInUse(in_bInUse)
		{}
		CObject* m_pObject;
		CShadow* m_pShadow;
		bool m_bInUse;
	};
	struct CAssignShadow {
		CAssignShadow(CShadow* in_pShadow) : m_pShadow(in_pShadow) {}
		void operator() (CShadowRecord& inout_rShadowRecord) {
			inout_rShadowRecord.m_pShadow = m_pShadow;
		}
	private:
		CShadow* m_pShadow;
	};
	struct CAssignUse {
		CAssignUse(bool in_bInUse) : m_bInUse(in_bInUse) {}
		void operator() (CShadowRecord& inout_rShadowRecord) {
			inout_rShadowRecord.m_bInUse = m_bInUse;
		}
	private:
		bool m_bInUse;
	};

	class Tag_Object {};
	typedef ordered_non_unique
	<
		tag< Tag_Object >,
		member< CShadowRecord, CObject*, &CShadowRecord::m_pObject >
	> CIndexObject;

	class Tag_Shadow {};
	typedef ordered_unique
	<
		tag< Tag_Shadow >,
		member< CShadowRecord, CShadow*, &CShadowRecord::m_pShadow >
	> CIndexShadow;

	typedef multi_index_container
	<
		CShadowRecord,
		indexed_by
		<
			CIndexObject
			,CIndexShadow
		>
	> CShadowRecordMap;
	typedef CShadowRecordMap::index< Tag_Object >::type CShadowRecordMap_by_Object;
	typedef CShadowRecordMap::index< Tag_Shadow >::type CShadowRecordMap_by_Shadow;
	CShadowRecordMap m_ShadowRecordMap;

	void CheckChildrenNotInShadowMap( CObject* in_pObject, bool in_bCheckThisObject );
	CShadow* CreateShadowRecurse( CObject* in_pObject, CShadow* in_pPreviousShadow );

	void DeleteAllShadowsForObjectRecurse( CShadow* );
};

#endif





























