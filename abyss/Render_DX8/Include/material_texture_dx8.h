#ifndef MATERIAL_TEXTURE_DX8_HPP_INCLUDED
#define MATERIAL_TEXTURE_DX8_HPP_INCLUDED

#include "../../Objects/Include/Render/texture_map.h"

#include "boost/shared_ptr.hpp"
#include <d3d8.h>
#include <assert.h>


//struct CTextureMapDX8 {
//	unsigned long		m_Width;
//	unsigned long		m_Height;
//	IDirect3DTexture8*	m_pTexture;
//};
//
//struct CTextureDX8 {
//	D3DTEXTUREADDRESS	m_WrapModeU;
//	D3DTEXTUREADDRESS	m_WrapModeV;
//	boost::shared_ptr< CTextureMapDX8 > m_pTextureMap;
//};

class CTextureContainer {
public:
	virtual void DecrementUsageCounter( unsigned long in_TextureHandle ) = 0;
};

struct CTextureDX8_Wrapper {
	//unsigned long		m_Width;
	//unsigned long		m_Height;
	//D3DTEXTUREADDRESS	m_WrapModeU;
	//D3DTEXTUREADDRESS	m_WrapModeV;
	unsigned long		m_TextureHandle;
	//
	CTextureContainer*	m_pContainer;
	//
	CTextureDX8_Wrapper() : m_TextureHandle(0), m_pContainer(NULL) {}
	CTextureDX8_Wrapper(
		//D3DTEXTUREADDRESS				in_WrapModeU,
		//D3DTEXTUREADDRESS				in_WrapModeV,
		unsigned long					in_TextureHandle,
		CTextureContainer*				in_pContainer
	) :
		//m_WrapModeU(in_WrapModeU),
		//m_WrapModeV(in_WrapModeV),
		m_TextureHandle(in_TextureHandle),
		m_pContainer(in_pContainer)
	{
		assert( m_TextureHandle > 0 );
		assert( m_pContainer );
	}
	//
	~CTextureDX8_Wrapper() {
		if( m_pContainer ) {
			assert( m_TextureHandle > 0 );
			m_pContainer->DecrementUsageCounter( m_TextureHandle );
		}
	}

	void Set(unsigned long in_TextureHandle, CTextureContainer* in_pContainer ) {
		assert( m_TextureHandle == 0 );
		assert( !m_pContainer );
		m_TextureHandle = in_TextureHandle;
		m_pContainer = in_pContainer;
		assert( m_TextureHandle > 0 );
		assert( m_pContainer );
	}
};

#endif























