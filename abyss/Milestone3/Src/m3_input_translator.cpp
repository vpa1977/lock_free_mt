#include "../Include/m3_input_translator.h"
#include <assert.h>

CTranslatedInfo
TranslateMouseCoordinates( float in_MX, float in_MY, CCameraInfoProvider* in_pInfoProvider )
{
	assert( in_pInfoProvider );
	CCameraInfo& rCamInfo = in_pInfoProvider->GetCameraInfo();
	//
	CMatrix ProjectionInverse( rCamInfo.m_ProjectionMatrix );
	ProjectionInverse.Invert();
	//
	CMatrix ViewInverse( rCamInfo.m_ViewMatrix );
	ViewInverse.Invert();
	//
	CVector ScreenPoint( in_MX, in_MY, 1.0f );
	CVector WorldPoint( ScreenPoint*ProjectionInverse*ViewInverse );
	//
	CTranslatedInfo TranslatedInfo;
	TranslatedInfo.m_Org = rCamInfo.m_CameraOrg;
	TranslatedInfo.m_Dir = (WorldPoint-rCamInfo.m_CameraOrg).Normalize();
	return TranslatedInfo;
}
