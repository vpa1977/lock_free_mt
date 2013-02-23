#include "../Include/r8_ps_manager.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>


unsigned long
CRenderDX8PixelShaderManager::CreatePixelShader( const CPixelShader& in_rShaderDescription )
{
	//CLog::Print("CRenderDX8PixelShaderManager::CreatePixelShader( const CPixelShader& )\n");
	CPSInternal* pPS = new CPSInternal;
	//CLog::Print("  pPS=%08x\n",pPS);
	std::vector< CPSPassInternal >& rDstPasses = pPS->m_Passes;
	size_t NPasses = in_rShaderDescription.m_Passes.size();
	//CLog::Print("  NPasses=%lu\n",NPasses);
	rDstPasses.resize( NPasses );
	for(size_t i=0; i<rDstPasses.size(); ++i) {
		//CLog::Print("  Pass %lu\n",i);
		CPSPassInternal& rDstPass = rDstPasses[i];
		const CPixelShaderPass& rSrcPass = in_rShaderDescription.m_Passes[i];
		//
		rDstPass.m_Textures = rSrcPass.m_Textures;
		//
		size_t DstPassStatesSize =
			6 // cullmode, alphablendenable, srcblend, dstblend, lighting, tfactor
			+ 1 // alphatest
			+ (rSrcPass.m_bAlphaTest ? 2 : 0) // if alphatest, then alphafunc and alpharef
			+ 1 // zwrite
			+ 1 // zenable
			+ (rSrcPass.m_bDepthEnabled ? 1 : 0); // if zenable, then zfunc
//CLog::Print("    NStates=%lu\n",DstPassStatesSize);
		std::vector< CSetRenderState >& rDstPassStates = rDstPass.m_PassStates;
		rDstPassStates.resize( DstPassStatesSize );
		//
//CLog::Print("    filling\n");
		size_t n = 0;
		//
		assert( n < DstPassStatesSize );
		(rDstPassStates[n]).m_State = D3DRS_CULLMODE;
		(rDstPassStates[n]).m_Value = Convert_RENDER_CULL_MODE_to_D3DCULL( rSrcPass.m_CullMode );
		n++;
		//
		assert( n < DstPassStatesSize );
		(rDstPassStates[n]).m_State = D3DRS_ALPHABLENDENABLE;
		(rDstPassStates[n]).m_Value = (rSrcPass.m_SrcBlend==RBM_ONE && rSrcPass.m_DstBlend==RBM_ZERO ) ? FALSE : TRUE;
		n++;
		//
		assert( n < DstPassStatesSize );
		(rDstPassStates[n]).m_State = D3DRS_SRCBLEND;
		(rDstPassStates[n]).m_Value = Convert_RENDER_BLEND_MODE_to_D3DBLEND( rSrcPass.m_SrcBlend );
		n++;
		//
		assert( n < DstPassStatesSize );
		(rDstPassStates[n]).m_State = D3DRS_DESTBLEND;
		(rDstPassStates[n]).m_Value = Convert_RENDER_BLEND_MODE_to_D3DBLEND( rSrcPass.m_DstBlend );
		n++;
		//
		assert( n < DstPassStatesSize );
		(rDstPassStates[n]).m_State = D3DRS_LIGHTING;
		(rDstPassStates[n]).m_Value = rSrcPass.m_bLightingEnabled ? TRUE : FALSE;
		n++;
		//
		assert( n < DstPassStatesSize );
		(rDstPassStates[n]).m_State = D3DRS_TEXTUREFACTOR;
		(rDstPassStates[n]).m_Value = rSrcPass.m_TFactor;
		n++;
		//
		assert( n < DstPassStatesSize );
		(rDstPassStates[n]).m_State = D3DRS_ALPHATESTENABLE;
		(rDstPassStates[n]).m_Value = rSrcPass.m_bAlphaTest ? TRUE : FALSE;
		n++;
		//
		if(rSrcPass.m_bAlphaTest) {
			assert( n < DstPassStatesSize );
			(rDstPassStates[n]).m_State = D3DRS_ALPHAFUNC;
			(rDstPassStates[n]).m_Value = Convert_RENDER_CMP_MODE_to_D3DCMPFUNC( rSrcPass.m_AlphaCmpMode );
			n++;
			//
			assert( n < DstPassStatesSize );
			(rDstPassStates[n]).m_State = D3DRS_ALPHAREF;
			(rDstPassStates[n]).m_Value = rSrcPass.m_AlphaRef;
			n++;
		}
		//
		assert( n < DstPassStatesSize );
		(rDstPassStates[n]).m_State = D3DRS_ZWRITEENABLE;
		(rDstPassStates[n]).m_Value = rSrcPass.m_bDepthWrite ? TRUE : FALSE;
		n++;
		//
		assert( n < DstPassStatesSize );
		(rDstPassStates[n]).m_State = D3DRS_ZENABLE;
		(rDstPassStates[n]).m_Value = rSrcPass.m_bDepthEnabled ? D3DZB_TRUE : D3DZB_FALSE;
		n++;
		//
		if(rSrcPass.m_bDepthEnabled) {
			assert( n < DstPassStatesSize );
			(rDstPassStates[n]).m_State = D3DRS_ZFUNC;
			(rDstPassStates[n]).m_Value = Convert_RENDER_CMP_MODE_to_D3DCMPFUNC( rSrcPass.m_ZCmpMode );
			n++;
		}
		//
		std::vector< CPSStageInternal >& rDstStages = rDstPass.m_Stages;
		const std::vector<CPixelShaderPassStage>& rSrcStages = rSrcPass.m_Stages;
		rDstStages.resize( rSrcStages.size() );
		for(size_t j=0; j<rDstStages.size(); ++j) {
			CPSStageInternal& rDstStage = rDstStages[j];
			const CPixelShaderPassStage& rSrcStage = rSrcStages[j];
			//
			std::vector< CSetTextureStageState >& rDstStageStates = rDstStage.m_StageStates;
			rDstStageStates.resize( 12 );
			//
			rDstStageStates[0].m_State = D3DTSS_COLORARG1;
			rDstStageStates[0].m_Value = Convert_RENDER_TEXTURE_STAGE_ARGUMENT( rSrcStage.m_ColorArg1 );
			rDstStageStates[1].m_State = D3DTSS_COLORARG2;
			rDstStageStates[1].m_Value = Convert_RENDER_TEXTURE_STAGE_ARGUMENT( rSrcStage.m_ColorArg2 );
			rDstStageStates[2].m_State = D3DTSS_COLOROP;
			rDstStageStates[2].m_Value = Convert_RENDER_TEXTURE_STAGE_OPERATION( rSrcStage.m_ColorOp );
			rDstStageStates[3].m_State = D3DTSS_ALPHAARG1;
			rDstStageStates[3].m_Value = Convert_RENDER_TEXTURE_STAGE_ARGUMENT( rSrcStage.m_AlphaArg1 );
			rDstStageStates[4].m_State = D3DTSS_ALPHAARG2;
			rDstStageStates[4].m_Value = Convert_RENDER_TEXTURE_STAGE_ARGUMENT( rSrcStage.m_AlphaArg2 );
			rDstStageStates[5].m_State = D3DTSS_ALPHAOP;
			rDstStageStates[5].m_Value = Convert_RENDER_TEXTURE_STAGE_OPERATION( rSrcStage.m_AlphaOp );
			rDstStageStates[6].m_State = D3DTSS_TEXCOORDINDEX;
			rDstStageStates[6].m_Value = rSrcStage.m_TextureIndex;
			rDstStageStates[7].m_State = D3DTSS_ADDRESSU;
			rDstStageStates[7].m_Value = Convert_RENDER_TEXTURE_ADDRESS_MODE( rSrcStage.m_TextureAddressModeU );
			rDstStageStates[8].m_State = D3DTSS_ADDRESSV;
			rDstStageStates[8].m_Value = Convert_RENDER_TEXTURE_ADDRESS_MODE( rSrcStage.m_TextureAddressModeV );
			rDstStageStates[9].m_State = D3DTSS_MAGFILTER;
			rDstStageStates[9].m_Value = Convert_RENDER_TEXTURE_FILTER_MODE( rSrcStage.m_TextureMagFilter );
			rDstStageStates[10].m_State = D3DTSS_MINFILTER;
			rDstStageStates[10].m_Value = Convert_RENDER_TEXTURE_FILTER_MODE( rSrcStage.m_TextureMinFilter );
			rDstStageStates[11].m_State = D3DTSS_MIPFILTER;
			rDstStageStates[11].m_Value = Convert_RENDER_TEXTURE_FILTER_MODE( rSrcStage.m_TextureMipFilter );
		}
		//CLog::Print("    filled\n");
	}

	// remember it
	unsigned long PSHandle = m_PSHandlesStorage.GetNewHandle();
	assert(PSHandle>0);
	CPixelShaderInfoMapInsertResult PSIR = m_PShaderMap.insert(
		CPixelShaderInfoMapValueType(
			PSHandle,
			CPixelShaderInfo(pPS)
		)
	);
	assert( PSIR.second == true );

	m_ActiveShaderIt = m_PShaderMap.end();

	return PSHandle;
}


