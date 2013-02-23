#ifndef M3_INPUT_TRANSLATOR_HPP_INCLUDED
#define M3_INPUT_TRANSLATOR_HPP_INCLUDED

#include "../../Render_DX8/Include/algebra.h"

struct CCameraInfo
{
	CMatrix m_ViewMatrix;
	CMatrix m_ProjectionMatrix;
	CVector m_CameraOrg;
};

class CCameraInfoProvider
{
public:
	virtual CCameraInfo& GetCameraInfo() = 0;
};

struct CTranslatedInfo
{
	CVector m_Org;
	CVector m_Dir;
};

CTranslatedInfo TranslateMouseCoordinates( float in_MX, float in_MY, CCameraInfoProvider* in_pInfoProvider );

#endif














