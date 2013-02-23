#include "../Include/game_object.h"
#include <assert.h>

void
CGameObject::StartPreload()
{
	vStartPreload();
	std::vector< CGameObject* >& rChildren = GetChildren();
	for( std::vector< CGameObject* >::iterator It = rChildren.begin(); It!=rChildren.end(); ++It )
	{
		(*It)->StartPreload();
	}
}

bool
CGameObject::AcceptMouse(const CVector& in_Org, const CVector& in_Dir, bool in_bClickDetected)
//CGameObject::AcceptClick(const CVector& vec) 
{
	if (vAcceptMouse(in_Org,in_Dir,in_bClickDetected))
	{
		InputAccepted();
		return true;
	}
	std::vector< CGameObject* >& rChildren = GetChildren();
	for( std::vector< CGameObject* >::iterator It = rChildren.begin(); It!=rChildren.end(); ++It )
	{
		bool res = (*It)->AcceptMouse(in_Org,in_Dir,in_bClickDetected);
		if (res) {
			InputAccepted();
			return true;
		}
	}
	return false;

}

bool 
CGameObject::AcceptKey(unsigned int scanCode) 
{
	if (vAcceptKey(scanCode))
	{
		InputAccepted();
		return true;
	}
	std::vector< CGameObject* >& rChildren = GetChildren();
	for( std::vector< CGameObject* >::iterator It = rChildren.begin(); It!=rChildren.end(); ++It )
	{
		bool res = (*It)->AcceptKey(scanCode);
		if (res) {
			InputAccepted();
			return true;
		}

	}
	return false;

}



void
CGameObject::UpdateAndDraw( unsigned long in_Time )
{
	vUpdateBeforeChildren( in_Time );
	std::vector< CGameObject* >& rChildren = GetChildren();
	for( std::vector< CGameObject* >::iterator It = rChildren.begin(); It!=rChildren.end(); ++It )
	{
		(*It)->UpdateAndDraw( in_Time );
	}
	vUpdateAfterChildren();
}

void
CGameObject::PreloadComplete( bool in_bSuccess )
{
	if(!m_bPreloadComplete)
	{
		assert( m_pOwner );
		m_pOwner->SomeObjectPreloadComplete( in_bSuccess, this );
		m_bPreloadComplete = true;
	}
}




























