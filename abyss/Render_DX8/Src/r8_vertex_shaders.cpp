#include "../Include/r8.h"
#include "../Include/r8_vs_tokens.h"
#include "../../MT_Core/Include/rtassert.h"

#include "shader_definitions.inl"

void
CRenderDX8::SetupShaderDefinitions() {
	for( size_t i=0; i<N_GENERATED_SHADERS; ++i ) {
		const CShaderDescriptionGenerated& rDesc = g_GeneratedShaders[i];
		m_ShaderDataArray.insert(
			CShaderDataMap::value_type(
				CShaderDataKey(rDesc.m_FVF,rDesc.m_Lights,rDesc.m_Textures)
				,CShaderDataValue(rDesc.m_Declaration, rDesc.m_Binary, rDesc.m_ConstantBindings )
			)
		);
	}
}

unsigned long
CRenderDX8::GetTexturesCode(unsigned long in_FVF) {
	// TODO:
	// Pass texgen params from pixel shader to this function, and use it for texcode calculation.
	unsigned long NTC = (in_FVF>>D3DFVF_TEXCOUNT_SHIFT) & 0x00000003;
	if(NTC==0)
		return 0x00;
	else if(NTC==1)
		return 0x01;
	else if(NTC==2)
		return 0x11;
	else {
		assert(false);
		return 666;
	}
}

void
CRenderDX8::ApplyVertexShader(
	unsigned long in_FVF
	,const CMatrix& in_M
	,const CSkeleton_DX8& in_Skel
	,const std::vector<unsigned char>& in_UsedBones
)
{
	assert( m_pD3D8Device );

	HRESULT Res;

	// find shader record
	unsigned long LC = GetLightsCode();
	unsigned long TC = GetTexturesCode(in_FVF);
	std::map< CShaderDataKey, CShaderDataValue >::iterator It = m_ShaderDataArray.find(
		CShaderDataKey( in_FVF, LC, TC )
	);
	//if(false) {
	if( It != m_ShaderDataArray.end() ) {
		// setup shader
		CShaderDataValue& rShaderData = It->second;
		if( !rShaderData.m_bCreated ) {
			// create shader
			Res = m_pD3D8Device->CreateVertexShader(	rShaderData.m_pDeclaration
														,rShaderData.m_pCode
														,&rShaderData.m_Handle
														,m_VertexProcessType==RVP_HARDWARE ? 0 : D3DUSAGE_SOFTWAREPROCESSING );
			assert( Res==D3D_OK );
			rShaderData.m_bCreated = true;
		}
		// set shader
		Res = m_pD3D8Device->SetVertexShader( rShaderData.m_Handle );
		assert( Res==D3D_OK );
		// set constants
		ApplyVertexShaderConstants( rShaderData.m_Constants, in_M, in_Skel, in_UsedBones );
	} else {
		// we dont use FFP vertex functionality anymore
		RTASSERT(false);

		//Res = m_pD3D8Device->SetTransform( D3DTS_WORLD, (const D3DMATRIX*)&in_M );
		//assert(Res==D3D_OK);
		//Res = m_pD3D8Device->SetVertexShader( in_FVF );
		//assert(Res==D3D_OK);
	}
}

void
CRenderDX8::ApplyVertexShaderConstants(
	const DWORD* in_pConstants
	,const CMatrix& in_M
	,const CSkeleton_DX8& in_Skel
	,const std::vector<unsigned char>& in_UsedBones
)
{
	assert( m_pD3D8Device );
	assert( in_pConstants );

	HRESULT Res;
	for( ; *in_pConstants; ) {
		// TODO: SPEEDUP THIS CODE !!!

		if( VS_TOKEN_WORLD_MATRIX == *in_pConstants ) {
			++in_pConstants; // skip token
			CMatrix MW = in_M;
			MW.Transpose();
			Res = m_pD3D8Device->SetVertexShaderConstant( *in_pConstants, &MW, 4 );
			assert(Res==D3D_OK);
			++in_pConstants; // skip constant register number

		} else if( VS_TOKEN_VIEWPROJ_MATRIX == *in_pConstants ) {
			++in_pConstants; // skip token
			CMatrix MVP = m_ViewMatrix*m_ProjMatrix;
			MVP.Transpose();
			Res = m_pD3D8Device->SetVertexShaderConstant( *in_pConstants, &MVP, 4 );
			assert(Res==D3D_OK);
			++in_pConstants; // skip constant register number

		} else if( VS_TOKEN_WORLDVIEWPROJ_MATRIX == *in_pConstants ) {
			++in_pConstants; // skip token
			CMatrix MWVP = in_M*m_ViewMatrix*m_ProjMatrix;
			MWVP.Transpose();
			Res = m_pD3D8Device->SetVertexShaderConstant( *in_pConstants, &MWVP, 4 );
			assert(Res==D3D_OK);
			++in_pConstants; // skip constant register number
		} else if( VS_TOKEN_BONE_MATRICES == *in_pConstants ) {
			++in_pConstants; // skip token
			assert( 0 == *in_pConstants ); // bone matrices begin at const register 0
			//CLog::Print("-bones-------------------------------------------------------\n");
			size_t NB = in_UsedBones.size();
			assert( NB <= 24 );
			// skeleton bones already have actual m_FullCurrentPosition
			for( size_t i=0; i<NB; ++i) {
				int index = in_UsedBones[i];
				const CBone_DX8* pBone = in_Skel.m_BoneIDs[ index ];
								
				CMatrix BoneM( pBone->m_InverseBoneSkin * pBone->m_FullCurrentPosition);
				
				BoneM.Transpose(); 
				HRESULT Res = m_pD3D8Device->SetVertexShaderConstant( i*3, &BoneM, 3 );
				assert(Res==D3D_OK);
			}
			++in_pConstants; // skip constant register number
		} else if( VS_TOKEN_DEFAULT_CONSTANTS == *in_pConstants ) {
			++in_pConstants; // skip token
			CVector V(1.0f,0.0f,0.0f);
			Res = m_pD3D8Device->SetVertexShaderConstant( *in_pConstants, &V, 1 );
			assert(Res==D3D_OK);
			++in_pConstants; // skip constant register number
		} else {
			assert( false );
		}
	}
}

