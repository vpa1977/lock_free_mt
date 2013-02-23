#ifndef M3_MOVECOUNTER_HPP_INCLUDED
#define M3_MOVECOUNTER_HPP_INCLUDED

#include "game_object.h"
//#include "load_proxy.h"


// move counter.
class CMoveCounter :
	public CGameObject
//	,public CLoadUser
{
public:
	CMoveCounter( CGameObjectOwner* in_pOwner ) :
		CGameObject( in_pOwner )
		,m_iCount(0)
		,m_bComplete(false)
	{
	}
	void Complete()
	{
		m_bComplete = true;
	}
	void Increment()
	{
		m_iCount++;
	}
	void Reset()
	{
		m_iCount = 0;
		m_bComplete = false;
	}
//	void ReadFileResult( bool in_bSuccess, const std::string& in_rFileName, const std::vector<unsigned char>& in_rData );
//	void DecodeFileResult( bool in_bSuccess, const std::string& in_rFileName, unsigned long in_Width, unsigned long in_Height, const std::vector<unsigned char>& in_rData );
//	void DecodeRawResult( bool in_bSuccess, unsigned long in_Width, unsigned long in_Height, const std::vector<unsigned char>& in_rData );

protected:
	void vStartPreload();
	void vUpdateBeforeChildren( unsigned long in_Time );
	void vUpdateAfterChildren() {}
	bool vAcceptKey(unsigned int scanCode ) { return false; }
	bool vAcceptMouse(const CVector& in_Org, const CVector& in_Dir, bool in_bClickDetected) {return false;}

private:
	unsigned int m_iCount;
	bool m_bComplete;
};

#endif
