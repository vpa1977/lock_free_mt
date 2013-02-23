#ifndef RENDER_PIXEL_SHADER_HPP_INCLUDED
#define RENDER_PIXEL_SHADER_HPP_INCLUDED

#include "render_enums.h"
#include <vector>

struct CPixelShaderPassStage
{
	CPixelShaderPassStage() :
		m_ColorArg1(RTSA_UNDEFINED),
		m_ColorArg2(RTSA_UNDEFINED),
		m_ColorOp(RTSO_DISABLE),
		m_AlphaArg1(RTSA_UNDEFINED),
		m_AlphaArg2(RTSA_UNDEFINED),
		m_AlphaOp(RTSO_DISABLE),
		m_TextureIndex(0),
		m_TextureAddressModeU(RTAM_WRAP),
		m_TextureAddressModeV(RTAM_WRAP),
		m_TextureMagFilter(RTFM_POINT),
		m_TextureMinFilter(RTFM_POINT),
		m_TextureMipFilter(RTFM_NONE)
	{}
	CPixelShaderPassStage(
		RENDER_TEXTURE_STAGE_ARGUMENT in_ColorArg1,
		RENDER_TEXTURE_STAGE_ARGUMENT in_ColorArg2,
		RENDER_TEXTURE_STAGE_OPERATION in_ColorOp,
		RENDER_TEXTURE_STAGE_ARGUMENT in_AlphaArg1,
		RENDER_TEXTURE_STAGE_ARGUMENT in_AlphaArg2,
		RENDER_TEXTURE_STAGE_OPERATION in_AlphaOp,
		unsigned long in_TextureIndex,
		RENDER_TEXTURE_ADDRESS_MODE in_TextureAddressModeU,
		RENDER_TEXTURE_ADDRESS_MODE in_TextureAddressModeV,
		RENDER_TEXTURE_FILTER_MODE in_TextureMagFilter,
		RENDER_TEXTURE_FILTER_MODE in_TextureMinFilter,
		RENDER_TEXTURE_FILTER_MODE in_TextureMipFilter
	) :
		m_ColorArg1(in_ColorArg1),
		m_ColorArg2(in_ColorArg2),
		m_ColorOp(in_ColorOp),
		m_AlphaArg1(in_AlphaArg1),
		m_AlphaArg2(in_AlphaArg2),
		m_AlphaOp(in_AlphaOp),
		m_TextureIndex(in_TextureIndex),
		m_TextureAddressModeU(in_TextureAddressModeU),
		m_TextureAddressModeV(in_TextureAddressModeV),
		m_TextureMagFilter(in_TextureMagFilter),
		m_TextureMinFilter(in_TextureMinFilter),
		m_TextureMipFilter(in_TextureMipFilter)
	{}
	// stage setup
	RENDER_TEXTURE_STAGE_ARGUMENT m_ColorArg1;
	RENDER_TEXTURE_STAGE_ARGUMENT m_ColorArg2;
	RENDER_TEXTURE_STAGE_OPERATION m_ColorOp;
	RENDER_TEXTURE_STAGE_ARGUMENT m_AlphaArg1;
	RENDER_TEXTURE_STAGE_ARGUMENT m_AlphaArg2;
	RENDER_TEXTURE_STAGE_OPERATION m_AlphaOp;
	// texture
	unsigned long m_TextureIndex;
	RENDER_TEXTURE_ADDRESS_MODE m_TextureAddressModeU;
	RENDER_TEXTURE_ADDRESS_MODE m_TextureAddressModeV;
	RENDER_TEXTURE_FILTER_MODE m_TextureMagFilter;
	RENDER_TEXTURE_FILTER_MODE m_TextureMinFilter;
	RENDER_TEXTURE_FILTER_MODE m_TextureMipFilter;
};

struct CPixelShaderPass
{
	CPixelShaderPass() :
		m_CullMode(RCM_NONE),
		m_SrcBlend(RBM_ONE),
		m_DstBlend(RBM_ZERO),
		m_bLightingEnabled(false),
		m_TFactor(0),
		m_bAlphaTest(false),
		m_AlphaCmpMode(RCMP_ALWAYS),
		m_AlphaRef(0),
		m_bDepthWrite(false),
		m_bDepthEnabled(true),
		m_ZCmpMode(RCMP_LESSEQUAL)
	{}
	CPixelShaderPass(
		const std::vector<CPixelShaderPassStage>& in_Stages,
		const std::vector<unsigned long>& in_Textures,
		RENDER_CULL_MODE in_CullMode,
		RENDER_BLEND_MODE in_SrcBlend,
		RENDER_BLEND_MODE in_DstBlend,
		bool in_bLightingEnabled,
		unsigned long in_TFactor,
		bool in_bAlphaTest,
		RENDER_CMP_MODE in_AlphaCmpMode,
		unsigned long in_AlphaRef,
		bool in_bDepthWrite,
		bool in_bDepthEnabled,
		RENDER_CMP_MODE in_ZCmpMode
	) :
		m_Stages(in_Stages),
		m_Textures(in_Textures),
		m_CullMode(in_CullMode),
		m_SrcBlend(in_SrcBlend),
		m_DstBlend(in_DstBlend),
		m_bLightingEnabled(in_bLightingEnabled),
		m_TFactor(in_TFactor),
		m_bAlphaTest(in_bAlphaTest),
		m_AlphaCmpMode(in_AlphaCmpMode),
		m_AlphaRef(in_AlphaRef),
		m_bDepthWrite(in_bDepthWrite),
		m_bDepthEnabled(in_bDepthEnabled),
		m_ZCmpMode(in_ZCmpMode)
	{}
	//
	std::vector<CPixelShaderPassStage> m_Stages;
	std::vector<unsigned long> m_Textures;
	//
	RENDER_CULL_MODE	m_CullMode;
	//
	RENDER_BLEND_MODE	m_SrcBlend;
	RENDER_BLEND_MODE	m_DstBlend;
	//
	bool				m_bLightingEnabled;
	//
	unsigned long		m_TFactor;
	//
	bool				m_bAlphaTest;
	RENDER_CMP_MODE		m_AlphaCmpMode;
	unsigned long		m_AlphaRef;
	//
	bool				m_bDepthWrite;
	bool				m_bDepthEnabled;
	RENDER_CMP_MODE		m_ZCmpMode;
};

struct CPixelShader
{
	CPixelShader() {}
	CPixelShader(const std::vector<CPixelShaderPass>& in_Passes) : m_Passes(in_Passes) {}
	std::vector<CPixelShaderPass> m_Passes;
};

#endif






























