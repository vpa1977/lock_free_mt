#ifndef M3_GAMEDEVICE_HPP_INCLUDED
#define M3_GAMEDEVICE_HPP_INCLUDED

#include "game_object.h"
#include "m3_gameunit.h"
#include "m3_gamebox.h"
#include <vector>

class CGameDevice :
	public CGameObject
	,public CGameObjectOwner
{
public:
	CGameDevice( CGameObjectOwner* in_pOwner );

/////////////////////////////////////////////////
	// virtual from CGameObjectOwner
public:
	void SomeObjectPreloadComplete( bool in_bSuccess, void* in_Handle );
/////////////////////////////////////////////////

protected:
	void vStartPreload();
	void vUpdateBeforeChildren( unsigned long in_Time );
	void vUpdateAfterChildren() {}
	bool vAcceptKey(unsigned int scanCode );
	bool vAcceptMouse(const CVector& in_Org, const CVector& in_Dir, bool in_bClickDetected);

private:
	void UpdateHighlight();

	std::set<CGameObject*> m_ChildrenWithPendingPreload;
	bool m_bSelfPreloadComplete;
	bool m_bWasErrorDuringPreload;
	void CheckPreloadComplete();
	void CheckComplete();
	void SetMatrix();
	void Shuffle();
	bool MovePiece(int where);
	void MovePieceForShuffle(int where);

	CVector m_Org;
	CVector m_Dir;
	std::vector<CGameUnit*> m_Units;
	CMoveCounter* m_pMoveCounter;
	CGameBox* m_Box;
	unsigned char m_Field[4][4];

	unsigned int m_iSeqLength;
	unsigned int m_iDir;
	bool m_bComplete;
	bool m_bNeedUpdateHighlight;

	int m_xHole;
	int m_yHole;

	unsigned long m_LastUpdateTime;
	bool m_bMoving;
	unsigned long m_StartMoveTime;
};

#endif