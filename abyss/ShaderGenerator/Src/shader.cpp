#include "shader.h"
#include "shader_resources.h"
#include "linker_interface.h"

#include "feature_position_and_bones.h"
#include "feature_lighting.h"
#include "feature_textures.h"

#include "../../Render_DX8/Include/r8_vs_tokens.h"
#include "../../Render_DX8/Include/r8_vs_lights.h"
#include "../../Render_DX8/Include/r8_vs_textures.h"

#include <stdio.h>
#include <algorithm>
#include <map>

#define MAX_BONES 24
#define CONSTANTS_RESERVED (MAX_BONES*3)

CShader::CShader( INVLink* in_pLinker ) : m_ParamSeqNumber(0) {
	m_Features.push_back( new CFeature_PositionBones() );
	m_Features.push_back( new CFeature_Lighting() );
	m_Features.push_back( new CFeature_Textures() );
	// add all fragments
	for( size_t i=0; i<m_Features.size(); ++i ) {
		m_Features[i]->AddFragments( in_pLinker );
	}
	for( size_t i=0; i<m_Features.size(); ++i ) {
		m_Features[i]->GetMyIDs( in_pLinker );
	}
}

CShader::~CShader() {
	for( size_t i=0; i<m_Features.size(); ++i ) {
		delete m_Features[i];
	}
}

void CShader::Dump( const CInputParams& in_P ) {
	printf("%d:  XYZ",m_ParamSeqNumber++);
	if(in_P.m_NBones>0)
		printf(" %dbones",in_P.m_NBones);
	if(in_P.m_Normal)
		printf(" Norm");
	if(in_P.m_Diffuse)
		printf(" Diff");
	if(in_P.m_LightEnabled)
		printf(" Light");
	printf(" ");

	for(long i=0; i<MAX_LIGHTS; ++i)
		printf("%d",in_P.m_Lights[i]);
	//printf("%d",in_P.m_Light0);
	//printf("%d",in_P.m_Light1);
	//printf("%d",in_P.m_Light2);
	//printf("%d",in_P.m_Light3);

	printf(" ");
	printf("%d",in_P.m_Tex0);
	printf("%d",in_P.m_Tex1);
	//printf("%d",in_P.m_Tex2);
	//printf("%d",in_P.m_Tex3);
	printf("\n");
}

void
CShader::Reset() {
	m_ParamSeqNumber = 0;
	m_Processed.clear();
	m_Params = CInputParams();
	m_GenerateParams = m_Params;
}

bool
CShader::NextCombination() {

	CInputParams Params;
	if( GetNextParamSet(m_Params,m_Processed,Params) ) {
		m_Params = Params;
		NormalizeParamsSet(m_Params,m_GenerateParams);
		return true;
	}

	return false;
}

