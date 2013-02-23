#include "../Include/rendercritical_section.h"

#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/log.h"

#include "../../App_Win32/Include/commandline.h"

#include <assert.h>

CRenderCriticalSection::CRenderCriticalSection( long in_SectionID ) :
	CSection( in_SectionID )
	,m_HWnd(NULL)
	,m_pD3D8(NULL)
	,m_pD3D8Device(NULL)
	,m_b16bitDepthOnly(false)
{
	//
	RegisterReaction( GetID((CCommand_SectionStartup*)NULL), CTConform<CRenderCriticalSection,CCommand_SectionStartup>::Conform );
	//
	RegisterReaction( GetID((CRenderSection_InitRender*)NULL), CTConform<CRenderCriticalSection,CRenderSection_InitRender>::Conform );
	RegisterReaction( GetID((CRenderSection_CloseRender*)NULL), CTConform<CRenderCriticalSection,CRenderSection_CloseRender>::Conform );
	//
	RegisterReaction( GetID((CRenderCriticalSection_BindToWindow*)NULL), CTConform<CRenderCriticalSection,CRenderCriticalSection_BindToWindow>::Conform );
	RegisterReaction( GetID((CRenderCriticalSection_UnbindAndDie*)NULL), CTConform<CRenderCriticalSection,CRenderCriticalSection_UnbindAndDie>::Conform );
	RegisterReaction( GetID((CRenderCriticalSection_Get3DEnvironmentInfo*)NULL), CTConform<CRenderCriticalSection,CRenderCriticalSection_Get3DEnvironmentInfo>::Conform );
	RegisterReaction( GetID((CRenderCriticalSection_RestoreDevice*)NULL), CTConform<CRenderCriticalSection,CRenderCriticalSection_RestoreDevice>::Conform );


}

CRenderCriticalSection::~CRenderCriticalSection()
{
	ReleaseRenderObjects();
	//assert(!m_pD3D8Device);
	//assert(!m_pD3D8);
}

// ============================================================================================

void
CRenderCriticalSection::Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )
{
	CLog::Print("CRenderCriticalSection::Reaction( const CCommand_SectionStartup& in_rStartupCommand )\n");
	CLog::Print("  ID = %ld\n",GetThisID());
	std::string CmdLine = CCommandLineParamerers::GetCommandLine();
	const char* P = strstr( CmdLine.c_str(), "-force16bitdepth" );
	m_b16bitDepthOnly = (P!=NULL);
}

// ============================================================================================

void
CRenderCriticalSection::Reaction( long in_SrcSectionID, const CRenderSection_InitRender& in_rInit )
{
	CLog::Print("CRenderCriticalSection::Reaction( const CRenderSection_InitRender& in_rInit )\n");
	CLog::Print("  width  = %lu\n",in_rInit.m_DX);
	CLog::Print("  height = %lu\n",in_rInit.m_DY);
	CLog::Print("  windowed = %s\n",in_rInit.m_bWindowed?"yes":"no");
	CLog::Print("  depth   = %s\n",in_rInit.m_bDepthBufferRequired?"yes":"no");
	CLog::Print("  stencil = %s\n",in_rInit.m_bStencilBufferRequired?"yes":"no");
	CLog::Print("  refresh = %lu\n",in_rInit.m_FullscreenRefreshRate);
	CLog::Print("  vsync = %s\n",in_rInit.m_bVSync?"yes":"no");
	assert(m_pD3D8==NULL);
	assert(m_HWnd!=NULL);

	assert(in_rInit.m_bWindowed); // TODO: fullscreen initialization

	D3DFORMAT BackBufferFormat;
	bool Ok = false;
	RENDER_VERTEX_PROCESS_TYPE VertexProcessingType = RVP_UNKNOWN;
	do
	{
		m_pD3D8 = Direct3DCreate8( D3D_SDK_VERSION );
		assert(m_pD3D8!=NULL);

		if(in_rInit.m_bWindowed)
		{
			D3DDISPLAYMODE Mode;
			HRESULT Res = m_pD3D8->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &Mode );
			assert(Res==D3D_OK);
			BackBufferFormat = Mode.Format;
		}
		else
		{
			assert(2==5);
			// TODO
		}
		if( TryToCreateDevice( in_rInit, BackBufferFormat, RVP_HARDWARE ) )
			VertexProcessingType = RVP_HARDWARE;
		else
		{
			if( TryToCreateDevice( in_rInit, BackBufferFormat, RVP_SOFTWARE ) )
				VertexProcessingType = RVP_SOFTWARE;
			else {
				assert(false);
			}
		}

		Ok = true;
	} while(false);

	if(!Ok)
	{
		ReleaseRenderObjects();
	}

	CTCommandSender<CRenderCriticalSection_InitRenderResult>::SendCommand(
		in_SrcSectionID,
		CRenderCriticalSection_InitRenderResult(
			Ok ? IRR_OK : IRR_FAIL,
			Ok ? m_pD3D8 : NULL,
			Ok ? m_pD3D8Device : NULL,
			Ok ? VertexProcessingType : RVP_UNKNOWN,
			Ok ? BackBufferFormat : D3DFMT_UNKNOWN
		)
	);
}

