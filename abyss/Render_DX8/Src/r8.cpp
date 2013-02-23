#include "../Include/r8.h"
#include "../Include/algebra.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

CRenderDX8::CRenderDX8() :
	m_pD3D8(NULL)
	,m_pD3D8Device(NULL)
	,m_RenderFormat(D3DFMT_UNKNOWN)
	,m_VertexProcessType(RVP_UNKNOWN)
	,m_DotShaderHandle(0)
{
	m_ObjectTextures.SetInterface( &m_TextureManager );
	SetupShaderDefinitions();
}

CRenderDX8::~CRenderDX8()
{
	assert( m_pD3D8Device == NULL );
	m_ObjectTextures.SetInterface( NULL );
}

//#include "r8_shaders_code.cpp"

void
CRenderDX8::SetInterface(IDirect3D8* in_pD3D8, IDirect3DDevice8* in_pD3D8Device, D3DFORMAT in_RenderFormat, RENDER_VERTEX_PROCESS_TYPE in_VertexProcessType )
{
	CLog::Print("CRenderDX8::SetInterface()\n");
	CLog::Print("  in_pD3D8        = %p\n",in_pD3D8);
	CLog::Print("  in_pD3D8Device  = %p\n",in_pD3D8Device);
	CLog::Print("  D3DFMT_X8R8G8B8 = %s\n",(in_RenderFormat==D3DFMT_X8R8G8B8)?"YES":"no");
	CLog::Print("  D3DFMT_R5G6B5   = %s\n",(in_RenderFormat==D3DFMT_R5G6B5)?"YES":"no");
	CLog::Print("  D3DFMT_UNKNOWN  = %s\n",(in_RenderFormat==D3DFMT_UNKNOWN)?"YES":"no");
	CLog::Print("  RVP_SOFTWARE    = %s\n",(in_VertexProcessType==RVP_SOFTWARE)?"YES":"no");
	CLog::Print("  RVP_MIXED       = %s\n",(in_VertexProcessType==RVP_MIXED)?"YES":"no");
	CLog::Print("  RVP_HARDWARE    = %s\n",(in_VertexProcessType==RVP_HARDWARE)?"YES":"no");
	assert(
		(	in_pD3D8 &&
			in_pD3D8Device &&
			( (in_RenderFormat==D3DFMT_X8R8G8B8) || (in_RenderFormat==D3DFMT_R5G6B5) ) &&
			(in_VertexProcessType!=RVP_UNKNOWN)
		)
		||
		(	!in_pD3D8 &&
			!in_pD3D8Device &&
			(in_RenderFormat==D3DFMT_UNKNOWN) &&
			(in_VertexProcessType==RVP_UNKNOWN)
		)
	);
	if(in_pD3D8)
	{
		assert(	!m_pD3D8 &&
				!m_pD3D8Device &&
				m_RenderFormat==D3DFMT_UNKNOWN &&
				m_VertexProcessType==RVP_UNKNOWN
		);
		m_pD3D8 = in_pD3D8;
		m_pD3D8Device = in_pD3D8Device;
		m_RenderFormat = in_RenderFormat;
		m_VertexProcessType = in_VertexProcessType;
		m_TextureManager.SetInterface(in_pD3D8,in_pD3D8Device,in_RenderFormat);
		m_PixelShaderManager.SetInterface(in_pD3D8,in_pD3D8Device,&m_TextureManager,&m_ObjectTextures);
		CreateInternalFont();
		CreateDotShader();

		/*
		// TEST {
		// Create the shader declaration.
		DWORD dwDecl[] =
		{
			D3DVSD_STREAM(0),
			D3DVSD_REG(D3DVSDE_POSITION,  D3DVSDT_FLOAT3),
			D3DVSD_END()
		};
		HRESULT Res = m_pD3D8Device->CreateVertexShader(
			dwDecl
			,(DWORD*)g_ShaderCode
			,&m_VertexShader
			,(m_VertexProcessType == RVP_HARDWARE) ? 0 : D3DUSAGE_SOFTWAREPROCESSING
		);
		assert( Res == D3D_OK );
		// } TEST
		*/



	}
	else
	{
		assert(	m_pD3D8 &&
				m_pD3D8Device &&
				(m_RenderFormat==D3DFMT_X8R8G8B8||m_RenderFormat==D3DFMT_R5G6B5)&&
				m_VertexProcessType!=RVP_UNKNOWN
		);
		m_pD3D8 = NULL;
		m_pD3D8Device = NULL;
		m_RenderFormat = D3DFMT_UNKNOWN;
		m_VertexProcessType = RVP_UNKNOWN;
		m_TextureManager.SetInterface(NULL,NULL,D3DFMT_UNKNOWN);
		m_PixelShaderManager.SetInterface(NULL,NULL,NULL,&m_ObjectTextures);
	}
}

