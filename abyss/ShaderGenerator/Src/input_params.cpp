#include "input_params.h"
#include <algorithm>


bool NextLightType( const CInputParams::LIGHT_TYPE& in, CInputParams::LIGHT_TYPE& out ) {
	if(in==CInputParams::LT_SPOT) {
		out = CInputParams::LT_NONE;
		return true;
	}
	out = (CInputParams::LIGHT_TYPE)( ((int)in) + 1 );
	return false;
}

bool NextTextureType( const CInputParams::TEXTURE_TYPE& in_TT, CInputParams::TEXTURE_TYPE& out_TT ) {
	if( in_TT == CInputParams::TT_CAMSPACE_REFLECTION ) {
		out_TT = CInputParams::TT_NONE;
		return true;
	} else {
		out_TT = (CInputParams::TEXTURE_TYPE)( ((int)in_TT) + 1 );
		return false;
	}
}

bool GetNextParamSet(const CInputParams& in_rSet, std::set<CInputParams>& in_Processed, CInputParams& in_rNext) {
	in_rNext = in_rSet;

	for(;;) {
		bool PseudoSuccess = true;
		do {
			bool NextDigit;
			
			NextDigit = false;
			in_rNext.m_NBones++;
			if( in_rNext.m_NBones > 4 ) {
				in_rNext.m_NBones = 0;
				NextDigit = true;
			}
			if( !NextDigit )
				break;

			NextDigit = false;
			in_rNext.m_Normal = !in_rNext.m_Normal;
			if( !in_rNext.m_Normal )
				NextDigit = true;
			if( !NextDigit )
				break;

			NextDigit = false;
			in_rNext.m_Diffuse = !in_rNext.m_Diffuse;
			if( !in_rNext.m_Diffuse )
				NextDigit = true;
			if( !NextDigit )
				break;

			NextDigit = false;
			in_rNext.m_LightEnabled = !in_rNext.m_LightEnabled;
			if( !in_rNext.m_LightEnabled )
				NextDigit = true;
			if( !NextDigit )
				break;

			for(long i=0; i<MAX_LIGHTS; ++i) {
				NextDigit = NextLightType( in_rNext.m_Lights[i], in_rNext.m_Lights[i] );
				if( !NextDigit )
					break;
			}
			if( !NextDigit )
				break;
			//NextDigit = NextLightType( in_rNext.m_Light0, in_rNext.m_Light0 );
			//if( !NextDigit )
			//	break;
			//NextDigit = NextLightType( in_rNext.m_Light1, in_rNext.m_Light1 );
			//if( !NextDigit )
			//	break;
			//NextDigit = NextLightType( in_rNext.m_Light2, in_rNext.m_Light2 );
			//if( !NextDigit )
			//	break;
			//NextDigit = NextLightType( in_rNext.m_Light3, in_rNext.m_Light3 );
			//if( !NextDigit )
			//	break;

			NextDigit = NextTextureType(in_rNext.m_Tex0,in_rNext.m_Tex0);
			if( !NextDigit )
				break;

			NextDigit = NextTextureType(in_rNext.m_Tex1,in_rNext.m_Tex1);
			if( !NextDigit )
				break;

			PseudoSuccess = false;
		} while(false);

		if(!PseudoSuccess)
			return false;

		if( !in_rNext.m_Normal && ((int)in_rNext.m_Tex0)>CInputParams::TT_PASSTHRU ) {
			int qq = 666;
		}

		//
		// filter out impossible or unnesessary combinations
		//

		if( !in_rNext.m_Normal && in_rNext.m_LightEnabled )
			continue;

		if( !in_rNext.m_LightEnabled ) {
			bool bActiveLightFound = false;
			for(long i=0; i<MAX_LIGHTS; ++i) {
				if( in_rNext.m_Lights[i]!=CInputParams::LT_NONE ) {
					bActiveLightFound = true;
					break;
				}
			}
			if(bActiveLightFound)
				continue;
		}
		//if( !in_rNext.m_LightEnabled &&
		//	(	in_rNext.m_Light0!=CInputParams::LT_NONE ||
		//		in_rNext.m_Light1!=CInputParams::LT_NONE ||
		//		in_rNext.m_Light2!=CInputParams::LT_NONE ||
		//		in_rNext.m_Light3!=CInputParams::LT_NONE
		//	)
		//)
		//	continue;

		bool NullLightInTheBeginningFound = false;
		for(long i=0; i<MAX_LIGHTS-1 && !NullLightInTheBeginningFound; ++i) {
			if( in_rNext.m_Lights[i] == CInputParams::LT_NONE ) {
				for(long j=i+1; j<MAX_LIGHTS && !NullLightInTheBeginningFound; ++j) {
					if( in_rNext.m_Lights[j] != CInputParams::LT_NONE ) {
						NullLightInTheBeginningFound = true;
					}
				}
			}
		}
		if(NullLightInTheBeginningFound)
			continue;
		//if( in_rNext.m_Light1!=CInputParams::LT_NONE && (in_rNext.m_Light0==CInputParams::LT_NONE) )
		//	continue;
		//if( in_rNext.m_Light2!=CInputParams::LT_NONE && (in_rNext.m_Light0==CInputParams::LT_NONE || in_rNext.m_Light1==CInputParams::LT_NONE) )
		//	continue;
		//if( in_rNext.m_Light3!=CInputParams::LT_NONE && (in_rNext.m_Light0==CInputParams::LT_NONE || in_rNext.m_Light1==CInputParams::LT_NONE || in_rNext.m_Light2==CInputParams::LT_NONE) )
		//	continue;

		if( !in_rNext.m_Normal &&
			(	in_rNext.m_Tex0==CInputParams::TT_CAMSPACE_NORMAL ||
				in_rNext.m_Tex0==CInputParams::TT_CAMSPACE_REFLECTION ||
				in_rNext.m_Tex1==CInputParams::TT_CAMSPACE_NORMAL ||
				in_rNext.m_Tex1==CInputParams::TT_CAMSPACE_REFLECTION
			)
		)
			continue;

		if( in_rNext.m_Tex1!=CInputParams::TT_NONE && in_rNext.m_Tex0==CInputParams::TT_NONE )
			continue;

		int nGeneratedTextures = 0;
		if( in_rNext.m_Tex0 > CInputParams::TT_PASSTHRU )
			++nGeneratedTextures;
		if( in_rNext.m_Tex1 > CInputParams::TT_PASSTHRU )
			++nGeneratedTextures;
		if(nGeneratedTextures>1)
			continue;

		CInputParams Test;
		NormalizeParamsSet(in_rNext,Test);
		//if( Test.m_NBones==0
		//	&& Test.m_Normal
		//	&& !Test.m_Diffuse
		//	&& Test.m_LightEnabled
		//	&& Test.m_Light0==CInputParams::LT_SPOT
		//	&& Test.m_Light1==CInputParams::LT_POINT
		//	&& Test.m_Light2==CInputParams::LT_DIRECTIONAL
		//	&& Test.m_Light3==CInputParams::LT_NONE
		//	&& Test.m_Tex0==CInputParams::TT_NONE
		//	&& Test.m_Tex1==CInputParams::TT_NONE
		//)
		//{
		//	int i = 666;
		//}
		//size_t Size = in_Processed.size();
		std::pair< std::set<CInputParams>::iterator, bool > Result = in_Processed.insert(Test);
		if( Result.second == false )
			continue;

		return true;

	} // for

	return true;
}