void
CRenderCriticalSection::Reaction( long in_SrcSectionID, const CRenderSection_CloseRender& in_rClose )
{
	CLog::Print("CRenderCriticalSection::Reaction( const CRenderSection_CloseRender& in_rClose )\n");
	ReleaseRenderObjects();
	CTCommandSender<CRenderSection_CloseRenderResult>::SendCommand(
		in_SrcSectionID,
		CRenderSection_CloseRenderResult(true)
	);
}

// ============================================================================================

void
CRenderCriticalSection::Reaction( long in_SrcSectionID, const CRenderCriticalSection_BindToWindow& in_rBind )
{
	CLog::Print("CRenderCriticalSection::Reaction( const CRenderCriticalSection_BindToWindow& in_rBind )\n");
	assert(!m_HWnd);
	assert(in_rBind.m_HWnd);
	m_HWnd = in_rBind.m_HWnd;
	CLog::Print("  srcID = %ld\n",in_SrcSectionID);
	CTCommandSender<CRenderCriticalSection_BoundToWindow>::SendCommand(
		in_SrcSectionID,
		CRenderCriticalSection_BoundToWindow( in_rBind.m_HWnd )
	);
}

void
CRenderCriticalSection::Reaction( long in_SrcSectionID, const CRenderCriticalSection_UnbindAndDie& in_rDie )
{
	CLog::Print("CRenderCriticalSection::Reaction( const CRenderCriticalSection_UnbindAndDie& in_rDie )\n");
	CLog::Print("  thisID = %ld\n",GetThisID());
	CLog::Print("  srcID  = %ld\n",in_SrcSectionID);
	ReleaseRenderObjects();
	Quit();
	CLog::Print("CRenderCriticalSection::Reaction( const CRenderCriticalSection_UnbindAndDie& in_rDie ) end\n");
}

const char* Format_16 = "D3DFMT_R5G6B5";
const char* Format_32 = "D3DFMT_X8R8G8B8";
const char* Format_U = "unknown format";

const char*
DisplayFormatToString( D3DFORMAT in_Format )
{
	if(in_Format==D3DFMT_R5G6B5)
	{
		return Format_16;
	}
	else if(in_Format==D3DFMT_X8R8G8B8)
	{
		return Format_32;
	}
	else
	{
		return Format_U;
	}
}

RENDER_DISPLAY_MODE_FORMAT
DisplayFormatToRDMF( D3DFORMAT in_Format )
{
	switch( in_Format )
	{
	case D3DFMT_R5G6B5:
		return RDMF_R5G6B5;
		break;
	case D3DFMT_X8R8G8B8:
		return RDMF_X8R8G8B8;
		break;
	}
	return RDMF_UNKNOWN;
}

struct CLocalD3D8Holder
{
	CLocalD3D8Holder(IDirect3D8* in_pD3D8) : m_pD3D8(in_pD3D8) {}
	~CLocalD3D8Holder()
	{
//		if(m_pD3D8)
//			m_pD3D8->Release();
	}
	IDirect3D8* m_pD3D8;
};

