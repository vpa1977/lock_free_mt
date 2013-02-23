#include "feature_lighting.h"
#include "../../Render_DX8/Include/r8_vs_tokens.h"


// ===================================================================================================================
// Lighting disabled
// ===================================================================================================================

std::string
CActiveFragment_LightDisabled::GetName() {
	return "lighting_disabled";
}

void
CActiveFragment_LightDisabled::GetIDs(INVLink* in_pLinker) {
	HRESULT Res;
	Res = in_pLinker->GetFragmentID("f_lighting_disabled", &m_FragmentID_Disabled );
	assert( Res == S_OK );
	Res = in_pLinker->GetConstantID("c_one_zero", &m_ConstantID_WhiteColor);
	assert( Res == S_OK );
}

void
CActiveFragment_LightDisabled::GetConstants(INVLink* in_pLinker) {
	m_Slot_WhiteColor = 0;
	HRESULT Res;
	Res = in_pLinker->GetConstantSlot(m_ConstantID_WhiteColor, 0, &m_Slot_WhiteColor);
	assert( S_OK == Res );
	assert( NVLINK_NOT_USED != m_Slot_WhiteColor );
	//printf("    whitecolor in slot %lu\n", m_Slot_WhiteColor );
}

std::map< unsigned long,unsigned long >
CActiveFragment_LightDisabled::GetBindings() const {
	std::map< unsigned long,unsigned long > Result;
	Result.insert( std::map< unsigned long,unsigned long >::value_type(VS_TOKEN_DEFAULT_CONSTANTS,m_Slot_WhiteColor) );
	return Result;
}

// ===================================================================================================================
// Light
// ===================================================================================================================

const char* g_LightTypeSuffix[] = { "_none", "_dir", "_point", "_spot" };

CActiveFragment_Light::CActiveFragment_Light(long in_N, CInputParams::LIGHT_TYPE in_Type) :
	m_FragmentID(0)
	,m_ConstantID_LightOrg(0)
	,m_ConstantID_LightDir(0)
	,m_ConstantID_LightColor(0)
	,m_ConstantID_LightParams(0)
	,m_ConstantID_WhiteColor(0)
	,m_N(in_N)
	,m_Type(in_Type)
{
	char Buffer[128];
	//
	sprintf(Buffer,"f_lighting_light%d%s",m_N,g_LightTypeSuffix[(int)m_Type]);
	m_FragmentName = Buffer;
	//
	sprintf(Buffer,"c_light_%d_org",m_N);
	m_LightOrgName = Buffer;
	//
	sprintf(Buffer,"c_light_%d_dir",m_N);
	m_LightDirName = Buffer;
	//
	sprintf(Buffer,"c_light_%d_color",m_N);
	m_LightColorName = Buffer;
	//
	sprintf(Buffer,"c_light_%d_params",m_N);
	m_LightParamsName = Buffer;
}

std::string
CActiveFragment_Light::GetName() {
	char Buffer[16];
	sprintf(Buffer,"light%d_",m_N);
	switch(m_Type) {
		case CInputParams::LT_NONE:
			return std::string(Buffer) + "none";
		case CInputParams::LT_DIRECTIONAL:
			return std::string(Buffer) + "dir";
		case CInputParams::LT_POINT:
			return std::string(Buffer) + "point";
		case CInputParams::LT_SPOT:
			return std::string(Buffer) + "spot";
	}
	return "FUCK";
}

void
CActiveFragment_Light::GetIDs(INVLink* in_pLinker) {
	HRESULT Res;
	//
	Res = in_pLinker->GetFragmentID(m_FragmentName.c_str(), &m_FragmentID);
	assert( Res == S_OK );
	//
	Res = in_pLinker->GetConstantID(m_LightOrgName.c_str(), &m_ConstantID_LightOrg);
	assert( Res == S_OK );
	//
	Res = in_pLinker->GetConstantID(m_LightDirName.c_str(), &m_ConstantID_LightDir);
	assert( Res == S_OK );
	//
	Res = in_pLinker->GetConstantID(m_LightColorName.c_str(), &m_ConstantID_LightColor);
	assert( Res == S_OK );
	//
	Res = in_pLinker->GetConstantID(m_LightParamsName.c_str(), &m_ConstantID_LightParams);
	assert( Res == S_OK );
	//
	Res = in_pLinker->GetConstantID("c_one_zero", &m_ConstantID_WhiteColor);
	assert( Res == S_OK );
}

bool
CActiveFragment_Light::BelongsToCurrentSetup(const CShaderResources& rRes, const CInputParams& rParams) const {
	return rParams.m_LightEnabled && (rParams.m_Lights[m_N]==m_Type);
}

NVLINK_FRAGMENT_ID
CActiveFragment_Light::GetID() const {
	//assert(m_Type != CInputParams::LT_NONE);
	return m_FragmentID;
}

