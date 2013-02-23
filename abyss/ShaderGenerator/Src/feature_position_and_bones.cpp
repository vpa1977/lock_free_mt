#include "feature_position_and_bones.h"
#include "../../Render_DX8/Include/r8_vs_tokens.h"
#include <stdio.h>

CFeature_PositionBones::CFeature_PositionBones() {
}

CFeature_PositionBones::~CFeature_PositionBones() {
}

void
CFeature_PositionBones::AddFragments( INVLink* in_pLinker ) {
	//printf("CFeature_PositionBones::AddFragments()\n");
	AddFragmentsInternal(in_pLinker,"pos_bones.nvo");
	AddFragmentsInternal(in_pLinker,"pos_bones_n.nvo");
	AddFragmentsInternal(in_pLinker,"pos_position.nvo");
}

void
CFeature_PositionBones::GetMyIDs( INVLink* in_pLinker ) {
	//printf("CFeature_PositionBones::GetMyIDs()\n");
	HRESULT Res;

	Res = in_pLinker->GetFragmentID("f_bones_0", &(m_Fragment_Bones[0]) );
	assert( Res == S_OK );
	Res = in_pLinker->GetFragmentID("f_bones_1", &(m_Fragment_Bones[1]) );
	assert( Res == S_OK );
	Res = in_pLinker->GetFragmentID("f_bones_2", &(m_Fragment_Bones[2]) );
	assert( Res == S_OK );
	Res = in_pLinker->GetFragmentID("f_bones_3", &(m_Fragment_Bones[3]) );
	assert( Res == S_OK );
	Res = in_pLinker->GetFragmentID("f_bones_4", &(m_Fragment_Bones[4]) );
	assert( Res == S_OK );

	Res = in_pLinker->GetFragmentID("f_bones_n_0", &(m_Fragment_Bones_Normal[0]) );
	assert( Res == S_OK );
	Res = in_pLinker->GetFragmentID("f_bones_n_1", &(m_Fragment_Bones_Normal[1]) );
	assert( Res == S_OK );
	Res = in_pLinker->GetFragmentID("f_bones_n_2", &(m_Fragment_Bones_Normal[2]) );
	assert( Res == S_OK );
	Res = in_pLinker->GetFragmentID("f_bones_n_3", &(m_Fragment_Bones_Normal[3]) );
	assert( Res == S_OK );
	Res = in_pLinker->GetFragmentID("f_bones_n_4", &(m_Fragment_Bones_Normal[4]) );
	assert( Res == S_OK );

	Res = in_pLinker->GetFragmentID("f_position_worldviewproj", &m_Fragment_Position_WorldViewProj );
	assert( Res == S_OK );

	Res = in_pLinker->GetConstantID("c_one_zero", &m_Constant_10xx);
	assert( Res == S_OK );
	Res = in_pLinker->GetConstantID("c_worldviewproj", &m_Constant_WorldViewProj);
	assert( Res == S_OK );
}

void
CFeature_PositionBones::Reset() {
}

bool
CFeature_PositionBones::PreGenerate( CShaderResources& rRes, const CInputParams& rParams ) {
	return true;
}

std::list<NVLINK_FRAGMENT_ID>
CFeature_PositionBones::GetFragmentsForThisCombination( const CShaderResources& rRes, const CInputParams& rParams ) {
	m_Res = rRes;
	m_Params = rParams;

	std::list<NVLINK_FRAGMENT_ID> Frags;

	// bones
	if(rRes.m_Required_NObject) {
		assert(rParams.m_Normal);
		assert(rParams.m_LightEnabled);
		Frags.push_back(m_Fragment_Bones_Normal[rParams.m_NBones]);
	} else {
		Frags.push_back(m_Fragment_Bones[rParams.m_NBones]);
	}

	// post-bones transformation
	if( !rRes.m_Required_VWorld && !rRes.m_Required_VCamSpace ) {
		Frags.push_back(m_Fragment_Position_WorldViewProj);
	} else if( !rRes.m_Required_VWorld && rRes.m_Required_VCamSpace ) {
		// TODO
		assert(false);
	} else if( rRes.m_Required_VWorld && !rRes.m_Required_VCamSpace ) {
		// TODO
		assert(false);
	} else if( rRes.m_Required_VWorld && rRes.m_Required_VCamSpace ) {
		// TODO
		assert(false);
	} else {
		assert(false);
	}
	return Frags;
}

void
CFeature_PositionBones::GetConstants( INVLink* in_pLinker ) {
	//printf("  CFeature_PositionBones::GetConstants()\n" );
	HRESULT Res;

	m_Slot_10xx = 0;
	Res = in_pLinker->GetConstantSlot(m_Constant_10xx, 0, &m_Slot_10xx);
	assert( S_OK == Res );

	m_Slot_WorldViewProj = 0;
	Res = in_pLinker->GetConstantSlot(m_Constant_WorldViewProj, 0, &m_Slot_WorldViewProj);
	assert( S_OK == Res );
}

std::vector< std::pair<unsigned long,unsigned long> >
CFeature_PositionBones::GetBindings() const {
	std::vector< std::pair<unsigned long,unsigned long> > Result;

	if( m_Params.m_NBones > 0 ) {
		Result.push_back( std::pair<unsigned long,unsigned long>(VS_TOKEN_BONE_MATRICES,0) );
	}

	if( m_Params.m_NBones > 0 /*|| TODO */ ) {
		assert(m_Slot_10xx != NVLINK_NOT_USED);
		Result.push_back( std::pair<unsigned long,unsigned long>(VS_TOKEN_DEFAULT_CONSTANTS,m_Slot_10xx) );
	}

	if( !m_Res.m_Required_VWorld && !m_Res.m_Required_VCamSpace ) {
		Result.push_back( std::pair<unsigned long,unsigned long>(VS_TOKEN_WORLDVIEWPROJ_MATRIX,m_Slot_WorldViewProj) );
	} else if( !m_Res.m_Required_VWorld && m_Res.m_Required_VCamSpace ) {
		// TODO
		assert(false);
	} else if( m_Res.m_Required_VWorld && !m_Res.m_Required_VCamSpace ) {
		// TODO
		assert(false);
	} else if( m_Res.m_Required_VWorld && m_Res.m_Required_VCamSpace ) {
		// TODO
		assert(false);
	} else {
		assert(false);
	}
	
	return Result;
}

































