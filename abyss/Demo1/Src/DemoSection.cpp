#include "../../MT_Core/Include/log.h"
#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/section_registrator.h"
/// Initialization 
#include <assert.h>

#define TEST_WINDOW_WIDTH			640
#define TEST_WINDOW_HEIGHT			480

#include "../Include/DemoSection_interface.h"
#include "../Include/DemoSection.h"
#include "../Include/CVertexLoader.h"

static CTSectionRegistrator<DemoSection_CLSID,DemoSection> g_DemoSection( SINGLE_INSTANCE_ONLY, STARTUP_SECTION );



#define QUERY_MT_INFO_MAGIC					1
#define LAUNCH_THREADS_MAGIC				2
#define LAUNCH_WINDOW_SECTION_MAGIC			3
#define CREATE_WINDOW_MAGIC					4
#define LAUNCH_RENDER_SECTION_MAGIC			5
#define LAUNCH_VFS_SECTION_MAGIC			6
#define LAUNCH_PRELOADER_SECTION_MAGIC    	7


#define TEST_WINDOW_WIDTH 640
#define TEST_WINDOW_HEIGHT 480

#define IMAGE_SRC "/tex.tga"
#define OBJECT_SRC "/object.txt"




DemoSection::DemoSection( long in_SectionID ) :
	CGameBaseSection(in_SectionID)
	,m_NFramesToRender(0)
	,m_LastTimeFrame(0)
	,m_NFrames(0)
	,m_Alpha(0.0f)
	,m_VBHandle(0)
	,m_PrimitiveType(0)
	,m_NVertices(0)
	,m_NPrimitives(0)
	,m_TextureHandle(0)
	,m_PSHandle(0)

{

	RegisterReaction( GetID((CVFSSection_ReadResponse*)NULL),CTConform<DemoSection,CVFSSection_ReadResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_MountResponse*)NULL),CTConform<DemoSection,CVFSSection_MountResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_OpenResponse*)NULL),CTConform<DemoSection,CVFSSection_OpenResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_ExistsResponse*)NULL),CTConform<DemoSection,CVFSSection_ExistsResponse>::Conform );
	RegisterReaction( GetID((CPreloaderSection_DecodeResponse*)NULL),CTConform<DemoSection,CPreloaderSection_DecodeResponse>::Conform );

	

	RegisterReaction( GetID((CRenderSection_NewFrame*)NULL),					CTConform<DemoSection,CRenderSection_NewFrame>::Conform );
	RegisterReaction( GetID((CRenderSection_CreateVertexBuffer_Response*)NULL),	CTConform<DemoSection,CRenderSection_CreateVertexBuffer_Response>::Conform );
	RegisterReaction( GetID((CRenderSection_CreateTexture_Response*)NULL),		CTConform<DemoSection,CRenderSection_CreateTexture_Response>::Conform );
	RegisterReaction( GetID((CRenderSection_CreatePixelShader_Response*)NULL),	CTConform<DemoSection,CRenderSection_CreatePixelShader_Response>::Conform );
	RegisterReaction( GetID((DemoSection_Render*)NULL),						    CTConform<DemoSection,DemoSection_Render>::Conform );


}



DemoSection::~DemoSection()
{
	
}


void DemoSection::StartTheGame()
{
		CTCommandSender<CVFSSection_MountRequest>::SendCommand(GetVFSSectionID() , CVFSSection_MountRequest("."));
}

void DemoSection::Reaction( long in_SrcSectionID, const CVFSSection_ExistsResponse& in_rRead )
{
	if (in_rRead.m_Exists) 
	{
		CLog::Println("%s", in_rRead.m_Url.data());
	}
}

