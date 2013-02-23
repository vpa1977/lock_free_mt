#ifndef R8_TEXTURE_MANAGER_HPP_INCLUDED
#define R8_TEXTURE_MANAGER_HPP_INCLUDED

#include "handle_storage.h"
#include <d3d8.h>

#include <map>

// --------------------------------------------------------------------------------------------------------------

class CRenderDX8TextureManager
{
public:
	CRenderDX8TextureManager();
	~CRenderDX8TextureManager();

	void ReleaseEverything();

	void SetInterface( IDirect3D8* in_pD3D8, IDirect3DDevice8* in_pD3D8Device, D3DFORMAT in_RenderFormat );

	unsigned long CreateTexture(
		unsigned long in_DX
		,unsigned long in_DY
		// TODO: insert in_InputFormat. ATM input is always 32bit (X8R8G8B8 или A8R8G8B8)
		,bool in_HasAlpha // TODO: get rid of this once in_InputFormat is implemented.
		// TODO: implement bool in_InputHasMipmaps
		// TODO: implement bool in_GenerateMipmaps
		,const void* in_pData, unsigned long in_DataLength
	);
	void ReleaseTexture( unsigned long in_TextureHandle );

	IDirect3DTexture8* GetTexture( unsigned long in_TextureHandle );
private:
	IDirect3D8*			m_pD3D8;
	IDirect3DDevice8*	m_pD3D8Device;
	D3DFORMAT			m_RenderFormat;

	void ChooseTextureFormats();
	inline bool CheckTextureFormat(D3DFORMAT in_TextureFormat);
	D3DFORMAT m_PreferredTextureFormat;
	D3DFORMAT m_PreferredTextureFormatAlpha;

	CHandleStorage m_TexturesHandlesStorage;
	struct CTextureInfo
	{
		CTextureInfo() : m_DX(0), m_DY(0), m_Format(D3DFMT_UNKNOWN), m_pTex(NULL) {}
		CTextureInfo(unsigned long in_DX, unsigned long in_DY, D3DFORMAT in_Format, IDirect3DTexture8* in_pTex) :
			m_DX(in_DX), m_DY(in_DY),
			m_Format(in_Format),
			m_pTex(in_pTex)
		{}
		unsigned long m_DX;
		unsigned long m_DY;
		D3DFORMAT m_Format;
		IDirect3DTexture8* m_pTex;
	};
	typedef std::map< unsigned long, CTextureInfo > CTextureInfoMap;
	typedef CTextureInfoMap::value_type CTextureInfoMapValueType;
	typedef CTextureInfoMap::iterator CTextureInfoMapIterator;
	typedef std::pair<CTextureInfoMapIterator,bool> CTextureInfoMapInsertResult;
	CTextureInfoMap m_TexMap;
};

#endif






























