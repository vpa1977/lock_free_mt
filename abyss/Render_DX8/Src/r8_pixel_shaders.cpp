#include "../Include/r8.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

unsigned long
CRenderDX8::CreatePixelShader( const CPixelShader& in_rShaderDescription )
{
	return m_PixelShaderManager.CreatePixelShader( in_rShaderDescription );
}

void
CRenderDX8::ReleasePixelShader( unsigned long in_PixelShaderHandle )
{
	m_PixelShaderManager.ReleasePixelShader( in_PixelShaderHandle );
}
