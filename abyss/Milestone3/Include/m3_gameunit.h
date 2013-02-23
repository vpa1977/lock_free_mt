#ifndef M3_GAMEUNIT_HPP_INCLUDED
#define M3_GAMEUNIT_HPP_INCLUDED

#include "game_object.h"
#include "load_proxy.h"
#include "render_proxy.h"
#include "m3_movecounter.h"
#include <string>


class CGameUnit :
	public CGameObject
	,public CLoadUser
	,public CRenderUser
{
public:
	CGameUnit( unsigned long in_N, CGameObjectOwner* in_pOwner );

	// load user
	void ReadFileResult( bool in_bSuccess, const std::string& in_rFileName, const std::vector<unsigned char>& in_rData );
	void DecodeFileResult( bool in_bSuccess, const std::string& in_rFileName, long in_Width, long in_Height, const std::vector<unsigned char>& in_rData );
	void DecodeRawResult( bool in_bSuccess, long in_Width, long in_Height, const std::vector<unsigned char>& in_rData );

	// render user
	void CreateVBResult( unsigned long in_VBHandle );
	void UpdateVBResult( bool in_bSuccess );
	void CreateTextureResult( unsigned long in_TexHandle );
	void CreatePSResult( unsigned long in_ShaderHandle );
	void CreateFontResult(unsigned long in_FontHandle);

    void SetPosition(int x, int y);
	void MoveToPosition(long X, long Y, unsigned long in_DeltaTime);
	bool GetHighlight(const CVector& in_Org, const CVector& in_Dir);
	void SetHighlight(bool f ) { m_bHighlighted = f; };
protected:
	void vStartPreload();
	void vUpdateBeforeChildren( unsigned long in_Time );
	void vUpdateAfterChildren() {}
	bool vAcceptKey(unsigned int scanCode ) { return false; }
	bool vAcceptMouse(const CVector& in_Org, const CVector& in_Dir, bool in_bClickDetected);

private:

	unsigned long m_N;
	unsigned short m_InternalState;

	long m_X;
	long m_Y;
	unsigned long m_LastUpdateTime;
	bool m_bMoving;
	unsigned long m_StartMoveTime;
	unsigned long m_DeltaTime;
	long m_DestinationX;
	long m_DestinationY;

	long m_hObjectVB;
	long m_iPrimitiveType;
	long m_iPrimitiveCount;
	CMatrix m_Matrix;

	long m_hTextureNormal;
	long m_hTextureLight;
	
	long m_hShaderNormal;
	long m_hShaderLight;


	bool m_bHighlighted;
	
	std::vector<CVector> m_IntersectDataHolder;
};

#endif