#ifndef R8_VS_TEXTURES_HPP_INCLUDED
#define R8_VS_TEXTURES_HPP_INCLUDED

enum VS_TEXTURE_TYPE {
	VSTT_NONE			= 0
	,VSTT_DIRECTIONAL	= 1
	,VSTT_POINT			= 2
	,VSTT_SPOT			= 3
};

// get
#define TEX0_FROM_VS_CODE(x) ( (VS_TEXTURE_TYPE) ( x    &0x0F) )
#define TEX1_FROM_VS_CODE(x) ( (VS_TEXTURE_TYPE) ((x>>4)&0x0F) )

// construct
#define VS_CODE_FOR_TEXTURES(T0,T1) ( (((int)T0)&0x0F) | ((((int)T1)&0x0F)<<4) )

#endif