bool
CShader::Generate( INVLink* in_pLinker ) {
	Dump(m_GenerateParams);

	CShaderResources Resources;

	for( size_t i=0; i<m_Features.size(); ++i ) {
		if( !m_Features[i]->PreGenerate( Resources,m_GenerateParams ) )
			return false;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	// Generate !
	/////////////////////////////////////////////////////////////////////////////////////

	assert(m_Features.size());
	std::vector< NVLINK_FRAGMENT_ID > Fragments;
	for( size_t i=0; i<m_Features.size(); ++i ) {
		std::list<NVLINK_FRAGMENT_ID> Frags = m_Features[i]->GetFragmentsForThisCombination( Resources,m_GenerateParams );
		for( std::list<NVLINK_FRAGMENT_ID>::const_iterator It = Frags.begin(); It != Frags.end(); ++It ) {
			Fragments.push_back( *It );
		}
	}
	Fragments.push_back(0);

	HRESULT Res;
	INVLinkBuffer* pReturnedShader = NULL;
	Res = in_pLinker->ReserveConstantRange(0,CONSTANTS_RESERVED-1);
	assert( Res == S_OK );
	Res = in_pLinker->CreateBinaryShader( &Fragments[0], &pReturnedShader, 0 );
	if( Res != S_OK ) {
		if(Res==NVLINK_TOO_MANY_INSTRUCTIONS)
			printf("  TOO MANY INSTRUCTIONS !\n");
		else if(Res==NVLINK_TOO_MANY_CONSTANTS)
			printf("  TOO MANY CONSTANTS !\n");
		else if(Res==NVLINK_TOO_MANY_REGISTERS)
			printf("  TOO MANY REGISTERS !\n");
		else if(Res==NVLINK_TOO_MANY_ADDRESS_REGISTERS)
			printf("  TOO MANY ADDRESS REGISTERS !\n");
		else if(Res==NVLINK_TOO_MANY_VERTICES)
			printf("  TOO MANY ADDRESS VERTICES !\n");
		else
			printf("  GENERAL FUCKUP !\n");
		assert(false);
	}
	NVLinkStatistics Statistics;
	Statistics.dwSize = sizeof(NVLinkStatistics);
	Res = in_pLinker->GetStatistics( &Statistics );
	assert( Res == S_OK );
	printf("  dwInstructionsUsed = %lu\n", Statistics.dwInstructionsUsed );
	printf("  dwInstructionsRemoved = %lu\n", Statistics.dwInstructionsRemoved );
	printf("  dwConstantsUsed = %lu\n", Statistics.dwConstantsUsed );
	printf("  dwAddressRegistersUsed = %lu\n", Statistics.dwAddressRegistersUsed );
	printf("  dwRegistersUsed = %lu\n", Statistics.dwRegistersUsed );
	printf("  dwVerticesUsed = %lu\n", Statistics.dwVerticesUsed );

	size_t Size = pReturnedShader->GetBufferSize();
	printf("  shader code size = %lu\n", Size );
	unsigned long* Ptr = (unsigned long*)(pReturnedShader->GetPointer());
	m_Binary.assign( Ptr, Ptr+(Size/sizeof(unsigned long)) );

	//INVLinkBuffer* pShaderSource = NULL;
	//Res = in_pLinker->GetShaderSource(&pShaderSource);
	//if( Res == S_OK ) {
	//	const char* pSourceText = (const char*)pShaderSource->GetPointer();
	//	printf("  shader text:\n%s\n", pSourceText );
	//	pShaderSource->Release();
	//} else {
	//	printf("  shader text cannot be retrieved\n" );
	//}

	m_Identifier = ConstructMeaningfulIdentifier(m_GenerateParams);
	m_FVF = ConstructFVF(m_GenerateParams);
	m_Lights = ConstructLights(m_GenerateParams);
	m_Textures = ConstructTextures(m_GenerateParams);
	m_Declaration = ConstructDeclaration(m_GenerateParams);

	std::map<unsigned long,unsigned long> tmpBindings;
	for( size_t i=0; i<m_Features.size(); ++i ) {
		m_Features[i]->GetConstants( in_pLinker );
		//
		std::vector< std::pair<unsigned long,unsigned long> > Bindings = m_Features[i]->GetBindings();
		for(size_t i=0; i<Bindings.size(); ++i) {
			unsigned long TOKEN = Bindings[i].first;
			unsigned long VALUE = Bindings[i].second;
			std::pair< std::map<unsigned long,unsigned long>::iterator, bool > result = tmpBindings.insert( Bindings[i] );
			std::map<unsigned long,unsigned long>::iterator Where = result.first;
			assert( Where->first == TOKEN );
			if( result.second == false ) {
				unsigned long PREV_VALUE = Where->second;
				assert( PREV_VALUE == VALUE );
			}
		}
	}
	m_ConstantBindings.clear();
	m_ConstantBindings.reserve( tmpBindings.size() + 1 );
	for( std::map<unsigned long,unsigned long>::const_iterator It = tmpBindings.begin(); It != tmpBindings.end(); ++It ) {
		m_ConstantBindings.push_back( *It );
	}
	m_ConstantBindings.push_back( std::pair<unsigned long,unsigned long>(VS_TOKEN_END_OF_SEQUENCE,0) );

	//pReturnedShader->Release();

	return true;
}

std::string
CShader::ConstructMeaningfulIdentifier( const CInputParams& in_rParams ) {
	// base
	std::string Result("XYZ");
	// bones
	if(in_rParams.m_NBones > 0) {
		char Buffer[16];
		sprintf(Buffer,"_%dbones",in_rParams.m_NBones);
		Result += Buffer;
	}
	// normal
	if(in_rParams.m_Normal) {
		Result += "_Normal";
	}
	// diffuse
	if(in_rParams.m_Diffuse) {
		Result += "_Diffuse";
	}
	// lights
	if(in_rParams.m_LightEnabled) {
		char Buffer[16];
		sprintf(Buffer,"_");
		for(long i=0; i<MAX_LIGHTS; ++i)
			sprintf(Buffer+1+i,"%d",in_rParams.m_Lights[i]);
		//sprintf(Buffer,"_%d%d%d%d",in_rParams.m_Light0,in_rParams.m_Light1,in_rParams.m_Light2,in_rParams.m_Light3);
		Result += Buffer;
	} else {
		Result += "_LightDisabled";
	}
	// textures
	{
		char Buffer[16];
		sprintf(Buffer,"_%d%d",in_rParams.m_Tex0,in_rParams.m_Tex1);
		Result += Buffer;
	}
	//
	return Result;
}

std::vector<DWORD>
CShader::ConstructDeclaration( const CInputParams& in_rParams ) {
	std::vector<DWORD> Result;
	Result.push_back( D3DVSD_STREAM(0) );
	Result.push_back( D3DVSD_REG(D3DVSDE_POSITION,D3DVSDT_FLOAT3) );
	if(in_rParams.m_NBones>0) {
		switch(in_rParams.m_NBones) {
			case 1:
				break;
			case 2:
				Result.push_back( D3DVSD_REG(D3DVSDE_BLENDWEIGHT,D3DVSDT_FLOAT1) );
				break;
			case 3:
				Result.push_back( D3DVSD_REG(D3DVSDE_BLENDWEIGHT,D3DVSDT_FLOAT2) );
				break;
			case 4:
				Result.push_back( D3DVSD_REG(D3DVSDE_BLENDWEIGHT,D3DVSDT_FLOAT3) );
				break;
		}
		Result.push_back( D3DVSD_REG(D3DVSDE_BLENDINDICES,D3DVSDT_UBYTE4) );
	}
	if(in_rParams.m_Normal) {
		Result.push_back( D3DVSD_REG(D3DVSDE_NORMAL,D3DVSDT_FLOAT3) );
	}
	if(in_rParams.m_Diffuse) {
		Result.push_back( D3DVSD_REG(D3DVSDE_DIFFUSE,D3DVSDT_D3DCOLOR) );
	}
	if(in_rParams.m_Tex0 == CInputParams::TT_PASSTHRU) {
		Result.push_back( D3DVSD_REG(D3DVSDE_TEXCOORD0,D3DVSDT_FLOAT2) );
		if(in_rParams.m_Tex1 == CInputParams::TT_PASSTHRU) {
			Result.push_back( D3DVSD_REG(D3DVSDE_TEXCOORD1,D3DVSDT_FLOAT2) );
		}
	}
	Result.push_back( D3DVSD_END() );
	return Result;
}

unsigned long
CShader::ConstructFVF( const CInputParams& in_rParams ) {
	unsigned long Result = 0;
	// position and bones
	switch( in_rParams.m_NBones ) {
		case 0:
			Result = D3DFVF_XYZ;
			break;
		case 1:
			Result = (D3DFVF_XYZB1|D3DFVF_LASTBETA_UBYTE4);
			break;
		case 2:
			Result = (D3DFVF_XYZB2|D3DFVF_LASTBETA_UBYTE4);
			break;
		case 3:
			Result = (D3DFVF_XYZB3|D3DFVF_LASTBETA_UBYTE4);
			break;
		case 4:
			Result = (D3DFVF_XYZB4|D3DFVF_LASTBETA_UBYTE4);
			break;
	}
	// normal
	if(in_rParams.m_Normal)
		Result |= D3DFVF_NORMAL;
	// diffuse
	if(in_rParams.m_Diffuse)
		Result |= D3DFVF_DIFFUSE;
	// textures
	long nTexCoordSets = 0;
	if(in_rParams.m_Tex0 == CInputParams::TT_PASSTHRU)
		++nTexCoordSets;
	if(in_rParams.m_Tex1 == CInputParams::TT_PASSTHRU)
		++nTexCoordSets;
	switch(nTexCoordSets) {
		case 1:
			Result |= D3DFVF_TEX1;
			break;
		case 2:
			Result |= D3DFVF_TEX2;
			break;
		case 0:
		default:
			break;
	}
	return Result;
}

unsigned long
CShader::ConstructLights( const CInputParams& in_rParams ) {
#if MAX_LIGHTS==4
	return VS_CODE_FOR_LIGHTS(in_rParams.m_LightEnabled,in_rParams.m_Lights[0],in_rParams.m_Lights[1],in_rParams.m_Lights[2],in_rParams.m_Lights[3]);
#elif MAX_LIGHTS==3
	return VS_CODE_FOR_LIGHTS(in_rParams.m_LightEnabled,in_rParams.m_Lights[0],in_rParams.m_Lights[1],in_rParams.m_Lights[2]);
#elif MAX_LIGHTS==2
	return VS_CODE_FOR_LIGHTS(in_rParams.m_LightEnabled,in_rParams.m_Lights[0],in_rParams.m_Lights[1]);
#elif MAX_LIGHTS==1
	return VS_CODE_FOR_LIGHTS(in_rParams.m_LightEnabled,in_rParams.m_Lights[0]);
#elif
	#error MAX_LIGHTS must be between 1 and 4
#endif
	//return VS_CODE_FOR_LIGHTS(in_rParams.m_LightEnabled,in_rParams.m_Light0,in_rParams.m_Light1,in_rParams.m_Light2,in_rParams.m_Light3);
}

unsigned long
CShader::ConstructTextures( const CInputParams& in_rParams ) {
	return VS_CODE_FOR_TEXTURES(in_rParams.m_Tex0,in_rParams.m_Tex1);
}
































