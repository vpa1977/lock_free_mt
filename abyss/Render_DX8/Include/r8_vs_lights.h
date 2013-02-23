#ifndef R8_VS_LIGHTS_HPP_INCLUDED
#define R8_VS_LIGHTS_HPP_INCLUDED

enum VS_LIGHT_TYPE {
	VSLT_NONE			= 0
	,VSLT_DIRECTIONAL	= 1
	,VSLT_POINT			= 2
	,VSLT_SPOT			= 3
};

// get
#define LIGHT_ENABLED_FROM_VS_CODE(x) ( (x&0xFFFF0000) != 0 )
#define LIGHT0_FROM_VS_CODE(x) ( (VS_LIGHT_TYPE) ( x     &0x0F) )
#define LIGHT1_FROM_VS_CODE(x) ( (VS_LIGHT_TYPE) ((x>> 4)&0x0F) )
#define LIGHT2_FROM_VS_CODE(x) ( (VS_LIGHT_TYPE) ((x>> 8)&0x0F) )
#define LIGHT3_FROM_VS_CODE(x) ( (VS_LIGHT_TYPE) ((x>>12)&0x0F) )

// construct
#if MAX_LIGHTS==4
	#define VS_CODE_FOR_LIGHTS(EN,L0,L1,L2,L3) ( EN ? ( 0xFFFF0000 | (((int)L0)&0x0F) | ((((int)L1)&0x0F)<<4) | ((((int)L2)&0x0F)<<8) | ((((int)L3)&0x0F)<<12) ) : 0 )
#elif MAX_LIGHTS==3
	#define VS_CODE_FOR_LIGHTS(EN,L0,L1,L2) ( EN ? ( 0xFFFFF000 | (((int)L0)&0x0F) | ((((int)L1)&0x0F)<<4) | ((((int)L2)&0x0F)<<8) ) : 0 )
#elif MAX_LIGHTS==2
	#define VS_CODE_FOR_LIGHTS(EN,L0,L1) ( EN ? ( 0xFFFFFF00 | (((int)L0)&0x0F) | ((((int)L1)&0x0F)<<4) ) : 0 )
#elif MAX_LIGHTS==1
	#define VS_CODE_FOR_LIGHTS(EN,L0) ( EN ? ( 0xFFFFFFF0 | (((int)L0)&0x0F) ) : 0 )
#elif
	#error MAX_LIGHTS must be between 1 and 4
#endif


#endif















