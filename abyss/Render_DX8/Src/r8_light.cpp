#include "../Include/r8.h"
#include "../Include/algebra.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

void
CRenderDX8::SetAmbient( unsigned long in_AmbientColor )
{
	assert(m_pD3D8Device);
	HRESULT Res = m_pD3D8Device->SetRenderState( D3DRS_AMBIENT, in_AmbientColor );
	assert( Res == D3D_OK );
}

void
CRenderDX8::SetDirectionalLight( const CVector& in_Dir, float in_R, float in_G, float in_B )
{
	assert(m_pD3D8Device);
	HRESULT Res;

	// light
	D3DLIGHT8 Light;
	memset( &Light, 0, sizeof(D3DLIGHT8) );
	Light.Type = D3DLIGHT_DIRECTIONAL;
	Light.Diffuse.r = in_R;
	Light.Diffuse.g = in_G;
	Light.Diffuse.b = in_B;
	Light.Diffuse.a = 0.0f;
	Light.Specular.r = Light.Specular.g = Light.Specular.b = Light.Specular.a = 0.0f;
	Light.Ambient.r = Light.Ambient.g = Light.Ambient.b = Light.Ambient.a = 0.0f;
	Light.Direction.x = in_Dir.x;
	Light.Direction.y = in_Dir.y;
	Light.Direction.z = in_Dir.z;
	Res = m_pD3D8Device->SetLight( 0, &Light );
	assert( Res == D3D_OK );
	Res = m_pD3D8Device->LightEnable( 0, TRUE );
	assert( Res == D3D_OK );
}

void
CRenderDX8::SetPointLight( const CVector& in_Org, float in_R, float in_G, float in_B )
{
	assert(m_pD3D8Device);
	HRESULT Res;

	// light
	D3DLIGHT8 Light;
	memset( &Light, 0, sizeof(D3DLIGHT8) );
	Light.Type = D3DLIGHT_POINT;
	Light.Diffuse.r = in_R;
	Light.Diffuse.g = in_G;
	Light.Diffuse.b = in_B;
	Light.Diffuse.a = 0.0f;
	Light.Specular.r = Light.Specular.g = Light.Specular.b = Light.Specular.a = 0.0f;
	Light.Ambient.r = Light.Ambient.g = Light.Ambient.b = Light.Ambient.a = 0.0f;
	Light.Position.x = in_Org.x;
	Light.Position.y = in_Org.y;
	Light.Position.z = in_Org.z;
	Light.Range = 1000000.0f;
	Light.Attenuation0 = 1.0f;
	Light.Attenuation1 = 0.0f;
	Light.Attenuation2 = 0.0f;
	Res = m_pD3D8Device->SetLight( 0, &Light );
	assert( Res == D3D_OK );
	Res = m_pD3D8Device->LightEnable( 0, TRUE );
	assert( Res == D3D_OK );
}


unsigned long
CRenderDX8::GetLightsCode() {
	return 0;
}

























