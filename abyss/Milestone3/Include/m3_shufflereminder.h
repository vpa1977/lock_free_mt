#ifndef M3_SHUFFLEREMINDER_HPP_INCLUDED
#define M3_SHUFFLEREMINDER_HPP_INCLUDED

#include "game_object.h"

class CShuffleReminder :
	public CGameObject
{
public:
	CShuffleReminder( CGameObjectOwner* in_pOwner ) :
		CGameObject( in_pOwner )
	{}

protected:
	void vStartPreload();
	void vUpdateBeforeChildren( unsigned long in_Time );
	void vUpdateAfterChildren() {}
	bool vAcceptKey(unsigned int scanCode ) { return false; }
	bool vAcceptMouse(const CVector& in_Org, const CVector& in_Dir, bool in_bClickDetected) {return false;}

private:
};

#endif