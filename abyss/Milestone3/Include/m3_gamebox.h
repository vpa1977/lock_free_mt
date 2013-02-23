#ifndef M3_GAMEBOX_HPP_INCLUDED
#define M3_GAMEBOX_HPP_INCLUDED

#include "game_object.h"
#include "load_proxy.h"
#include "render_proxy.h"
#include <string>


class CGameBox :
	public CGameObject
	,public CLoadUser
	,public CRenderUser
{
public:
	CGameBox( CGameObjectOwner* in_pOwner );

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

protected:
	void vStartPreload();
	void vUpdateBeforeChildren( unsigned long in_Time );
	void vUpdateAfterChildren() {}
	bool vAcceptKey(unsigned int scanCode ) { return false; }
	bool vAcceptMouse(const CVector& in_Org, const CVector& in_Dir, bool in_bClickDetected) {return false;}

private:
	bool m_bPreloadError;

	unsigned long m_TextureHandle;
	unsigned long m_PSHandle;

	unsigned long m_PrimitiveCount;
	unsigned long m_PrimitiveType;
	unsigned long m_VBHandle;

	CMatrix m_Matrix;

	void CheckPreloadComplete();
};

#endif