unsigned long
CRenderDX8PixelShaderManager::CreatePixelShader( const CRenderable_AbstractMaterial& in_rMaterial, std::vector< unsigned long >& in_rTextureHandles ) {
	//CLog::Print("CRenderDX8PixelShaderManager::CreatePixelShader( const CRenderable_AbstractMaterial& )\n");

	CShaderCreateData Data;
	bool Ok = PlanShader( in_rMaterial, Data );
	assert( Ok );

	// create
	CPSInternal* pPS = new CPSInternal;
	std::vector< CPSPassInternal >& rDstPasses = pPS->m_Passes;
	rDstPasses.resize( Data.m_Passes.size() );
	assert( m_pObjectTexturesContainer );
	for( size_t i=0; i<Data.m_Passes.size(); ++i ) {
		FillPass( in_rMaterial, Data.m_Passes[i], rDstPasses[i] );
	}
	// remember it
	unsigned long PSHandle = m_PSHandlesStorage.GetNewHandle();
	assert(PSHandle>0);
	CPixelShaderInfoMapInsertResult PSIR = m_PShaderMap.insert(
		CPixelShaderInfoMapValueType(
			PSHandle,
			CPixelShaderInfo(pPS)
		)
	);
	assert( PSIR.second == true );

	m_ActiveShaderIt = m_PShaderMap.end();

	return PSHandle;










/*
	assert( m_pObjectTexturesContainer );

	in_rTextureHandles.resize( 1 );
	in_rTextureHandles[0] = m_pObjectTexturesContainer->UseTexture( in_rMaterial.m_Diffuse.m_pTextureMap.get() );

	CPSInternal* pPS = new CPSInternal;
	//CLog::Print("  pPS=%08x\n",pPS);
	std::vector< CPSPassInternal >& rDstPasses = pPS->m_Passes;
	rDstPasses.resize( 1 );
	CPSPassInternal& rDstPass = rDstPasses[0];
	rDstPass.m_Textures = in_rTextureHandles;
	size_t DstPassStatesSize =
		6 // cullmode, alphablendenable, srcblend, dstblend, lighting, tfactor
		+ 1 // alphatest
		+ 0 // (rSrcPass.m_bAlphaTest ? 2 : 0) // if alphatest, then alphafunc and alpharef
		+ 1 // z_write
		+ 1 // z_enable
		+ 1; // (rSrcPass.m_bDepthEnabled ? 1 : 0); // if z_enable, then z_func
	std::vector< CSetRenderState >& rDstPassStates = rDstPass.m_PassStates;
	rDstPassStates.resize( DstPassStatesSize );

	size_t n = 0;
	//
	assert( n < DstPassStatesSize );
	(rDstPassStates[n]).m_State = D3DRS_CULLMODE;
	(rDstPassStates[n]).m_Value = in_rMaterial.m_bTwoSided ? D3DCULL_NONE : D3DCULL_CW;
	n++;
	//
	assert( n < DstPassStatesSize );
	(rDstPassStates[n]).m_State = D3DRS_ALPHABLENDENABLE;
	(rDstPassStates[n]).m_Value = FALSE;
	n++;
	//
	assert( n < DstPassStatesSize );
	(rDstPassStates[n]).m_State = D3DRS_SRCBLEND;
	(rDstPassStates[n]).m_Value = D3DBLEND_ONE;
	n++;
	//
	assert( n < DstPassStatesSize );
	(rDstPassStates[n]).m_State = D3DRS_DESTBLEND;
	(rDstPassStates[n]).m_Value = D3DBLEND_ZERO;
	n++;
	//
	assert( n < DstPassStatesSize );
	(rDstPassStates[n]).m_State = D3DRS_LIGHTING;
	(rDstPassStates[n]).m_Value = FALSE;
	n++;
	//
	assert( n < DstPassStatesSize );
	(rDstPassStates[n]).m_State = D3DRS_TEXTUREFACTOR;
	(rDstPassStates[n]).m_Value = 0xffffffff;
	n++;
	//
	assert( n < DstPassStatesSize );
	(rDstPassStates[n]).m_State = D3DRS_ALPHATESTENABLE;
	(rDstPassStates[n]).m_Value = FALSE;
	n++;
	//
	//if(rSrcPass.m_bAlphaTest) {
	//	assert( n < DstPassStatesSize );
	//	(rDstPassStates[n]).m_State = D3DRS_ALPHAFUNC;
	//	(rDstPassStates[n]).m_Value = Convert_RENDER_CMP_MODE_to_D3DCMPFUNC( rSrcPass.m_AlphaCmpMode );
	//	n++;
	//	//
	//	assert( n < DstPassStatesSize );
	//	(rDstPassStates[n]).m_State = D3DRS_ALPHAREF;
	//	(rDstPassStates[n]).m_Value = rSrcPass.m_AlphaRef;
	//	n++;
	//}
	//
	assert( n < DstPassStatesSize );
	(rDstPassStates[n]).m_State = D3DRS_ZWRITEENABLE;
	(rDstPassStates[n]).m_Value = TRUE;
	n++;
	//
	assert( n < DstPassStatesSize );
	(rDstPassStates[n]).m_State = D3DRS_ZENABLE;
	(rDstPassStates[n]).m_Value = D3DZB_TRUE;
	n++;
	//
	if( true ) {
		assert( n < DstPassStatesSize );
		(rDstPassStates[n]).m_State = D3DRS_ZFUNC;
		(rDstPassStates[n]).m_Value = D3DCMP_LESS;
		n++;
	}
	//
	std::vector< CPSStageInternal >& rDstStages = rDstPass.m_Stages;
	//const std::vector<CPixelShaderPassStage>& rSrcStages = rSrcPass.m_Stages;
	rDstStages.resize( 1 ); //rDstStages.resize( rSrcStages.size() );
	for(size_t j=0; j<rDstStages.size(); ++j) {
		CPSStageInternal& rDstStage = rDstStages[j];
		//const CPixelShaderPassStage& rSrcStage = rSrcStages[j];
		//
		std::vector< CSetTextureStageState >& rDstStageStates = rDstStage.m_StageStates;
		rDstStageStates.resize( 12 );
		//
		rDstStageStates[0].m_State = D3DTSS_COLORARG1;
		rDstStageStates[0].m_Value = D3DTA_TEXTURE;
		//
		rDstStageStates[1].m_State = D3DTSS_COLORARG2;
		rDstStageStates[1].m_Value = D3DTA_CURRENT;
		//
		rDstStageStates[2].m_State = D3DTSS_COLOROP;
		rDstStageStates[2].m_Value = D3DTOP_MODULATE;
		//
		rDstStageStates[3].m_State = D3DTSS_ALPHAARG1;
		rDstStageStates[3].m_Value = D3DTA_DIFFUSE;
		//
		rDstStageStates[4].m_State = D3DTSS_ALPHAARG2;
		rDstStageStates[4].m_Value = D3DTA_CURRENT;
		//
		rDstStageStates[5].m_State = D3DTSS_ALPHAOP;
		rDstStageStates[5].m_Value = D3DTOP_SELECTARG1;
		//
		rDstStageStates[6].m_State = D3DTSS_TEXCOORDINDEX;
		rDstStageStates[6].m_Value = 0;
		//
		rDstStageStates[7].m_State = D3DTSS_ADDRESSU;
		rDstStageStates[7].m_Value = D3DTADDRESS_WRAP;
		//
		rDstStageStates[8].m_State = D3DTSS_ADDRESSV;
		rDstStageStates[8].m_Value = D3DTADDRESS_WRAP;
		//
		rDstStageStates[9].m_State = D3DTSS_MAGFILTER;
		rDstStageStates[9].m_Value = D3DTEXF_LINEAR;
		//
		rDstStageStates[10].m_State = D3DTSS_MINFILTER;
		rDstStageStates[10].m_Value = D3DTEXF_LINEAR;
		//
		rDstStageStates[11].m_State = D3DTSS_MIPFILTER;
		rDstStageStates[11].m_Value = D3DTEXF_NONE;
	}

	// remember it
	unsigned long PSHandle = m_PSHandlesStorage.GetNewHandle();
	assert(PSHandle>0);
	CPixelShaderInfoMapInsertResult PSIR = m_PShaderMap.insert(
		CPixelShaderInfoMapValueType(
			PSHandle,
			CPixelShaderInfo(pPS)
		)
	);
	assert( PSIR.second == true );

	m_ActiveShaderIt = m_PShaderMap.end();

	return PSHandle;
*/
}