void
CRenderDX8::ReleaseEverything()
{
	// release fonts
	//		Font textures will be released in "release textures" section below.
	//		We only need to release font handles.
	m_FontsHandlesStorage.Clear();

	// release pixel shaders
	m_PixelShaderManager.ReleaseEverything();
	m_PixelShaderManager.SetInterface(NULL,NULL,NULL,NULL);

	// release vertex shaders

	// release textures
	m_TextureManager.ReleaseEverything();
	m_TextureManager.SetInterface(NULL,NULL,D3DFMT_UNKNOWN);

	// release index buffers

	// release vertex buffers
	for( CVBInfoMapIterator It=m_VBMap.begin(); It!=m_VBMap.end(); ++It )
	{
		if(It->second.m_pVB)
			It->second.m_pVB->Release();
	}
	m_VBMap.clear();
	m_VertexBuffersHandlesStorage.Clear();

	m_RenderFormat = D3DFMT_UNKNOWN;
	m_VertexProcessType = RVP_UNKNOWN;

	// release device (actually done in render_critical section )
	m_pD3D8Device=NULL;
	m_pD3D8=NULL;
}

bool
CRenderDX8::Present()
{
//CLog::Print("CRenderDX8::Present()\n");
	assert(m_pD3D8Device);

	//DrawString(0,0," !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`",m_FontShaderHandle,0xffffffff);
	//DrawString(0,20,"abcdefghijklmnopqrstuvwxyz{|}~",m_FontShaderHandle,0xffffffff);
	//TestFont();

	HRESULT Res = m_pD3D8Device->Present(NULL,NULL,NULL,NULL);
	if(D3DERR_DEVICELOST==Res)
	{
		CLog::Print("CRenderDX8 : device lost inside Present()\n");
//CLog::Print("CRenderDX8::Present() finish\n");
		return false;
	}
	else
	{
//CLog::Print("CRenderDX8::Present() finish\n");
		return true;
	}
}

void
CRenderDX8::PrepareToRestoreDevice()
{
	CLog::Print("CRenderDX8::PrepareToRestoreDevice()\n");
	// release all D3DPOOL_DEFAULT resources
	for( CVBInfoMapIterator It=m_VBMap.begin(); It!=m_VBMap.end(); ++It )
	{
		CVBInfo& rInfo = It->second;
		if(rInfo.m_bDynamic)
		{
			// dynamic vertex buffers have D3DPOOL_DEFAULT memory class
			rInfo.m_pVB->Release();
			rInfo.m_pVB = NULL;
		}
	}
}

void
CRenderDX8::RestoreResources()
{
	CLog::Print("CRenderDX8::RestoreResources()\n");
	RestoreVertexBuffers();
}

void
CRenderDX8::Clear(unsigned long in_Color)
{
	assert(m_pD3D8Device);
	m_pD3D8Device->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,in_Color,1.0f,0);
}

void
CRenderDX8::SetViewport(	unsigned long X, unsigned long Y,
							unsigned long Width, unsigned long Height,
							float MinZ, float MaxZ	)
{
	assert(m_pD3D8Device);
	D3DVIEWPORT8 VP;
	VP.X = X;
	VP.Y = Y;
	VP.Width = Width;
	VP.Height = Height;
	VP.MinZ = MinZ;
	VP.MaxZ = MaxZ;
	m_pD3D8Device->SetViewport( &VP );
}

void
CRenderDX8::SetCamera(	const CVector& in_Org, const CVector& in_Dir, const CVector& in_Right,
						float in_FovX, float in_FovY,
						float in_Znear, float in_Zfar )
{
//CLog::Print("CRenderDX8::SetCamera()\n");
	assert(m_pD3D8Device);
//CLog::Print(" -1-\n");
	// TODO: check if vector basis is ortogonal and unit-length
	CVector CamUp = in_Right*in_Dir;
//CLog::Print(" -2-\n");
	CMatrix View(
		in_Right.x,			CamUp.x,			in_Dir.x,			0,
		in_Right.y,			CamUp.y,			in_Dir.y,			0,
		in_Right.z,			CamUp.z,			in_Dir.z,			0,
		-(in_Right^in_Org),	-(CamUp^in_Org),	-(in_Dir^in_Org),	1
	);
//CLog::Print(" -3-\n");
	SetViewMatrix( View );
//CLog::Print(" -4-\n");

	float Q = in_Zfar/(in_Zfar-in_Znear);
	CMatrix Project(
		1.0f/tan(0.5f*in_FovX),	0,						0,		0,
		0,						1.0f/tan(0.5f*in_FovY),	0,		0,
		0,						0,						Q,		1,
		0,						0,						-Q*in_Znear,	0
	);
//CLog::Print(" -5-\n");
	SetProjectionMatrix( Project );
//CLog::Print("CRenderDX8::SetCamera() finish\n");
}

void
CRenderDX8::SetViewMatrix( const CMatrix& in_View )
{
	HRESULT Res = m_pD3D8Device->SetTransform( D3DTS_VIEW, (const D3DMATRIX*)&in_View );
	assert(Res==D3D_OK);
	m_ViewMatrix = in_View;
}

void
CRenderDX8::SetProjectionMatrix( const CMatrix& in_Proj )
{
	HRESULT Res = m_pD3D8Device->SetTransform( D3DTS_PROJECTION, (const D3DMATRIX*)&in_Proj );
	assert(Res==D3D_OK);
	m_ProjMatrix = in_Proj;
}