void
CActiveFragment_Light::GetConstants(INVLink* in_pLinker) {
	//assert(m_Type != CInputParams::LT_NONE);
	if(m_Type!=CInputParams::LT_NONE) {
		HRESULT Res;
		if(m_Type==CInputParams::LT_POINT || CInputParams::LT_SPOT) {
			Res = in_pLinker->GetConstantSlot(m_ConstantID_LightOrg, 0, &m_Slot_LightOrg);
			assert( S_OK == Res );
			Res = in_pLinker->GetConstantSlot(m_ConstantID_LightParams, 0, &m_Slot_LightParams);
			assert( S_OK == Res );
		}
		if(m_Type==CInputParams::LT_DIRECTIONAL|| CInputParams::LT_SPOT) {
			Res = in_pLinker->GetConstantSlot(m_ConstantID_LightDir, 0, &m_Slot_LightDir);
			assert( S_OK == Res );
		}
		Res = in_pLinker->GetConstantSlot(m_ConstantID_LightColor, 0, &m_Slot_LightColor);
		assert( S_OK == Res );
		Res = in_pLinker->GetConstantSlot(m_ConstantID_WhiteColor, 0, &m_Slot_WhiteColor);
		assert( S_OK == Res );
	}
}

std::map< unsigned long,unsigned long >
CActiveFragment_Light::GetBindings() const {
	//assert(m_Type != CInputParams::LT_NONE);
	std::map< unsigned long,unsigned long > Result;

	switch(m_Type) {
		case CInputParams::LT_NONE:
			break;
		case CInputParams::LT_DIRECTIONAL:
			Result.insert( std::map< unsigned long,unsigned long >::value_type(	VS_TOKEN_DEFAULT_CONSTANTS,		m_Slot_WhiteColor) );
			Result.insert( std::map< unsigned long,unsigned long >::value_type(	VS_TOKEN_LIGHT_0_COLOR+m_N*4,	m_Slot_LightColor) );
			Result.insert( std::map< unsigned long,unsigned long >::value_type(	VS_TOKEN_LIGHT_0_DIR+m_N*4,		m_Slot_LightDir) );
			break;
		case CInputParams::LT_POINT:
			Result.insert( std::map< unsigned long,unsigned long >::value_type(	VS_TOKEN_DEFAULT_CONSTANTS,		m_Slot_WhiteColor) );
			Result.insert( std::map< unsigned long,unsigned long >::value_type(	VS_TOKEN_LIGHT_0_COLOR+m_N*4,	m_Slot_LightColor) );
			Result.insert( std::map< unsigned long,unsigned long >::value_type(	VS_TOKEN_LIGHT_0_PARAMS+m_N*4,	m_Slot_LightParams) );
			break;
		case CInputParams::LT_SPOT:
			Result.insert( std::map< unsigned long,unsigned long >::value_type(	VS_TOKEN_DEFAULT_CONSTANTS,		m_Slot_WhiteColor) );
			Result.insert( std::map< unsigned long,unsigned long >::value_type(	VS_TOKEN_LIGHT_0_COLOR+m_N*4,	m_Slot_LightColor) );
			Result.insert( std::map< unsigned long,unsigned long >::value_type(	VS_TOKEN_LIGHT_0_DIR+m_N*4,		m_Slot_LightDir) );
			Result.insert( std::map< unsigned long,unsigned long >::value_type(	VS_TOKEN_LIGHT_0_PARAMS+m_N*4,	m_Slot_LightParams) );
			break;
	}

	return Result;
}

// ===================================================================================================================
// Light
// ===================================================================================================================

CActiveFragment_Output::CActiveFragment_Output(bool in_bDiffuseVertexColorPresent) :
	m_bDiffusePresent(in_bDiffuseVertexColorPresent)
{
}

std::string
CActiveFragment_Output::GetName() {
	if(m_bDiffusePresent)
		return "lighting_output_diffuse";
	else
		return "lighting_output";
}

void
CActiveFragment_Output::GetIDs(INVLink* in_pLinker) {
	HRESULT Res;
	if(m_bDiffusePresent) {
		Res = in_pLinker->GetFragmentID("f_lighting_output_diffuse", &m_FragmentID);
		assert( Res == S_OK );
	} else {
		Res = in_pLinker->GetFragmentID("f_lighting_output", &m_FragmentID);
		assert( Res == S_OK );
	}
}

bool
CActiveFragment_Output::BelongsToCurrentSetup(const CShaderResources& rRes, const CInputParams& rParams) const {
	return rParams.m_LightEnabled;
}

NVLINK_FRAGMENT_ID
CActiveFragment_Output::GetID() const {
	return m_FragmentID;
}

void
CActiveFragment_Output::GetConstants(INVLink* in_pLinker) {
}

std::map< unsigned long,unsigned long >
CActiveFragment_Output::GetBindings() const {
	std::map< unsigned long,unsigned long > Result;
	// TODO
	return Result;
}































