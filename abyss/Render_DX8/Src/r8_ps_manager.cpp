#include "../Include/r8_ps_manager.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

// ---------------------------------------------------------------------------------------------------------------

CRenderDX8PixelShaderManager::CRenderDX8PixelShaderManager() :
	m_pD3D8(NULL),
	m_pD3D8Device(NULL),
	m_pTextureManager(NULL),
	m_pObjectTexturesContainer(NULL)
{
	m_ActiveShaderIt = m_PShaderMap.end();
}

CRenderDX8PixelShaderManager::~CRenderDX8PixelShaderManager()
{
	assert( !m_pD3D8 && !m_pD3D8Device && !m_pTextureManager );
	assert( m_PShaderMap.empty() );
}

void
CRenderDX8PixelShaderManager::ReleaseEverything()
{
	m_PShaderMap.clear();
	m_PSHandlesStorage.Clear();
}

void
CRenderDX8PixelShaderManager::SetInterface( IDirect3D8* in_pD3D8, IDirect3DDevice8* in_pD3D8Device, CRenderDX8TextureManager* in_pTextureManager, CRenderDX8_ObjectsTexturesContainer* in_pObjectTexturesContainer )
{
	assert( (in_pD3D8 && in_pD3D8Device && in_pTextureManager) || (!in_pD3D8 && !in_pD3D8Device && !in_pTextureManager) );
	if(in_pD3D8)
	{
		assert( !m_pD3D8 && !m_pD3D8Device );
		m_pD3D8 = in_pD3D8;
		m_pD3D8Device = in_pD3D8Device;
		m_pTextureManager = in_pTextureManager;
		m_pD3D8Device->GetDeviceCaps( &m_DeviceCaps );
	}
	else
	{
		assert(m_PShaderMap.empty());
		m_pD3D8 = NULL;
		m_pD3D8Device = NULL;
		m_pTextureManager = NULL;
	}
	m_pObjectTexturesContainer = in_pObjectTexturesContainer;
}



void
CRenderDX8PixelShaderManager::ReleasePixelShader( unsigned long in_PixelShaderHandle )
{
	CPixelShaderInfoMapIterator It = m_PShaderMap.find( in_PixelShaderHandle );
	assert( It != m_PShaderMap.end() );
	It->second.m_pPS.reset(); // kinda release
	m_PShaderMap.erase( It );
	m_PSHandlesStorage.ReleaseHandle( in_PixelShaderHandle );
	m_ActiveShaderIt = m_PShaderMap.end();
}

unsigned long
CRenderDX8PixelShaderManager::UsePixelShader( unsigned long in_PixelShaderHandle )
{
	CPixelShaderInfoMapIterator It = m_PShaderMap.find( in_PixelShaderHandle );
	if( It != m_PShaderMap.end() )
	{
		m_ActiveShaderIt = It;
		return (unsigned long)m_ActiveShaderIt->second.m_pPS->m_Passes.size();
	}
	else
		return 0;
}