void DemoSection::Reaction( long in_SrcSectionID, const CVFSSection_ReadResponse& in_rRead )
{
	if (in_rRead.m_Handle == m_ImageHandle) 
	{
		assert(in_rRead.m_Size > 0 );
		CPreloaderSection_DecodeRequest req;
		req.m_Data.resize(in_rRead.m_Size);
		memcpy((char*)&(req.m_Data[0]), (char*)&(in_rRead.m_pData[0]), in_rRead.m_Size);
		CTCommandSender<CPreloaderSection_DecodeRequest>::SendCommand(GetPreloaderSectionID() , req);
	}
	else
	if (in_rRead.m_Handle == m_ObjectHandle ) 
	{
		CVertexLoader loader;
		if (loader.Load(in_rRead)) 
		{

			CRenderSection_CreateVertexBuffer_Request Cmd(loader.GetVBFormat(), false );
			Cmd.m_Data = loader.GetData();
			m_PrimitiveType = loader.GetPrimitiveType();
			m_NVertices = loader.GetNVertices();
			m_NPrimitives = loader.GetNPrimitives();

			CTCommandSender<CRenderSection_CreateVertexBuffer_Request>::SendCommand(
				GetRenderSectionID(),
				Cmd
			);
		}
		else
		{
			// some error here please ~
			assert(0);
		}

	}
}

void DemoSection::Reaction( long in_SrcSectionID, const CPreloaderSection_DecodeResponse& response )
{

	CLog::Print("DemoSection::Reaction( const CPreloaderSection_DecodeResponse& )\n");
	const unsigned long* pFirstPixel = (const unsigned long*)(&(response.m_ARGBData[0]));
	CLog::Print(" FirstPixel = %08x\n",*pFirstPixel);

	//
	CRenderSection_CreateTexture_Request Cmd(response.m_Width,response.m_Height,true);
	Cmd.m_Data = response.m_ARGBData;

	CTCommandSender<CRenderSection_CreateTexture_Request>::SendCommand(
		GetRenderSectionID(),
		Cmd
	);

}



void DemoSection::Reaction( long in_SrcSectionID, const CVFSSection_MountResponse& in_rRead )
{

	CVFSSection_OpenRequest req1;
	req1.m_Url = OBJECT_SRC;
	CTCommandSender<CVFSSection_OpenRequest>::SendCommand(GetVFSSectionID() , req1);
	CVFSSection_ExistsRequest req2;
	req2.m_Url = OBJECT_SRC;
	CTCommandSender<CVFSSection_ExistsRequest>::SendCommand(GetVFSSectionID() , req2);
	CVFSSection_ExistsRequest req3;
	req3.m_Url = "/doesntexist";
	CTCommandSender<CVFSSection_ExistsRequest>::SendCommand(GetVFSSectionID() , req3);
}

void DemoSection::Reaction( long in_SrcSectionID, const CVFSSection_OpenResponse& in_rRead )
{
	assert(in_rRead.m_Handle  > 0);
	if (in_rRead.m_Url == IMAGE_SRC ) 
	{
		m_ImageHandle = in_rRead.m_Handle;
		CPreloaderSection_FileDecodeRequest req;
		req.m_Handle = m_ImageHandle;
		req.m_Type = TYPE_TGA;
		req.m_VFSSection = GetVFSSectionID();
		CTCommandSender<CPreloaderSection_FileDecodeRequest>::SendCommand(GetPreloaderSectionID() , req);
	
	}
	else
	if (in_rRead.m_Url == OBJECT_SRC ) 
	{
		m_ObjectHandle = in_rRead.m_Handle;
		CVFSSection_ReadRequest req;
		req.m_Handle  = in_rRead.m_Handle;
		req.m_Offset = 0;
		req.m_Size -1;
		CTCommandSender<CVFSSection_ReadRequest>::SendCommand(GetVFSSectionID() , req);
	}
}





void
DemoSection::Received_MTInfo(const CCommand_MTInfo& in_rInfo)
{
	CLog::Print("CTest6Section::Received_MTInfo()\n");
	CLog::Print("  NProcessors = %ld\n",in_rInfo.m_NProcessors);
	m_NThreadsToRun = 2*in_rInfo.m_NProcessors;
	CLog::Print("  NThreadsToRun = %ld\n",m_NThreadsToRun);
}