bool
CRenderDX8PixelShaderManager::PlanShader( const CRenderable_AbstractMaterial& in_rMaterial, CShaderCreateData& out_rData ) {
	out_rData.m_Passes.resize(1);
	out_rData.m_Passes[0].m_Content = CShaderCreateData::PC_DIFFUSE;
	out_rData.m_Passes[0].m_Blend = CShaderCreateData::PB_FROM_MATERIAL;
	out_rData.m_Passes[0].m_ZCmp = CShaderCreateData::PZ_FROM_MATERIAL;
	return true;
}

void
CRenderDX8PixelShaderManager::FillPass( const CRenderable_AbstractMaterial& in_rMaterial, const CShaderCreateData::CPassData& in_rWhat, CPSPassInternal& out_rDstPass ) {
	if( in_rWhat.m_Content == CShaderCreateData::PC_DIFFUSE ) {
		assert( in_rWhat.m_Blend == CShaderCreateData::PB_FROM_MATERIAL );
		assert( in_rWhat.m_ZCmp == CShaderCreateData::PZ_FROM_MATERIAL );
		out_rDstPass.m_Textures.clear();
		if( in_rMaterial.m_DiffuseSource == CRenderable_AbstractMaterial::DS_TEXTURE ) {
			assert( in_rMaterial.m_Diffuse.Present() );
			CRenderable_AbstractTextureMap* pMap = in_rMaterial.m_Diffuse.m_pTextureMap.get();
			assert( pMap );
			out_rDstPass.m_Textures.resize(1);
			out_rDstPass.m_Textures[0] = m_pObjectTexturesContainer->UseTexture( pMap );
		}
		FillStates( out_rDstPass.m_PassStates, in_rMaterial.m_bTwoSided, false, in_rMaterial.m_bLightingEnabled, in_rMaterial.m_ARGB );
		RENDER_TEXTURE_STAGE_ARGUMENT TexArg1 = (in_rMaterial.m_DiffuseSource==CRenderable_AbstractMaterial::DS_TEXTURE) ?
			RTSA_TEXTURE : ( in_rMaterial.m_DiffuseSource==CRenderable_AbstractMaterial::DS_VERTEX_COLOR ? RTSA_DIFFUSE : RTSA_TFACTOR);
		RENDER_TEXTURE_STAGE_ARGUMENT AlphaArg1 = in_rMaterial.m_DiffuseSource==CRenderable_AbstractMaterial::DS_TEXTURE ? RTSA_TEXTURE : RTSA_TFACTOR;
		CStageDescription Stages[] = { {TexArg1,RTSA_CURRENT,RTSO_MODULATE,AlphaArg1,RTSA_CURRENT,RTSO_SELECTARG1,0} };
		FillStageStates( out_rDstPass.m_Stages, Stages, 1 );
	}
}