void
CRenderDX8PixelShaderManager::ApplyPixelShaderPass( unsigned long in_PassNum )
{
	assert( m_pD3D8Device!=NULL );
	assert( m_pTextureManager!=NULL );
	assert( m_ActiveShaderIt != m_PShaderMap.end() );
	assert( in_PassNum < m_ActiveShaderIt->second.m_pPS->m_Passes.size() );
	const CPSPassInternal& rPass = m_ActiveShaderIt->second.m_pPS->m_Passes[in_PassNum];
	// setup states
	const std::vector< CSetRenderState >& rSetRenderStates = rPass.m_PassStates;
	for(	std::vector< CSetRenderState >::const_iterator SRSIt = rSetRenderStates.begin();
			SRSIt!=rSetRenderStates.end();
			++SRSIt
	)
	{
		m_pD3D8Device->SetRenderState( SRSIt->m_State, SRSIt->m_Value );
	}
	// setup textures
	unsigned long TexNum=0;
	for( ; TexNum<rPass.m_Textures.size(); ++TexNum )
	{
		IDirect3DTexture8* pTex = m_pTextureManager->GetTexture( rPass.m_Textures[TexNum] );
		assert( pTex );
		m_pD3D8Device->SetTexture( TexNum, pTex );
	}
	for( ; TexNum<8; ++TexNum )
	{
		m_pD3D8Device->SetTexture( TexNum, NULL );
	}
	// setup stages
	const std::vector< CPSStageInternal >& rStages = rPass.m_Stages;
	unsigned long StageNum=0;
	for( ; StageNum<rStages.size(); ++StageNum )
	{
		const CPSStageInternal& rStage = rStages[StageNum];
		const std::vector< CSetTextureStageState >& rSetTextureStageStates = rStage.m_StageStates;
		for(	std::vector< CSetTextureStageState >::const_iterator STSSIt = rSetTextureStageStates.begin();
				STSSIt!=rSetTextureStageStates.end();
				++STSSIt
		)
		{
			m_pD3D8Device->SetTextureStageState( StageNum, STSSIt->m_State, STSSIt->m_Value );
		}
	}
	if(StageNum<=7)
	{
		m_pD3D8Device->SetTextureStageState(StageNum,D3DTSS_COLOROP,D3DTOP_DISABLE);
		m_pD3D8Device->SetTextureStageState(StageNum,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
	}
}

// ---------------------------------------------------------------------------------------------------------------

D3DCULL
CRenderDX8PixelShaderManager::Convert_RENDER_CULL_MODE_to_D3DCULL(RENDER_CULL_MODE in_Arg)
{
	if(in_Arg==RCM_CW)
		return D3DCULL_CW;
	else if(in_Arg==RCM_CCW)
		return D3DCULL_CCW;
	else // in_Arg==RCM_NONE
		return D3DCULL_NONE;
}

D3DBLEND
CRenderDX8PixelShaderManager::Convert_RENDER_BLEND_MODE_to_D3DBLEND(RENDER_BLEND_MODE in_Arg)
{
	if(in_Arg==RBM_SRCALPHASAT)
		return D3DBLEND_SRCALPHASAT;
	else if(in_Arg==RBM_INVDESTCOLOR)
		return D3DBLEND_INVDESTCOLOR;
	else if(in_Arg==RBM_DESTCOLOR)
		return D3DBLEND_DESTCOLOR;
	else if(in_Arg==RBM_INVDESTALPHA)
		return D3DBLEND_INVDESTALPHA;
	else if(in_Arg==RBM_DESTALPHA)
		return D3DBLEND_DESTALPHA;
	else if(in_Arg==RBM_INVSRCALPHA)
		return D3DBLEND_INVSRCALPHA;
	else if(in_Arg==RBM_SRCALPHA)
		return D3DBLEND_SRCALPHA;
	else if(in_Arg==RBM_INVSRCCOLOR)
		return D3DBLEND_INVSRCCOLOR;
	else if(in_Arg==RBM_SRCCOLOR)
		return D3DBLEND_SRCCOLOR;
	else if(in_Arg==RBM_ONE)
		return D3DBLEND_ONE;
	else // in_Arg==RBM_ZERO
		return D3DBLEND_ZERO;
}

D3DCMPFUNC
CRenderDX8PixelShaderManager::Convert_RENDER_CMP_MODE_to_D3DCMPFUNC(RENDER_CMP_MODE in_Arg)
{
	if(in_Arg==RCMP_NOTEQUAL)
		return D3DCMP_NOTEQUAL;
	else if(in_Arg==RCMP_GREATEREQUAL)
		return D3DCMP_GREATEREQUAL;
	else if(in_Arg==RCMP_GREATER)
		return D3DCMP_GREATER;
	else if(in_Arg==RCMP_LESSEQUAL)
		return D3DCMP_LESSEQUAL;
	else if(in_Arg==RCMP_LESS)
		return D3DCMP_LESS;
	else if(in_Arg==RCMP_EQUAL)
		return D3DCMP_EQUAL;
	else if(in_Arg==RCMP_NEVER)
		return D3DCMP_NEVER;
	else // RCMP_ALWAYS
		return D3DCMP_ALWAYS;
}

DWORD
CRenderDX8PixelShaderManager::Convert_RENDER_TEXTURE_STAGE_ARGUMENT(RENDER_TEXTURE_STAGE_ARGUMENT in_Arg)
{
	if(in_Arg==RTSA_TFACTOR)
		return D3DTA_TFACTOR;
	else if(in_Arg==RTSA_TEXTURE)
		return D3DTA_TEXTURE;
	else if(in_Arg==RTSA_SPECULAR)
		return D3DTA_SPECULAR;
	else if(in_Arg==RTSA_DIFFUSE)
		return D3DTA_DIFFUSE;
	else if(in_Arg==RTSA_CURRENT)
		return D3DTA_CURRENT;
	else // RTSA_UNDEFINED
	{
		assert(false);
		return D3DTA_DIFFUSE;
	}
}

DWORD
CRenderDX8PixelShaderManager::Convert_RENDER_TEXTURE_STAGE_OPERATION(RENDER_TEXTURE_STAGE_OPERATION in_Arg)
{
	if(in_Arg==RTSO_MODULATEINVALPHA_ADDCOLOR)
		return D3DTOP_MODULATEINVALPHA_ADDCOLOR;
	else if(in_Arg==RTSO_MODULATEINVCOLOR_ADDALPHA)
		return D3DTOP_MODULATEINVCOLOR_ADDALPHA;
	else if(in_Arg==RTSO_MODULATECOLOR_ADDALPHA)
		return D3DTOP_MODULATECOLOR_ADDALPHA;
	else if(in_Arg==RTSO_MODULATEALPHA_ADDCOLOR)
		return D3DTOP_MODULATECOLOR_ADDALPHA ;
	else if(in_Arg==RTSO_PREMUL)
		return D3DTOP_PREMODULATE;
	else if(in_Arg==RTSO_BLEND_ALPHAPREMUL)
		return D3DTOP_BLENDTEXTUREALPHAPM;
	else if(in_Arg==RTSO_BLEND_CURRENTALPHA)
		return D3DTOP_BLENDCURRENTALPHA;
	else if(in_Arg==RTSO_BLEND_TFACTORALPHA)
		return D3DTOP_BLENDFACTORALPHA;
	else if(in_Arg==RTSO_BLEND_TEXALPHA)
		return D3DTOP_BLENDTEXTUREALPHA;
	else if(in_Arg==RTSO_BLEND_DIFFUSEALPHA)
		return D3DTOP_BLENDDIFFUSEALPHA;
	else if(in_Arg==RTSO_ADDSMOOTH)
		return D3DTOP_ADDSMOOTH;
	else if(in_Arg==RTSO_SUB)
		return D3DTOP_SUBTRACT;
	else if(in_Arg==RTSO_ADDSIGNED2)
		return D3DTOP_ADDSIGNED2X;
	else if(in_Arg==RTSO_ADDSIGNED)
		return D3DTOP_ADDSIGNED;
	else if(in_Arg==RTSO_ADD)
		return D3DTOP_ADD;
	else if(in_Arg==RTSO_MODULATE4)
		return D3DTOP_MODULATE4X;
	else if(in_Arg==RTSO_MODULATE2)
		return D3DTOP_MODULATE2X;
	else if(in_Arg==RTSO_MODULATE)
		return D3DTOP_MODULATE;
	else if(in_Arg==RTSO_SELECTARG2)
		return D3DTOP_SELECTARG2;
	else if(in_Arg==RTSO_SELECTARG1)
		return D3DTOP_SELECTARG1;
	else // in_Arg==RTSO_DISABLE
		return D3DTOP_DISABLE;
}

DWORD
CRenderDX8PixelShaderManager::Convert_RENDER_TEXTURE_ADDRESS_MODE(RENDER_TEXTURE_ADDRESS_MODE in_Arg)
{
	if(in_Arg==RTAM_MIRROR)
		return D3DTADDRESS_MIRROR;
	else if(in_Arg==RTAM_CLAMP)
		return D3DTADDRESS_CLAMP;
	else if(in_Arg==RTAM_WRAP)
		return D3DTADDRESS_WRAP;
	else // RTAM_UNDEFINED
	{
		assert(false);
		return D3DTADDRESS_WRAP;
	}
}

DWORD
CRenderDX8PixelShaderManager::Convert_RENDER_TEXTURE_FILTER_MODE(RENDER_TEXTURE_FILTER_MODE in_Arg)
{
	if(in_Arg==RTFM_ANISOTROPIC)
		return D3DTEXF_ANISOTROPIC;
	else if(in_Arg==RTFM_LINEAR)
		return D3DTEXF_LINEAR;
	else if(in_Arg==RTFM_POINT)
		return D3DTEXF_POINT;
	else // RTFM_NONE
		return D3DTEXF_NONE;
}
