long
DemoSection::GetNThreadsToRun()
{
	assert(m_NThreadsToRun>0);
	return m_NThreadsToRun;
}

bool
DemoSection::GetNeedVFS()
{
//	return true;
	return true;
}



void
DemoSection::GetGameWindowParams( CWindowParam& out_rParams )
{
	CLog::Print("CTest6Section::GetGameWindowParams()\n");
	out_rParams.m_Style = WS_CONSTANT;
	//out_rParams.m_Style = WS_RESIZEABLE;
	//out_rParams.m_Style = WS_MINMAX;
	out_rParams.m_ClientWidth = TEST_WINDOW_WIDTH;
	out_rParams.m_ClientHeight = TEST_WINDOW_HEIGHT;
	out_rParams.m_Caption = "Render test.";
}

void
DemoSection::Received_3DInfo( const CRenderSection_3DEnvironmentInfo& in_rInfo )
{
	CLog::Print("CTest6Section::Received_3DInfo()\n");
	m_3DInfo = in_rInfo;

}

void
DemoSection::GetRenderInitParams( CRenderSection_InitRender& out_rParams )
{
	CLog::Print("CTest6Section::GetRenderInitParams()\n");
	out_rParams.m_DX = 640;
	out_rParams.m_DY = 480;
	out_rParams.m_bWindowed = true;
	out_rParams.m_FullscreenFormat = RDMF_UNKNOWN;
	out_rParams.m_bDepthBufferRequired = true;
	out_rParams.m_bStencilBufferRequired = false;
	out_rParams.m_FullscreenRefreshRate = 60;
	out_rParams.m_bVSync = false;
}

void
DemoSection::Received_InitRenderResult( INIT_RENDER_RESULT in_bResult )
{
	CLog::Print("CTest6Section::Received_InitRenderResult()\n");
	

}

void DemoSection::Received_SectionLaunchResult(const CCommand_LaunchSectionResult& res)
{

}
/// 


void
DemoSection::Received_WindowClosed()
{
	CLog::Print("CTest6Section::Received_WindowClosed()\n");
}




void
DemoSection::Reaction( long in_SrcSectionID, const CRenderSection_CreateVertexBuffer_Response& in_rCmd )
{

	CLog::Print("DemoSection::Reaction( const CRenderSection_CreateVertexBuffer_Response& )\n");
	CLog::Print("  VB handle = %lu\n",in_rCmd.m_VBHandle);
	assert( in_rCmd.m_VBHandle > 0 );
	m_VBHandle = in_rCmd.m_VBHandle;

	CVFSSection_OpenRequest req;
	req.m_Url = IMAGE_SRC;
	CTCommandSender<CVFSSection_OpenRequest>::SendCommand(GetVFSSectionID() , req);


}