/*
void
CRenderDX8::SetupBoneRecurse( const CBone_DX8* pBone, const CMatrix& rParentM ) {
	if(pBone) {

		CMatrix ThisM( pBone->m_CurrentPosition.Get() );
		//CLog::Print("ThisM:\n");
		//CLog::Print("%f %f %f %f\n",ThisM.m[0*4+0],ThisM.m[0*4+1],ThisM.m[0*4+2],ThisM.m[0*4+3]);
		//CLog::Print("%f %f %f %f\n",ThisM.m[1*4+0],ThisM.m[1*4+1],ThisM.m[1*4+2],ThisM.m[1*4+3]);
		//CLog::Print("%f %f %f %f\n",ThisM.m[2*4+0],ThisM.m[2*4+1],ThisM.m[2*4+2],ThisM.m[2*4+3]);
		//CLog::Print("%f %f %f %f\n",ThisM.m[3*4+0],ThisM.m[3*4+1],ThisM.m[3*4+2],ThisM.m[3*4+3]);

		CMatrix FullM( ThisM*rParentM );
		//CLog::Print("FullM:\n");
		//CLog::Print("%f %f %f %f\n",FullM.m[0*4+0],FullM.m[0*4+1],FullM.m[0*4+2],FullM.m[0*4+3]);
		//CLog::Print("%f %f %f %f\n",FullM.m[1*4+0],FullM.m[1*4+1],FullM.m[1*4+2],FullM.m[1*4+3]);
		//CLog::Print("%f %f %f %f\n",FullM.m[2*4+0],FullM.m[2*4+1],FullM.m[2*4+2],FullM.m[2*4+3]);
		//CLog::Print("%f %f %f %f\n",FullM.m[3*4+0],FullM.m[3*4+1],FullM.m[3*4+2],FullM.m[3*4+3]);

		CMatrix BoneM( pBone->m_InitialPositionInverse*FullM );
		//CLog::Print("BoneM:\n");
		//CLog::Print("%f %f %f %f\n",BoneM.m[0*4+0],BoneM.m[0*4+1],BoneM.m[0*4+2],BoneM.m[0*4+3]);
		//CLog::Print("%f %f %f %f\n",BoneM.m[1*4+0],BoneM.m[1*4+1],BoneM.m[1*4+2],BoneM.m[1*4+3]);
		//CLog::Print("%f %f %f %f\n",BoneM.m[2*4+0],BoneM.m[2*4+1],BoneM.m[2*4+2],BoneM.m[2*4+3]);
		//CLog::Print("%f %f %f %f\n",BoneM.m[3*4+0],BoneM.m[3*4+1],BoneM.m[3*4+2],BoneM.m[3*4+3]);
		BoneM.Transpose();

		HRESULT Res = m_pD3D8Device->SetVertexShaderConstant( pBone->m_BoneID*3, &BoneM, 3 );
		//CMatrix Unit;
		//HRESULT Res = m_pD3D8Device->SetVertexShaderConstant( pBone->m_BoneID*3, &Unit, 3 );
		assert(Res==D3D_OK);
		for( std::vector< CBone_DX8* >::const_iterator It=pBone->m_Children.begin(); It!=pBone->m_Children.end(); ++It ) {
			SetupBoneRecurse(*It,FullM);
		}
	}
}
*/




