void
CRenderCriticalSection::Reaction( long in_SrcSectionID, const CRenderCriticalSection_Get3DEnvironmentInfo& in_rGetInfo )
{
	CLog::Print("CRenderCriticalSection::Reaction( const CRenderCriticalSection_Get3DEnvironmentInfo& in_rGetInfo )\n");
	assert(m_HWnd!=NULL);

	CRenderCriticalSection_3DEnvironmentInfo Collected3DInfo;
	Collected3DInfo.m_bError = true; // will be set to false after successfull 3d info query

	bool Have16bitModes = false;
	bool Have32bitModes = false;
	do
	{
		IDirect3D8* pD3D8 = (m_pD3D8!=NULL) ? m_pD3D8 : Direct3DCreate8( D3D_SDK_VERSION );
		CLocalD3D8Holder LocalD3D8Holder( (m_pD3D8!=NULL) ? NULL : pD3D8 );
		if(NULL==pD3D8)
		{
			break;
		}

		HRESULT res;

		// debug name
		D3DADAPTER_IDENTIFIER8 Ident;
		res = pD3D8->GetAdapterIdentifier( D3DADAPTER_DEFAULT, D3DENUM_NO_WHQL_LEVEL, &Ident );
		if(res!=D3D_OK)
		{
			break;
		}
		CLog::Print("  Primary display adapter name:\n");
		CLog::Print("    '%s'\n",Ident.Description);
		// fill in m_DisplayModes

		UINT NModes = pD3D8->GetAdapterModeCount( D3DADAPTER_DEFAULT );
		CLog::Print("  Primary display adapter has %u modes.\n", NModes);
		Collected3DInfo.m_DisplayModes.reserve(NModes);

		D3DDISPLAYMODE DisplayMode;
		for( UINT i=0; i<NModes; ++i )
		{
			HRESULT res = pD3D8->EnumAdapterModes(D3DADAPTER_DEFAULT,i,&DisplayMode);
			assert( D3D_OK == res );
			char Buffer[1024];
			sprintf(Buffer, "  %ux%u, %u hertz, ", DisplayMode.Width, DisplayMode.Height, DisplayMode.RefreshRate);
			strcat(Buffer,DisplayFormatToString(DisplayMode.Format));
			CLog::Print("%s\n",Buffer);
			//
			RENDER_DISPLAY_MODE_FORMAT RDMFormat = DisplayFormatToRDMF(DisplayMode.Format);
			if( RDMFormat!=RDMF_UNKNOWN )
			{
				Have16bitModes |= (RDMFormat==RDMF_R5G6B5);
				Have32bitModes |= (RDMFormat==RDMF_X8R8G8B8);
				Collected3DInfo.m_DisplayModes.push_back(
					CRenderDisplayMode(
						DisplayMode.Width,
						DisplayMode.Height,
						DisplayMode.RefreshRate,
						RDMFormat
					)
				);
			}
		}

		// fill in m_CanRenderWindowed and m_CurrentDisplayMode

		D3DCAPS8 Caps8;
		res = pD3D8->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&Caps8);
		assert( D3D_OK == res );

		CLog::Print("  D3DDEVCAPS_TLVERTEXSYSTEMMEMORY = %s\n",0!=(Caps8.DevCaps&D3DDEVCAPS_TLVERTEXSYSTEMMEMORY)?"YES":"no");
		CLog::Print("  D3DDEVCAPS_TLVERTEXVIDEOMEMORY = %s\n",0!=(Caps8.DevCaps&D3DDEVCAPS_TLVERTEXVIDEOMEMORY)?"YES":"no");
		CLog::Print("  MaxVertexIndex = %08x\n",Caps8.MaxVertexIndex);

		Collected3DInfo.m_CanRenderWindowed = 0!=(Caps8.Caps2&D3DCAPS2_CANRENDERWINDOWED);
		CLog::Print("  Primary display adapter %s render windowed.\n", Collected3DInfo.m_CanRenderWindowed?"CAN":"CANNOT");

		CLog::Print("  MaxTextureBlendStages    = %lu\n", Caps8.MaxTextureBlendStages);
		CLog::Print("  MaxSimultaneousTextures  = %lu\n", Caps8.MaxSimultaneousTextures);

		CLog::Print("  MaxVertexBlendMatrices    = %lu\n", Caps8.MaxVertexBlendMatrices);
		CLog::Print("  MaxVertexBlendMatrixIndex = %lu\n", Caps8.MaxVertexBlendMatrixIndex);

		long VSHi = (Caps8.VertexShaderVersion>>8) & 0xff;
		long VSLo = (Caps8.VertexShaderVersion   ) & 0xff;
		CLog::Print("  VertexShaderVersion  = %d.%d\n", VSHi, VSLo);
		CLog::Print("  MaxVertexShaderConst = %lu\n", Caps8.MaxVertexShaderConst);
		long PSHi = (Caps8.PixelShaderVersion>>8) & 0xff;
		long PSLo = (Caps8.PixelShaderVersion   ) & 0xff;
		CLog::Print("  PixelShaderVersion   = %d.%d\n", PSHi, PSLo);
		CLog::Print("  MaxPixelShaderValue  = %f\n", Caps8.MaxPixelShaderValue);

		res = pD3D8->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&DisplayMode);
		RENDER_DISPLAY_MODE_FORMAT RDMFormat = DisplayFormatToRDMF(DisplayMode.Format);
		assert( RDMFormat != RDMF_UNKNOWN );
		char Buffer[1024];
		sprintf(Buffer, "%ux%u, %u hertz, ", DisplayMode.Width, DisplayMode.Height, DisplayMode.RefreshRate);
		strcat(Buffer,DisplayFormatToString(DisplayMode.Format));
		CLog::Print("  Current display mode: %s\n",Buffer);
		Collected3DInfo.m_CurrentDisplayMode = CRenderDisplayMode(	DisplayMode.Width,
																	DisplayMode.Height,
																	DisplayMode.RefreshRate,
																	RDMFormat );
		if(RDMFormat==RDMF_R5G6B5)
		{
			assert(Have16bitModes);
		}
		else // RDMFormat==RDMF_X8R8G8B8
		{
			assert(Have32bitModes);
		}

		// fill in depth buffer flags

		if(Have16bitModes)
		{
			CLog::Print("  16bit:\n");
			Collected3DInfo.m_16Depth = (
				D3D_OK == pD3D8->CheckDeviceFormat(	D3DADAPTER_DEFAULT,
													D3DDEVTYPE_HAL,
													D3DFMT_R5G6B5,
													D3DUSAGE_DEPTHSTENCIL,
													D3DRTYPE_SURFACE,
													D3DFMT_D16)
			);
			CLog::Print("    Depth        = %s\n",Collected3DInfo.m_16Depth?"YES":"no");
			Collected3DInfo.m_16DepthStencil = false; // 16bit modes support only 1bit stencil, which is crap.
			CLog::Print("    DepthStencil = %s\n",Collected3DInfo.m_16DepthStencil?"YES":"no");
		}
		if(Have32bitModes)
		{
			CLog::Print("  32bit:\n");
			Collected3DInfo.m_32Depth = (
				D3D_OK==pD3D8->CheckDeviceFormat(	D3DADAPTER_DEFAULT,
													D3DDEVTYPE_HAL,
													D3DFMT_X8R8G8B8,
													D3DUSAGE_DEPTHSTENCIL,
													D3DRTYPE_SURFACE,
													D3DFMT_D32)
			) || (
				D3D_OK==pD3D8->CheckDeviceFormat(	D3DADAPTER_DEFAULT,
													D3DDEVTYPE_HAL,
													D3DFMT_X8R8G8B8,
													D3DUSAGE_DEPTHSTENCIL,
													D3DRTYPE_SURFACE,
													D3DFMT_D24X8)
			);
			CLog::Print("    Depth        = %s\n",Collected3DInfo.m_32Depth?"YES":"no");
			Collected3DInfo.m_32DepthStencil = (
				D3D_OK==pD3D8->CheckDeviceFormat(	D3DADAPTER_DEFAULT,
													D3DDEVTYPE_HAL,
													D3DFMT_X8R8G8B8,
													D3DUSAGE_DEPTHSTENCIL,
													D3DRTYPE_SURFACE,
													D3DFMT_D24S8)
			) || (
				D3D_OK==pD3D8->CheckDeviceFormat(	D3DADAPTER_DEFAULT,
													D3DDEVTYPE_HAL,
													D3DFMT_X8R8G8B8,
													D3DUSAGE_DEPTHSTENCIL,
													D3DRTYPE_SURFACE,
													D3DFMT_D24X4S4)
			);
			CLog::Print("    DepthStencil = %s\n",Collected3DInfo.m_32DepthStencil?"YES":"no");

		}

		pD3D8->Release();

		Collected3DInfo.m_bError = false;
	} while(false);
	CTCommandSender<CRenderCriticalSection_3DEnvironmentInfo>::SendCommand(
		in_SrcSectionID,
		Collected3DInfo
	);
	CLog::Print("CRenderCriticalSection::Reaction( const CRenderCriticalSection_Get3DEnvironmentInfo& in_rGetInfo ) end\n");
}

