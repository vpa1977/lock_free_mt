#ifndef INPUT_PARAMS_HPP_INCLUDED
#define INPUT_PARAMS_HPP_INCLUDED

#include <set>

#include "../../Render_DX8/Include/r8_settings.h"

struct CInputParams {
	enum TEXTURE_TYPE {
		TT_NONE					= 0
		,TT_PASSTHRU			= 1
		,TT_CAMSPACE_POSITION	= 2
		,TT_CAMSPACE_NORMAL		= 3
		,TT_CAMSPACE_REFLECTION	= 4
	};
	enum LIGHT_TYPE {
		LT_NONE			= 0
		,LT_DIRECTIONAL	= 1
		,LT_POINT		= 2
		,LT_SPOT		= 3
	};

	CInputParams() :
		m_NBones(0), m_Normal(false), m_Diffuse(false)
		,m_LightEnabled(false)
		//,m_Light0(LT_NONE), m_Light1(LT_NONE), m_Light2(LT_NONE), m_Light3(LT_NONE)
		,m_Tex0(TT_NONE), m_Tex1(TT_NONE)
	{
		for(long i=0; i<MAX_LIGHTS; ++i)
			m_Lights[i] = LT_NONE;
	}

	//bool m_XYZ; // always true
	long m_NBones; // 0..4
	bool m_Normal;
	bool m_Diffuse;

	bool m_LightEnabled;
	LIGHT_TYPE m_Lights[MAX_LIGHTS];
	//LIGHT_TYPE m_Light0;
	//LIGHT_TYPE m_Light1;
	//LIGHT_TYPE m_Light2;
	//LIGHT_TYPE m_Light3;

	TEXTURE_TYPE m_Tex0;
	TEXTURE_TYPE m_Tex1;

	bool operator < ( const CInputParams& other ) const {
		if( m_NBones != other.m_NBones )
			return m_NBones < other.m_NBones;
		if( m_Normal != other.m_Normal )
			return m_Normal==false;
		if( m_Diffuse != other.m_Diffuse )
			return m_Diffuse==false;
		if( m_LightEnabled != other.m_LightEnabled )
			return m_LightEnabled==false;

		for(long i=0; i<MAX_LIGHTS; ++i)
			if(m_Lights[i] != other.m_Lights[i])
				return ((int)m_Lights[i]) < ((int)other.m_Lights[i]);
		//if( m_Light0 != other.m_Light0 )
		//	return ((int)m_Light0) < ((int)other.m_Light0);
		//if( m_Light1 != other.m_Light1 )
		//	return ((int)m_Light1) < ((int)other.m_Light1);
		//if( m_Light2 != other.m_Light2 )
		//	return ((int)m_Light2) < ((int)other.m_Light2);
		//if( m_Light3 != other.m_Light3 )
		//	return ((int)m_Light3) < ((int)other.m_Light3);

		if( m_Tex0 != other.m_Tex0 )
			return ((int)m_Tex0) < ((int)other.m_Tex0);
		return ((int)m_Tex1) < ((int)other.m_Tex1);
	}
};

void NormalizeParamsSet( const CInputParams& in_rSrc, CInputParams& out_rDst );

bool GetNextParamSet(const CInputParams& in_rSet, std::set<CInputParams>& in_Processed, CInputParams& in_rNext);

#endif



































