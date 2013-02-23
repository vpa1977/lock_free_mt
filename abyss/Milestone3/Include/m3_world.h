#ifndef M3_WORLD_HPP_INCLUDED
#define M3_WORLD_HPP_INCLUDED

#include "game_object.h"
#include "load_proxy.h"
#include "render_proxy.h"
#include "m3_input_translator.h"

#include <assert.h>

class CWorld :
	public CGameObjectOwner
	,public CGameObject
	,public CCameraInfoProvider
	,public CLoadUser
	,public CRenderUser
{
public:
	CWorld( CGameObjectOwner* in_pOwner );
	~CWorld();

/////////////////////////////////////////////////
	// virtual from CLoadUser
public:
	void ReadFileResult( bool in_bSuccess, const std::string& in_rFileName, const std::vector<unsigned char>& in_rData );
	void DecodeFileResult( bool in_bSuccess, const std::string& in_rFileName, long in_Width, long in_Height, const std::vector<unsigned char>& in_rData );
	void DecodeRawResult( bool in_bSuccess, long in_Width, long in_Height, const std::vector<unsigned char>& in_rData );

/////////////////////////////////////////////////
	// virtual from CRenderUser
public:
	void CreateVBResult( unsigned long in_VBHandle );
	void UpdateVBResult( bool in_bSuccess );
	void CreateTextureResult( unsigned long in_TexHandle );
	void CreatePSResult( unsigned long in_ShaderHandle );
	void CreateFontResult(unsigned long in_FontHandle);

/////////////////////////////////////////////////
	// virtual from CGameObjectOwner
public:
	void SomeObjectPreloadComplete( bool in_bSuccess, void* in_Handle );
/////////////////////////////////////////////////

/////////////////////////////////////////////////
	// virtual from CGameObject
protected:
	void vStartPreload();
	void vUpdateBeforeChildren( unsigned long in_Time );
	void vUpdateAfterChildren();
/////////////////////////////////////////////////
	bool vAcceptMouse(const CVector& in_Org, const CVector& in_Dir, bool in_bClickDetected) {return false;}
	bool vAcceptKey(unsigned int code);
/////////////////////////////////////////////////
	// virtual from CCameraInfoProvider
public:
	CCameraInfo& GetCameraInfo();
/////////////////////////////////////////////////

protected:

private:
	void CheckPreloadComplete();
	std::set<CGameObject*> m_ChildrenWithPendingPreload;
	bool m_bSelfPreloadComplete;
	bool m_bWasErrorDuringPreload;
	


	CVector m_CamOrg;
	CVector m_CamDir;
	CVector m_CamRight;
	float m_FOVX;
	float m_FOVY;
	float m_ZNear;
	float m_ZFar;
};

#endif


























