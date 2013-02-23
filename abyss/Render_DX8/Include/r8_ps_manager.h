#ifndef R8_PIXEL_SHADER_MANAGER_HPP_INCLUDED
#define R8_PIXEL_SHADER_MANAGER_HPP_INCLUDED

#include "../../Objects/Include/Render/material.h"

#include "handle_storage.h"
#include "render_pixel_shader.h"
#include "r8_texture_manager.h"
#include "r8_object_textures.h"

#include <d3d8.h>

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <map>

struct CSetRenderState
{
	D3DRENDERSTATETYPE m_State;
	DWORD m_Value;
};

struct CSetTextureStageState
{
	D3DTEXTURESTAGESTATETYPE m_State;
	DWORD m_Value;
};

// --------------------------------------------------------------------------------------------------------------

class CRenderDX8PixelShaderManager
{
public:
	CRenderDX8PixelShaderManager();
	~CRenderDX8PixelShaderManager();

	void ReleaseEverything();

	void SetInterface(	IDirect3D8* in_pD3D8,
						IDirect3DDevice8* in_pD3D8Device,
						CRenderDX8TextureManager* in_pTextureManager,
						CRenderDX8_ObjectsTexturesContainer* in_pObjectTexturesContainer );

	unsigned long CreatePixelShader( const CPixelShader& in_rShaderDescription );
	unsigned long CreatePixelShader( const CRenderable_AbstractMaterial& , std::vector< unsigned long >& );
	void ReleasePixelShader( unsigned long in_PixelShaderHandle );

	unsigned long UsePixelShader( unsigned long in_PixelShaderHandle );
	void ApplyPixelShaderPass( unsigned long in_PassNum );
private:
	IDirect3D8*								m_pD3D8;
	IDirect3DDevice8*						m_pD3D8Device;
	D3DCAPS8								m_DeviceCaps;
	CRenderDX8TextureManager*				m_pTextureManager;
	CRenderDX8_ObjectsTexturesContainer*	m_pObjectTexturesContainer;

	CHandleStorage m_PSHandlesStorage;

	struct CPSStageInternal
	{
		std::vector< CSetTextureStageState > m_StageStates;
	};
	struct CPSPassInternal
	{
		std::vector< CSetRenderState > m_PassStates;
		std::vector< CPSStageInternal > m_Stages;
		std::vector< unsigned long > m_Textures;
	};
	struct CPSInternal
	{
		std::vector< CPSPassInternal > m_Passes;
	};
	struct CPixelShaderInfo
	{
		CPixelShaderInfo(CPSInternal* in_pPS=NULL) : m_pPS(in_pPS) {}
		boost::shared_ptr< CPSInternal > m_pPS;
	};
	typedef std::map< unsigned long, CPixelShaderInfo > CPixelShaderInfoMap;
	typedef CPixelShaderInfoMap::value_type CPixelShaderInfoMapValueType;
	typedef CPixelShaderInfoMap::iterator CPixelShaderInfoMapIterator;
	typedef CPixelShaderInfoMap::const_iterator CPixelShaderInfoMapConstIterator;
	typedef std::pair<CPixelShaderInfoMapIterator,bool> CPixelShaderInfoMapInsertResult;
	CPixelShaderInfoMap m_PShaderMap;

	CPixelShaderInfoMapConstIterator m_ActiveShaderIt;

	D3DCULL		Convert_RENDER_CULL_MODE_to_D3DCULL(RENDER_CULL_MODE);
	D3DBLEND	Convert_RENDER_BLEND_MODE_to_D3DBLEND(RENDER_BLEND_MODE);
	D3DCMPFUNC	Convert_RENDER_CMP_MODE_to_D3DCMPFUNC(RENDER_CMP_MODE);
	DWORD		Convert_RENDER_TEXTURE_STAGE_ARGUMENT(RENDER_TEXTURE_STAGE_ARGUMENT);
	DWORD		Convert_RENDER_TEXTURE_STAGE_OPERATION(RENDER_TEXTURE_STAGE_OPERATION);
	DWORD		Convert_RENDER_TEXTURE_ADDRESS_MODE(RENDER_TEXTURE_ADDRESS_MODE);
	DWORD		Convert_RENDER_TEXTURE_FILTER_MODE(RENDER_TEXTURE_FILTER_MODE);

	struct CShaderCreateData
	{
		enum PASS_CONTENT {
			PC_DIFFUSE,
			PC_REFLECTION,
			PC_REFLECTION_MULTIPLIED_BY_GLOSS,
			PC_SPECULAR,
			PC_SPECULAR_MULTIPLIED_BY_GLOSS
		};
		enum PASS_BLEND {
			PB_FROM_MATERIAL,
			PB_FILTER,
			PB_ADD
		};
		enum PASS_ZCMP {
			PZ_FROM_MATERIAL,
			PZ_EQUAL
		};
		struct CPassData {
			PASS_CONTENT	m_Content;
			PASS_BLEND		m_Blend;
			PASS_ZCMP		m_ZCmp;
		};
		std::vector< CPassData > m_Passes;
	};

	bool PlanShader( const CRenderable_AbstractMaterial& in_rMaterial, CShaderCreateData& out_rData );
	
	void FillPass( const CRenderable_AbstractMaterial& in_rMaterial, const CShaderCreateData::CPassData& in_rWhat, CPSPassInternal& out_rDstPass );
	
	// TODO:
	// 1. instead of "bool AlphaEnabled" make some fancy enum
	// 2. implement AlphaTest
	void FillStates( std::vector< CSetRenderState >& out_rStates
					,bool in_bTwoSided
					,bool AlphaEnabled
					,bool in_bLightingEnabled
					,unsigned long in_TFactor );
	
	// TODO:
	// 1. support various texture wrapping modes
	struct CStageDescription {
		RENDER_TEXTURE_STAGE_ARGUMENT  m_TexArg1;
		RENDER_TEXTURE_STAGE_ARGUMENT  m_TexArg2;
		RENDER_TEXTURE_STAGE_OPERATION m_DiffuseOp;
		RENDER_TEXTURE_STAGE_ARGUMENT  m_AlphaArg1;
		RENDER_TEXTURE_STAGE_ARGUMENT  m_AlphaArg2;
		RENDER_TEXTURE_STAGE_OPERATION m_AlphaOp;
		long m_TextureIndex;
	};
	void FillStageStates(	std::vector< CPSStageInternal >& out_rStages
							,const CStageDescription* in_pStagesDescriptions
							,long in_nStages );
};


#endif





