void
CRenderDX8PixelShaderManager::FillStates( std::vector< CSetRenderState >& out_rStates, bool in_bTwoSided, bool AlphaEnabled, bool in_bLightingEnabled, unsigned long in_TFactor ) {
	size_t StatesSize =
		6 // cullmode, alphablendenable, srcblend, dstblend, lighting, tfactor
		+ 1 // alphatest
		+ 0 // (rSrcPass.m_bAlphaTest ? 2 : 0) // if alphatest, then alphafunc and alpharef
		+ 1 // z_write
		+ 1 // z_enable
		+ 1; // (rSrcPass.m_bDepthEnabled ? 1 : 0); // if z_enable, then z_func
	out_rStates.resize( StatesSize );
	size_t n = 0;
	//
	assert( n < StatesSize );
	(out_rStates[n]).m_State = D3DRS_CULLMODE;
	(out_rStates[n]).m_Value = in_bTwoSided ? D3DCULL_NONE : D3DCULL_CW;
	n++;
	//
	assert( n < StatesSize );
	(out_rStates[n]).m_State = D3DRS_ALPHABLENDENABLE;
	(out_rStates[n]).m_Value = AlphaEnabled ? TRUE : FALSE;
	n++;
	//
	assert( n < StatesSize );
	(out_rStates[n]).m_State = D3DRS_SRCBLEND;
	(out_rStates[n]).m_Value = AlphaEnabled ? D3DBLEND_SRCALPHA : D3DBLEND_ONE;
	n++;
	//
	assert( n < StatesSize );
	(out_rStates[n]).m_State = D3DRS_DESTBLEND;
	(out_rStates[n]).m_Value = D3DBLEND_ZERO;
	n++;
	//
	assert( n < StatesSize );
	(out_rStates[n]).m_State = D3DRS_LIGHTING;
	(out_rStates[n]).m_Value = in_bLightingEnabled ? TRUE : FALSE;
	n++;
	//
	assert( n < StatesSize );
	(out_rStates[n]).m_State = D3DRS_TEXTUREFACTOR;
	(out_rStates[n]).m_Value = in_TFactor;
	n++;
	//
	assert( n < StatesSize );
	(out_rStates[n]).m_State = D3DRS_ALPHATESTENABLE;
	(out_rStates[n]).m_Value = FALSE;
	n++;
	//
	//if(rSrcPass.m_bAlphaTest) {
	//	assert( n < StatesSize );
	//	(out_rStates[n]).m_State = D3DRS_ALPHAFUNC;
	//	(out_rStates[n]).m_Value = Convert_RENDER_CMP_MODE_to_D3DCMPFUNC( rSrcPass.m_AlphaCmpMode );
	//	n++;
	//	//
	//	assert( n < StatesSize );
	//	(out_rStates[n]).m_State = D3DRS_ALPHAREF;
	//	(out_rStates[n]).m_Value = rSrcPass.m_AlphaRef;
	//	n++;
	//}
	//
	assert( n < StatesSize );
	(out_rStates[n]).m_State = D3DRS_ZWRITEENABLE;
	(out_rStates[n]).m_Value = TRUE;
	n++;
	//
	assert( n < StatesSize );
	(out_rStates[n]).m_State = D3DRS_ZENABLE;
	(out_rStates[n]).m_Value = D3DZB_TRUE;
	n++;
	//
	if( true ) {
		assert( n < StatesSize );
		(out_rStates[n]).m_State = D3DRS_ZFUNC;
		(out_rStates[n]).m_Value = D3DCMP_LESS;
		n++;
	}
}