void
DemoSection::Reaction( long in_SrcSectionID, const CRenderSection_CreateTexture_Response& in_rCmd )
{
	CLog::Print("DemoSection::Reaction( const CRenderSection_CreateTexture_Response& )\n");
	m_TextureHandle = in_rCmd.m_TexHandle;
	CLog::Print("  m_TextureHandle = %lu\n",m_TextureHandle);
	//
	CRenderSection_CreatePixelShader_Request Cmd;
	CPixelShader& rShaderDesc = Cmd.m_ShaderDescription;
	rShaderDesc.m_Passes.resize(1);
	{
		// pass 0
		CPixelShaderPass& rPass = rShaderDesc.m_Passes[0];
		rPass.m_CullMode = RCM_CCW;
		rPass.m_SrcBlend = RBM_ONE;
		rPass.m_DstBlend = RBM_ZERO;
		rPass.m_bLightingEnabled = true;
		rPass.m_TFactor = 0;
		rPass.m_bAlphaTest = false;
		rPass.m_bDepthWrite = true;
		rPass.m_bDepthEnabled = true;
		rPass.m_ZCmpMode = RCMP_LESSEQUAL;
		{
			// pass 0, textures
			rPass.m_Textures.resize(1);
			rPass.m_Textures[0] = m_TextureHandle;
		}
		rPass.m_Stages.resize(1);
		{
			// pass 0, stage 0
			CPixelShaderPassStage& rStage = rPass.m_Stages[0];
			//
			rStage.m_AlphaArg1 = RTSA_DIFFUSE;
			rStage.m_AlphaArg2 = RTSA_CURRENT;
			//rStage.m_AlphaOp = RTSO_SELECTARG1;
			rStage.m_AlphaOp = RTSO_DISABLE;
			//
			rStage.m_ColorArg1 = RTSA_TEXTURE;
			rStage.m_ColorArg2 = RTSA_CURRENT;//RTSA_CURRENT;
			//rStage.m_ColorOp = RTSO_MODULATE;
			rStage.m_ColorOp = RTSO_SELECTARG1;
			//rStage.m_ColorOp = RTSO_SELECTARG2;
			//
			rStage.m_TextureAddressModeU = RTAM_WRAP;
			rStage.m_TextureAddressModeV = RTAM_WRAP;
			rStage.m_TextureIndex = 0;
			rStage.m_TextureMagFilter = RTFM_LINEAR;
			rStage.m_TextureMinFilter = RTFM_LINEAR;
			rStage.m_TextureMipFilter = RTFM_NONE;
		}
	}
	CTCommandSender<CRenderSection_CreatePixelShader_Request>::SendCommand(
		GetRenderSectionID(),
		Cmd
	);
}

void
DemoSection::Reaction( long in_SrcSectionID, const CRenderSection_CreatePixelShader_Response& in_rCmd )
{
	CLog::Print("DemoSection::Reaction( const CRenderSection_CreatePixelShader_Response& )\n");
	m_PSHandle = in_rCmd.m_ShaderHandle;
	CLog::Print("  m_PSHandle = %lu\n",m_PSHandle);
	// set lights
	CTCommandSender<CRenderSection_SetAmbient>::SendCommand(
		GetRenderSectionID(),
		CRenderSection_SetAmbient( 0x404040 )
	);
	CTCommandSender<CRenderSection_SetDirectionalLight>::SendCommand(
		GetRenderSectionID(),
		CRenderSection_SetDirectionalLight( CVector(-1.0f,-1.0f,-1.0f), 1.0f, 1.0f, 1.0f )
	);
	// set object positions
	m_M0.ConstructScaling( CVector(4,4,4) );
	m_M1.ConstructRotationY( 0 );
	m_M1 *= CMatrix().ConstructTranslation( CVector(8,0,0) );
	m_M2.ConstructRotationY( CONST_PI_2 );
	m_M2 *= CMatrix().ConstructTranslation( CVector(0,0,8) );
	m_M3.ConstructRotationY( CONST_PI_2*2 );
	m_M3 *= CMatrix().ConstructTranslation( CVector(-8,0,0) );
	m_M4.ConstructRotationY( CONST_PI_2*3 );
	m_M4 *= CMatrix().ConstructTranslation( CVector(0,0,-8) );
	// start rendering
	m_NFramesToRender = 2;
	RenderFrame();
	CTCommandSender<DemoSection_Render>::SendCommand(
		GetThisID(),
		DemoSection_Render()
	);
}




////////////////

void
DemoSection::Reaction( long in_SrcSectionID, const CRenderSection_NewFrame& in_rCmd )
{
//CLog::Print("  CTest6Section::Reaction( const CRenderSection_NewFrame& )\n");
	++m_NFramesToRender;
	++m_NFrames;
	if(GetHWND()!=NULL)
	{
		unsigned long ThisFrameTime = GetTickCount();
		if( (ThisFrameTime-m_LastTimeFrame)>=1000 )
		{
			float FPS = 1000.0f*(float(m_NFrames))/(float(ThisFrameTime-m_LastTimeFrame));
			char Buffer[256];
			sprintf(Buffer,"Render test. FPS=%1.0f",FPS);
			CTCommandSender<CWindowSection_WindowName>::SendCommand(
				GetWindowSectionID(),
				CWindowSection_WindowName(std::string(Buffer))
			);
			//
			m_LastTimeFrame = ThisFrameTime;
			m_NFrames=0;
		}
		RenderFrame();
	}
}

