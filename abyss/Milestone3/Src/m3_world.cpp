#include "../Include/m3_world.h"

#include "../Include/m3_movecounter.h"
#include "../Include/m3_shufflereminder.h"
#include "../Include/m3_starfield.h"
#include "../Include/m3_positionreminder.h"
#include "../Include/m3_gamedevice.h"
#include "../Include/font_loader.h"

#include "../../MT_Core/Include/log.h"
#include "../../MT_Core/Include/rtassert.h"
#include <assert.h>

CWorld::CWorld( CGameObjectOwner* in_pOwner ) :
	CGameObject( in_pOwner )
	,m_bSelfPreloadComplete(false)
	,m_bWasErrorDuringPreload(false)
{
	std::vector< CGameObject* >& rChildren = GetChildren();

	rChildren.push_back( new CStarField(this) );
	m_ChildrenWithPendingPreload.insert( rChildren.back() );
	
	rChildren.push_back( new CGameDevice(this) );
	m_ChildrenWithPendingPreload.insert( rChildren.back() );

	rChildren.push_back( new CPositionReminder(this) );
	m_ChildrenWithPendingPreload.insert( rChildren.back() );

	//m_pMoveCounter = new CMoveCounter(this);
	//rChildren.push_back( m_pMoveCounter );
	//m_ChildrenWithPendingPreload.insert( rChildren.back() );

	rChildren.push_back( new CShuffleReminder(this) );
	m_ChildrenWithPendingPreload.insert( rChildren.back() );

	m_CamOrg = CVector( 0.0f, 0.0f, 10.0f );
	m_CamDir = CVector( 0.0f, 0.0f, -1.0f );
	m_CamRight = CVector( 1.0f, 0.0f, 0.0f );
	m_FOVX = 0.75f*CONST_PI_2;
	m_FOVY = 0.75f*CONST_PI_2;
	m_ZNear = 0.01f;
	m_ZFar = 100.0f;
}

CWorld::~CWorld()
{
	// TODO: delete children
}

void
CWorld::ReadFileResult( bool in_bSuccess, const std::string& in_rFileName, const std::vector<unsigned char>& in_rData )
{ RTASSERT(false); }

void
CWorld::DecodeFileResult( bool in_bSuccess, const std::string& in_rFileName, long in_Width, long in_Height, const std::vector<unsigned char>& in_rData )
{
	CFontDescription FD;
	bool Ok = LoadFontFromBitmap(in_rData,in_Width,in_Height,32,FD);
	RTASSERT(Ok);
	CRenderProxy::getInstance().CreateFont( FD, this );
}

void
CWorld::DecodeRawResult( bool in_bSuccess, long in_Width, long in_Height, const std::vector<unsigned char>& in_rData )
{ RTASSERT(false); }
void
CWorld::CreateVBResult( unsigned long in_VBHandle )
{ RTASSERT(false); }
void
CWorld::UpdateVBResult( bool in_bSuccess )
{ RTASSERT(false); }
void
CWorld::CreateTextureResult( unsigned long in_TexHandle )
{ RTASSERT(false); }
void
CWorld::CreatePSResult( unsigned long in_ShaderHandle )
{ RTASSERT(false); }

void
CWorld::CreateFontResult(unsigned long in_FontHandle)
{
	RTASSERT(in_FontHandle>1);
	m_bSelfPreloadComplete = true;
	CheckPreloadComplete();
}


void
CWorld::SomeObjectPreloadComplete( bool in_bSuccess, void* in_Handle )
{
	// Check that in_Handle is in both
	//   GetChildren() and m_ChildrenWithPendingPreload.
	bool FoundInChildren = false;
	std::vector< CGameObject* >& rChildren = GetChildren();
	for( std::vector< CGameObject* >::iterator ItV = rChildren.begin(); ItV!=rChildren.end(); ++ItV )
	{
		if( *ItV == in_Handle )
		{
			FoundInChildren = true;
			break;
		}
	}
	std::set<CGameObject*>::iterator ItS = m_ChildrenWithPendingPreload.find( (CGameObject*)in_Handle );
	bool FoundInPending = ItS!=m_ChildrenWithPendingPreload.end();
	assert(FoundInChildren && FoundInPending);

	m_bWasErrorDuringPreload = m_bWasErrorDuringPreload || (!in_bSuccess);
	m_ChildrenWithPendingPreload.erase( ItS );

	CheckPreloadComplete();
}

void
CWorld::vStartPreload()
{
	CLog::Print("CWorld::CheckPreloadComplete()\n");
	CLoadProxy::getInstance().DecodeImageFromFile( "/data/font2.tga", this );
	
}

void
CWorld::vUpdateBeforeChildren( unsigned long in_Time )
{
 	CRenderProxy::getInstance().SetCamera(
		CRenderSection_SetCamera(	m_CamOrg, m_CamDir, m_CamRight,
 									m_FOVX, m_FOVY,
 									m_ZNear, m_ZFar
		)
	);
#define LIGHT_ROTATE_MILISECONDS 10000
#define LIGHT_ROTATE_RADIUS 10.0f
#define LIGHT_ROTATE_DISTANCE 10.0f
	unsigned long T = in_Time%LIGHT_ROTATE_MILISECONDS;
	float Angle = CONST_2PI*float(T)/float(LIGHT_ROTATE_MILISECONDS);
	CVector LightOrg( LIGHT_ROTATE_RADIUS*cos(Angle), LIGHT_ROTATE_RADIUS*sin(Angle), LIGHT_ROTATE_DISTANCE );
	CVector LightDir( -LightOrg ); LightDir.Normalize();
	CRenderProxy::getInstance().SetDirectionalLight(LightDir,1.0f,1.0f,1.0f);
 	CRenderProxy::getInstance().SetAmbient(0x808080);
}

void
CWorld::vUpdateAfterChildren()
{
	CRenderProxy::getInstance().Present( 0 );
}

bool
CWorld::vAcceptKey(unsigned int code)
{
////	{
//		assert(m_pMoveCounter);
//	}
	return false;
}

CCameraInfo g_CameraInfo;
CCameraInfo&
CWorld::GetCameraInfo()
{
	g_CameraInfo.m_CameraOrg = m_CamOrg;
	float Q = m_ZFar/(m_ZFar-m_ZNear);
	g_CameraInfo.m_ProjectionMatrix = CMatrix(
		1.0f/tan(0.5f*m_FOVX),	0,						0,			0,
		0,						1.0f/tan(0.5f*m_FOVY),	0,			0,
		0,						0,						Q,			1,
		0,						0,						-Q*m_ZNear,	0
	);
	CVector CamUp = m_CamRight*m_CamDir;
	g_CameraInfo.m_ViewMatrix = CMatrix(
		m_CamRight.x,			CamUp.x,			m_CamDir.x,			0,
		m_CamRight.y,			CamUp.y,			m_CamDir.y,			0,
		m_CamRight.z,			CamUp.z,			m_CamDir.z,			0,
		-(m_CamRight^m_CamOrg),	-(CamUp^m_CamOrg),	-(m_CamDir^m_CamOrg),	1
	);
	return g_CameraInfo;
}

void
CWorld::CheckPreloadComplete()
{
	CLog::Print("  CWorld::CheckPreloadComplete()\n");
	if( m_ChildrenWithPendingPreload.empty() && m_bSelfPreloadComplete )
	{
		PreloadComplete( !m_bWasErrorDuringPreload );
	}
}