void
NormalizeParamsSet( const CInputParams& in_rSrc, CInputParams& out_rDst ) {
	// find out normalized lights set
	int Lights[MAX_LIGHTS];
	for(long i=0; i<MAX_LIGHTS; ++i)
		Lights[i] = in_rSrc.m_Lights[i];
	std::sort(Lights,Lights+MAX_LIGHTS);
	//Lights[0] = in_rSrc.m_Light0;
	//Lights[1] = in_rSrc.m_Light1;
	//Lights[2] = in_rSrc.m_Light2;
	//Lights[3] = in_rSrc.m_Light3;
	//std::sort(Lights,Lights+4);

	// find out normalized texture set
	CInputParams::TEXTURE_TYPE Texs[2];
	if( in_rSrc.m_Tex0 > CInputParams::TT_PASSTHRU && in_rSrc.m_Tex1 == CInputParams::TT_PASSTHRU ) {
		Texs[0] = in_rSrc.m_Tex1;
		Texs[1] = in_rSrc.m_Tex0;
	} else {
		Texs[0] = in_rSrc.m_Tex0;
		Texs[1] = in_rSrc.m_Tex1;
	}

	// construct result
	out_rDst = in_rSrc;
	for(long i=0; i<MAX_LIGHTS; ++i)
		out_rDst.m_Lights[i] = (CInputParams::LIGHT_TYPE)(Lights[MAX_LIGHTS-1-i]);
	//out_rDst.m_Light0 = (CInputParams::LIGHT_TYPE)(Lights[3]);
	//out_rDst.m_Light1 = (CInputParams::LIGHT_TYPE)(Lights[2]);
	//out_rDst.m_Light2 = (CInputParams::LIGHT_TYPE)(Lights[1]);
	//out_rDst.m_Light3 = (CInputParams::LIGHT_TYPE)(Lights[0]);
	out_rDst.m_Tex0 = Texs[0];
	out_rDst.m_Tex1 = Texs[1];
}































