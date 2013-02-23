#ifndef GAME_OBJECT_HPP_INCLUDED
#define GAME_OBJECT_HPP_INCLUDED

#include "../../Render_DX8/Include/algebra.h"

#include <assert.h>
#include <vector>
#include <set>

class CGameObjectOwner
{
public:
	virtual void SomeObjectPreloadComplete( bool in_bSuccess, void* in_Handle ) = 0;
};

class CGameObject
{
public:
	CGameObject( CGameObjectOwner* in_pOwner ) : m_pOwner(in_pOwner), m_bPreloadComplete(false)
	{
		assert( m_pOwner );
	}
	virtual ~CGameObject()
	{
		assert( m_Children.empty() );
	}

public:
	void StartPreload();
	void UpdateAndDraw( unsigned long in_Time );
	bool AcceptMouse(const CVector& in_Org, const CVector& in_Dir, bool in_bClickDetected);
	bool AcceptKey(unsigned int scanCode);
protected:

	virtual void InputAccepted(){};

	std::vector< CGameObject* >& GetChildren()
	{
		return m_Children;
	}

	virtual bool vAcceptMouse(const CVector& in_Org, const CVector& in_Dir, bool in_bClickDetected) = 0;
	virtual bool vAcceptKey(unsigned int scanCode) = 0;
	virtual void vStartPreload() = 0;
	void PreloadComplete( bool in_bSuccess );
	//{
	//	if(!m_bPreloadComplete)
	//	{
	//		assert( m_pOwner );
	//		m_pOwner->SomeObjectPreloadComplete( in_bSuccess, (T*)this );
	//		m_bPreloadComplete = true;
	//	}
	//}
	virtual void vUpdateBeforeChildren( unsigned long in_Time ) = 0;
	virtual void vUpdateAfterChildren() = 0;
	
	

protected:
	bool m_bPreloadComplete;
private:
	std::vector< CGameObject* > m_Children;
	CGameObjectOwner* m_pOwner;

};

#endif