void
CRenderCriticalSection::Reaction( long in_SrcSectionID, const CRenderCriticalSection_RestoreDevice& in_rRestore )
{
	CLog::Print("CRenderCriticalSection::Reaction( const CRenderCriticalSection_RestoreDevice& in_rRestore )\n");
	CLog::Print("  thisID = %ld\n",GetThisID());
	CLog::Print("  srcID  = %ld\n",in_SrcSectionID);
	assert(m_pD3D8Device);
	assert(m_pD3D8);
	bool Ok = false;
	do
	{
		HRESULT Res = m_pD3D8Device->TestCooperativeLevel();
		if( D3D_OK == Res )
		{
			Ok = true;
			break;
		}
		if( D3DERR_DEVICELOST == Res )
		{
			Ok = false;
			break;
		}
		if( D3DERR_DEVICENOTRESET == Res )
		{
			Res = m_pD3D8Device->Reset(&m_PP);
			Ok = (Res==D3D_OK);
			break;
		}
	} while(false);
	CLog::Print("  restore %s\n",Ok?"SUCCEEDED":"FAILED");
	CTCommandSender<CRenderCriticalSection_RestoreDeviceComplete>::SendCommand(
		in_SrcSectionID,
		CRenderCriticalSection_RestoreDeviceComplete(Ok)
	);
	CLog::Print("CRenderCriticalSection::Reaction( const CRenderCriticalSection_RestoreDevice& in_rRestore ) end\n");
}

