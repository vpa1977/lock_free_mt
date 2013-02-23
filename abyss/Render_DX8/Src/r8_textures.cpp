#include "../Include/r8.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

unsigned long
CRenderDX8::CreateTexture(
	unsigned long in_DX
	,unsigned long in_DY
	,bool in_HasAlpha
	,const void* in_pData, unsigned long in_DataLength
)
{
	return m_TextureManager.CreateTexture(
		in_DX
		,in_DY
		,in_HasAlpha
		,in_pData,in_DataLength
	);
}

void
CRenderDX8::ReleaseTexture( unsigned long in_TextureHandle )
{
	m_TextureManager.ReleaseTexture( in_TextureHandle );
}


























