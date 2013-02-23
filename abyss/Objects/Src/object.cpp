#include "../Include/object.h"
#include "../Include/e_render.h"
#include "../../MT_Core/Include/log.h"
#include <stdio.h>

CObject::CObject(	const std::string& in_Name
					,bool in_bEnabled
					,const CMatrix& in_Position
					,const std::vector<CObject*>& in_Children
					,CEntity* in_pEntity ) :
	m_Name(in_Name)
	,m_bEnabled(in_bEnabled)
	,m_Position(in_Position)
	,m_Children(in_Children)
	,m_pEntity(in_pEntity)
	,m_bFinalizedInternals(true)
{
	//CLog::Print("CObject(%p)::CObject(), pOrigin=%p\n",this,(CLinkOrigin*)this);
	m_Position.Init(this);
}

CObject::CObject(	const std::string& in_Name
					,bool in_bEnabled
					,const CMatrix& in_Position ) :
	m_Name(in_Name)
	,m_bEnabled(in_bEnabled)
	,m_Position(in_Position)
	,m_pEntity( NULL )
	,m_bFinalizedInternals(false)
{
	//CLog::Print("CObject(%p)::CObject(), pOrigin=%p\n",this,(CLinkOrigin*)this);
	m_Position.Init(this);
}

CObject::~CObject()
{
	//CLog::Print("CObject(%p)::~CObject()\n",this);
	assert( !IsBeingChangedRightNow() );
	assert( m_ShadowTracker.NoShadowsAlive() );
	// TODO: delete entity ?
	// TODO: delete children ?
}

void
CObject::AssignInternals( const std::vector<CObject*>& in_Children, CEntity* in_pEntity ) {
	assert( !m_bFinalizedInternals );
	m_Children = in_Children;
	m_pEntity = in_pEntity;
}

void
CObject::FinalizeInternals() {
	m_bFinalizedInternals = true;
}


CBox
GetBox( const CObject& in_rObject, bool in_bApplyRootMatrixToo ) {
	CBox Box;
	CEntity* const pEnt = in_rObject.GetEntity();
	if(ED_RENDER == pEnt->GetDomain()) {
		Box = ((CEntity_RenderDomain*)pEnt)->GetBox();
	}
	const std::vector<CObject*>& rChildren = in_rObject.GetChildren();
	for( std::vector<CObject*>::const_iterator It = rChildren.begin(); It!=rChildren.end(); ++It ) {
		Box += GetBox( *(*It), true );
	}
	if( in_bApplyRootMatrixToo )
		Box *= in_rObject.m_Position;
	return Box;
}