void
DemoSection::Reaction( long in_SrcSectionID, const DemoSection_Render& in_rCmd )
{
	if(GetHWND()!=NULL)
	{
		RenderFrame();
	}
}

void
DemoSection::RenderFrame()
{
	assert(GetHWND()!=NULL);
	assert(GetRenderSectionID()!=SECTION_ID_ERROR);
	if(m_NFramesToRender>0)
	{
		//CLog::Print("  m_NFramesToRender=%lu\n",m_NFramesToRender);
		{
			CVector CamOrg( 10.0f*sin(m_Alpha), 0.0f, 10.0f*cos(m_Alpha) );
			CVector CamDir( CVector(0,0,0) - CamOrg );
			CamDir.Normalize();
			CVector Tmp( CVector(0,10,0) - CamOrg );
			CVector CamRight( CamDir*Tmp );
			CamRight.Normalize();
			CTCommandSender<CRenderSection_SetCamera>::SendCommand(
				GetRenderSectionID(),
				CRenderSection_SetCamera(	CamOrg,CamDir,CamRight,
											CONST_PI_2, 0.75f*CONST_PI_2,
											0.01f, 100.0f)
			);
			//CTCommandSender<CRenderSection_SetCamera>::SendCommand(
			//	GetRenderSectionID(),
			//	CRenderSection_SetCamera(
			//		CVector(0,15,0),
			//		CVector(0,-1,0),
			//		CVector(0,0,1),
			//		CONST_PI_2, 0.75f*CONST_PI_2,
			//		0.01f, 100.0f
			//	)
			//);
			m_Alpha += 0.001f;
			if(m_Alpha>CONST_2PI)
				m_Alpha -= CONST_2PI;
		}
		{
//			CTCommandSender<CRenderSection_RenderVB>::SendCommand(
//				GetRenderSectionID(),
//				CRenderSection_RenderVB(m_M0,m_VBHandle,0,m_PrimitiveType,m_NPrimitives)
//			);
			CTCommandSender<CRenderSection_RenderVB>::SendCommand(
				GetRenderSectionID(),
				CRenderSection_RenderVB(m_M0,m_VBHandle,m_PSHandle,0,m_PrimitiveType,m_NPrimitives)
			);
			//CTCommandSender<CRenderSection_RenderVB>::SendCommand(
			//	GetRenderSectionID(),
			//	CRenderSection_RenderVB(m_M1,m_VBHandle,0,m_PrimitiveType,m_NPrimitives)
			//);
			//CTCommandSender<CRenderSection_RenderVB>::SendCommand(
			//	GetRenderSectionID(),
			//	CRenderSection_RenderVB(m_M2,m_VBHandle,0,m_PrimitiveType,m_NPrimitives)
			//);
			//CTCommandSender<CRenderSection_RenderVB>::SendCommand(
			//	GetRenderSectionID(),
			//	CRenderSection_RenderVB(m_M3,m_VBHandle,0,m_PrimitiveType,m_NPrimitives)
			//);
			//CTCommandSender<CRenderSection_RenderVB>::SendCommand(
			//	GetRenderSectionID(),
			//	CRenderSection_RenderVB(m_M4,m_VBHandle,0,m_PrimitiveType,m_NPrimitives)
			//);
		}
		CTCommandSender<CRenderSection_PresentAndClear>::SendCommand(
			GetRenderSectionID(),
			CRenderSection_PresentAndClear( 0x808080 )
		);
		--m_NFramesToRender;
	}
}
