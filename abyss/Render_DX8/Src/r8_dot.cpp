#include "../Include/r8.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

void
CRenderDX8::CreateDotShader()
{
	CPixelShader PSh;
	PSh.m_Passes.resize(1);
	CPixelShaderPass& rPass = PSh.m_Passes[0];
	rPass.m_CullMode = RCM_CCW;
	rPass.m_SrcBlend = RBM_ONE;
	rPass.m_DstBlend = RBM_ZERO;
	rPass.m_bLightingEnabled = false;
	rPass.m_TFactor = 0;
	rPass.m_bAlphaTest = false;
	rPass.m_bDepthWrite = false;
	rPass.m_bDepthEnabled = false;
	rPass.m_ZCmpMode = RCMP_LESSEQUAL;
	rPass.m_Stages.resize(1);
	{
		// pass 0, stage 0
		CPixelShaderPassStage& rStage = rPass.m_Stages[0];
		//
		rStage.m_AlphaArg1 = RTSA_TEXTURE;
		rStage.m_AlphaArg2 = RTSA_DIFFUSE;
		rStage.m_AlphaOp = RTSO_SELECTARG2;
		//
		rStage.m_ColorArg1 = RTSA_TEXTURE;
		rStage.m_ColorArg2 = RTSA_DIFFUSE;
		rStage.m_ColorOp = RTSO_SELECTARG2;
		//
		rStage.m_TextureAddressModeU = RTAM_CLAMP;
		rStage.m_TextureAddressModeV = RTAM_CLAMP;
		rStage.m_TextureIndex = 0;
		rStage.m_TextureMagFilter = RTFM_LINEAR;
		rStage.m_TextureMinFilter = RTFM_LINEAR;
		rStage.m_TextureMipFilter = RTFM_NONE;
	}
	m_DotShaderHandle = CreatePixelShader( PSh );
	assert( m_DotShaderHandle != 0 );
}

struct DOT_VERTEX
{
	float x,y,z;
	float rhw;
	unsigned long color;
};

void
CRenderDX8::DrawDot( long in_X, long in_Y, unsigned long in_Color)
{
	DOT_VERTEX Vertex;
	Vertex.x = (float)in_X;
	Vertex.y = (float)in_Y;
	Vertex.z = 0.1f;
	Vertex.rhw = 1.0f;
	Vertex.color = in_Color;

	HRESULT Res;
	Res = m_pD3D8Device->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
	assert(Res==D3D_OK);
	Res = m_pD3D8Device->BeginScene();
	assert(Res==D3D_OK);
	unsigned long NPasses = m_PixelShaderManager.UsePixelShader(m_DotShaderHandle);
	assert(NPasses>0);
	for(unsigned long i=0; i<NPasses; ++i)
	{
		m_PixelShaderManager.ApplyPixelShaderPass(i);
		Res = m_pD3D8Device->DrawPrimitiveUP(
			D3DPT_POINTLIST,
			1,
			&Vertex,
			sizeof(DOT_VERTEX)
		);
		assert(Res==D3D_OK);
	}
	Res = m_pD3D8Device->EndScene();
	assert(Res==D3D_OK);
}






