/*
void
CRenderCriticalSection::Reaction( long in_SrcSectionID, const CRenderCriticalSection_WindowClosed& in_rWindowClosed )
{
	CLog::Print("CRenderCriticalSection::Reaction( const CRenderCriticalSection_WindowClosed& in_rWindowClosed )\n");
	CLog::Print("  thisID = %ld\n",GetThisID());
	CLog::Print("  srcID  = %ld\n",in_SrcSectionID);
	ReleaseRenderObjects();
	CTCommandSender<CRenderCriticalSection_WindowClosedCleanupComplete>::SendCommand(
		in_SrcSectionID,
		CRenderCriticalSection_WindowClosedCleanupComplete( in_rWindowClosed.m_HWnd )
	);
	CLog::Print("CRenderCriticalSection::Reaction( const CRenderCriticalSection_WindowClosed& in_rWindowClosed ) end\n");
}


*/

bool
CRenderCriticalSection::TryToCreateDevice
( const CRenderSection_InitRender& in_rInit, D3DFORMAT in_BBFormat, RENDER_VERTEX_PROCESS_TYPE in_VertexProcessingType )
{
	CLog::Print("  CRenderCriticalSection::TryToCreateDevice()\n");
	DWORD BehaviorFlags;
	if(RVP_HARDWARE==in_VertexProcessingType)
	{
		BehaviorFlags = D3DCREATE_MULTITHREADED|D3DCREATE_HARDWARE_VERTEXPROCESSING;
		CLog::Print("    hardware vertex processing requested\n");
	}
	else if(RVP_MIXED==in_VertexProcessingType)
	{
		BehaviorFlags = D3DCREATE_MULTITHREADED|D3DCREATE_MIXED_VERTEXPROCESSING;
		CLog::Print("    mixed vertex processing requested\n");
	}
	else if(RVP_SOFTWARE==in_VertexProcessingType)
	{
		BehaviorFlags = D3DCREATE_MULTITHREADED|D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		CLog::Print("    software vertex processing requested\n");
	}
	else
	{
		assert(2==5);
	}

	bool Ok = false;
	do
	{
		m_PP.BackBufferWidth = in_rInit.m_DX;
		m_PP.BackBufferHeight = in_rInit.m_DY;
		m_PP.BackBufferFormat = in_BBFormat;
		m_PP.BackBufferCount = 1;
		m_PP.MultiSampleType = D3DMULTISAMPLE_NONE;
		m_PP.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_PP.hDeviceWindow = NULL;
		m_PP.Windowed = in_rInit.m_bWindowed ? TRUE : FALSE;
		m_PP.EnableAutoDepthStencil = in_rInit.m_bDepthBufferRequired ? TRUE : FALSE;
		if(in_rInit.m_bDepthBufferRequired)
		{
			D3DFORMAT DepthFormat;
			if(!ChooseDepthFormat(in_BBFormat,in_rInit.m_bStencilBufferRequired,DepthFormat))
				break;
			m_PP.AutoDepthStencilFormat = DepthFormat;
		}
		m_PP.Flags = 0;
		m_PP.FullScreen_RefreshRateInHz = 0;
		m_PP.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

		HRESULT Res = m_pD3D8->CreateDevice(	D3DADAPTER_DEFAULT,
												D3DDEVTYPE_HAL,
												m_HWnd,
												BehaviorFlags,
												&m_PP,
												&m_pD3D8Device );
		CLog::Print("    %s (Res=%x)\n", (Res==D3D_OK)? "SUCCESS":"FAIL", Res );
		Ok = (Res==D3D_OK);
	} while(false);
	return Ok;
}


