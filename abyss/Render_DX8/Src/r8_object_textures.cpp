#include "../Include/r8_object_textures.h"
#include <assert.h>

CRenderDX8_ObjectsTexturesContainer::CRenderDX8_ObjectsTexturesContainer() {
}

CRenderDX8_ObjectsTexturesContainer::~CRenderDX8_ObjectsTexturesContainer() {
	assert( m_TextureToHandle.empty() );
	assert( m_HandleToTexture.empty() );
}

void
CRenderDX8_ObjectsTexturesContainer::SetInterface( CRenderDX8TextureManager* in_pTextureManager ) {
	m_pTextureManager = in_pTextureManager;
}

unsigned long
CRenderDX8_ObjectsTexturesContainer::UseTexture( CRenderable_AbstractTextureMap* in_pTexture ) {
	assert( in_pTexture );
	assert( m_pTextureManager );

	CTextureToHandleConvertMap::iterator ItTexture = m_TextureToHandle.find( in_pTexture );
	if( ItTexture != m_TextureToHandle.end() ) {
		unsigned long Handle = ItTexture->second;
		CHandleToTextureConvertMap::iterator ItHandle = m_HandleToTexture.find( Handle );
		assert( ItHandle != m_HandleToTexture.end() );
		CCounterTexture& rCounter( ItHandle->second );
		rCounter.first++;
		return Handle;
	} else {
		unsigned long Handle = m_pTextureManager->CreateTexture( in_pTexture->m_Width, in_pTexture->m_Height, in_pTexture->m_bHasAlpha, &(in_pTexture->m_Data[0]), (unsigned long)(in_pTexture->m_Data.size()*sizeof(unsigned long)) );
		assert( Handle > 0 );
		m_TextureToHandle[ in_pTexture ] = Handle;
		CHandleToTextureConvertMap::iterator ItHandle = m_HandleToTexture.find( Handle );
		assert( ItHandle == m_HandleToTexture.end() );
		m_HandleToTexture[ Handle ] = CCounterTexture( 1, in_pTexture );
		return Handle;
	}
}

void
CRenderDX8_ObjectsTexturesContainer::DecrementUsageCounter( unsigned long in_TextureHandle ) {
	assert( in_TextureHandle > 0 );
	assert( m_pTextureManager );

	CHandleToTextureConvertMap::iterator ItHandle = m_HandleToTexture.find( in_TextureHandle );
	assert( ItHandle != m_HandleToTexture.end() );
	CCounterTexture& rCounter( ItHandle->second );
	if( (--rCounter.first) == 0 ) {
		CRenderable_AbstractTextureMap* pTex = rCounter.second;
		assert( pTex );
		CTextureToHandleConvertMap::iterator ItTexture = m_TextureToHandle.find( pTex );
		assert( ItTexture != m_TextureToHandle.end() );
		m_TextureToHandle.erase( ItTexture );
		//
		m_HandleToTexture.erase( ItHandle );
	}
}