void
CRenderDX8PixelShaderManager::FillStageStates(
	std::vector< CPSStageInternal >& out_rStages
	,const CStageDescription* in_pStagesDescriptions
	,long in_nStages )
{
	out_rStages.resize(in_nStages);
	for( long i=0; i<in_nStages; ++i ) {
		CPSStageInternal& rDstStage = out_rStages[i];
		const CStageDescription& pDesc = in_pStagesDescriptions[i];
		//
		std::vector< CSetTextureStageState >& rDstStageStates = rDstStage.m_StageStates;
		rDstStageStates.resize( 12 );
		//
		rDstStageStates[0].m_State = D3DTSS_COLORARG1;
		rDstStageStates[0].m_Value = Convert_RENDER_TEXTURE_STAGE_ARGUMENT( pDesc.m_TexArg1 );
		//
		rDstStageStates[1].m_State = D3DTSS_COLORARG2;
		rDstStageStates[1].m_Value = Convert_RENDER_TEXTURE_STAGE_ARGUMENT( pDesc.m_TexArg2 );
		//
		rDstStageStates[2].m_State = D3DTSS_COLOROP;
		rDstStageStates[2].m_Value = Convert_RENDER_TEXTURE_STAGE_OPERATION( pDesc.m_DiffuseOp );
		//
		rDstStageStates[3].m_State = D3DTSS_ALPHAARG1;
		rDstStageStates[3].m_Value = Convert_RENDER_TEXTURE_STAGE_ARGUMENT( pDesc.m_AlphaArg1 );
		//
		rDstStageStates[4].m_State = D3DTSS_ALPHAARG2;
		rDstStageStates[4].m_Value = Convert_RENDER_TEXTURE_STAGE_ARGUMENT( pDesc.m_AlphaArg2 );
		//
		rDstStageStates[5].m_State = D3DTSS_ALPHAOP;
		rDstStageStates[5].m_Value = Convert_RENDER_TEXTURE_STAGE_OPERATION( pDesc.m_AlphaOp );
		//
		rDstStageStates[6].m_State = D3DTSS_TEXCOORDINDEX;
		rDstStageStates[6].m_Value = pDesc.m_TextureIndex;
		//
		rDstStageStates[7].m_State = D3DTSS_ADDRESSU;
		rDstStageStates[7].m_Value = D3DTADDRESS_WRAP;
		//
		rDstStageStates[8].m_State = D3DTSS_ADDRESSV;
		rDstStageStates[8].m_Value = D3DTADDRESS_WRAP;
		//
		rDstStageStates[9].m_State = D3DTSS_MAGFILTER;
		rDstStageStates[9].m_Value = D3DTEXF_LINEAR;
		//
		rDstStageStates[10].m_State = D3DTSS_MINFILTER;
		rDstStageStates[10].m_Value = D3DTEXF_LINEAR;
		//
		rDstStageStates[11].m_State = D3DTSS_MIPFILTER;
		rDstStageStates[11].m_Value = D3DTEXF_NONE;
	}
}







