bool
CRenderCriticalSection::ChooseDepthFormat( D3DFORMAT in_RenderFormat, bool in_bStencil, D3DFORMAT& out_rDepthFormat )
{
	CLog::Print("    ChooseDepthFormat()\n");
	assert(m_pD3D8);
	assert(in_RenderFormat==D3DFMT_R5G6B5 || in_RenderFormat==D3DFMT_X8R8G8B8);
	if(in_RenderFormat==D3DFMT_R5G6B5)
	{
		CLog::Print("      16bit\n");
		if(in_bStencil)
		{
			CLog::Print("        no stencil format\n");
			return false;
		}
		if( CheckDepthFormat(D3DFMT_R5G6B5,D3DFMT_D16) )
		{
			CLog::Print("        D3DFMT_D16\n");
			out_rDepthFormat = D3DFMT_D16;
			return true;
		}
		else
		{
			CLog::Print("        no format found\n");
			return false;
		}
	}
	else
	{
		CLog::Print("      32bit\n");
		if(m_b16bitDepthOnly)
		{
			if(in_bStencil)
			{
				CLog::Print("        no stencil format\n");
				return false;
			}
			if( CheckDepthFormat(D3DFMT_X8R8G8B8,D3DFMT_D16) )
			{
				CLog::Print("        D3DFMT_D16\n");
				out_rDepthFormat = D3DFMT_D16;
				return true;
			}
			else
			{
				CLog::Print("        no format found\n");
				return false;
			}
		}
		else
		{
			if(in_bStencil)
			{
				CLog::Print("        stencil\n");
				bool Ok = true;
				do
				{
					if( CheckDepthFormat(D3DFMT_X8R8G8B8,D3DFMT_D24S8) )
					{
						CLog::Print("          D3DFMT_D24S8\n");
						out_rDepthFormat = D3DFMT_D24S8;
						break;
					}
					if( CheckDepthFormat(D3DFMT_X8R8G8B8,D3DFMT_D24X4S4) )
					{
						CLog::Print("          D3DFMT_D24X4S4\n");
						out_rDepthFormat = D3DFMT_D24X4S4;
						break;
					}
					CLog::Print("          no format found\n");
					Ok = false;
				} while(false);
				return Ok;
			}
			else
			{
				CLog::Print("        no stencil\n");
				bool Ok = true;
				do
				{
					if( CheckDepthFormat(D3DFMT_X8R8G8B8,D3DFMT_D32) )
					{
						CLog::Print("          D3DFMT_D32\n");
						out_rDepthFormat = D3DFMT_D32;
						break;
					}
					if( CheckDepthFormat(D3DFMT_X8R8G8B8,D3DFMT_D24X8) )
					{
						CLog::Print("          D3DFMT_D24X8\n");
						out_rDepthFormat = D3DFMT_D24X8;
						break;
					}
					CLog::Print("          no format found\n");
					Ok = false;
				} while(false);
				return Ok;
			}
		}
	}
}

bool
CRenderCriticalSection::CheckDepthFormat( D3DFORMAT in_RenderFormat, D3DFORMAT in_DepthFormat )
{
	assert(m_pD3D8);
	if( D3D_OK != m_pD3D8->CheckDeviceFormat(	D3DADAPTER_DEFAULT,
												D3DDEVTYPE_HAL,
												in_RenderFormat,
												D3DUSAGE_DEPTHSTENCIL,
												D3DRTYPE_SURFACE,
												in_DepthFormat) )
		return false;
	if( D3D_OK != m_pD3D8->CheckDepthStencilMatch(	D3DADAPTER_DEFAULT,
													D3DDEVTYPE_HAL,
													in_RenderFormat,
													in_RenderFormat,
													in_DepthFormat	) )
		return false;
	return true;
}

void
CRenderCriticalSection::ReleaseRenderObjects()
{
	if(m_pD3D8Device!=NULL)
	{
		ULONG Count = m_pD3D8Device->Release();
		assert(Count==0);
		m_pD3D8Device=NULL;
		CLog::Print("  m_pD3D8Device released\n");
	}
	if(m_pD3D8!=NULL)
	{
		ULONG Count = m_pD3D8->Release();
		assert(Count==0);
		m_pD3D8=NULL;
		CLog::Print("  m_pD3D8 released\n");
	}
}






















