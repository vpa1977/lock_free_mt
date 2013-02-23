#include "../Include/sun_section.h"
#include "../../MT_Core/Include/section_registrator.h"
#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/log.h"
#include "../../Render_DX8/Include/algebra.h"
#include <list>

#include "../../MT_Core/Include/shared_variables.h"

static CTSectionRegistrator<CSunSection_CLSID,CSunSection>
g_StarsSectionRegistrator(SINGLE_INSTANCE_ONLY);

#define DATA_LOCATION "/data/"

#define VB_UPDATE_FREQUENCY 25 // frames per second
#define TIMER_PERIOD_MICROSECONDS (1000000/VB_UPDATE_FREQUENCY)

unsigned long
AgeToDiffuse( float in_Age )
{
	if(in_Age<=0.0f || in_Age>=1.0f)
		return 0xff000000;
	else
	{
		unsigned long C;
		if(in_Age<=0.5f)
		{
			C = in_Age*2.0f*255.0f;
		}
		else
		{
			C = (1.0f-in_Age)*2.0f*255.0f;
		}
		C = (C>255) ? 255:C;
		return 0xff000000 | (C<<16) | (C<<8) | C;
	}
}

CSunSection::CSunSection( long in_SectionID ) :
	CSection( in_SectionID )
	,m_RenderSectionID(SECTION_ID_ERROR)
	,m_VfsSectionID(SECTION_ID_ERROR)
	,m_MainSectionID(SECTION_ID_ERROR)
	,m_PreloaderSectionID(SECTION_ID_ERROR)
	,m_ConfigUrl("")
	,m_TextureUrl("")
	,m_ConfigHandle(0)
	,m_TextureHandle(0)
	,m_VBHandle(0)
	,m_bVBCreateRequestSent(false)
	,m_PSHandle(0)
	,m_THandle(0)
	,m_SunRadius(0)
	,m_NParticles(0)
	,m_ParticleRadiusMin(0)
	,m_ParticleRadiusMax(0)
	,m_ParticleLifeMin(0)
	,m_ParticleLifeMax(0)
	,m_bHaveCamera(false)
{
	RegisterReaction( GetID((CCommand_SectionStartup*)NULL),	CTConform<CSunSection,CCommand_SectionStartup>::Conform );
	// system
	RegisterReaction( GetID((CCommand_SetTimer_Response*)NULL),	CTConform<CSunSection,CCommand_SetTimer_Response>::Conform );
	RegisterReaction( GetID((CCommand_Timer_Event*)NULL),	CTConform<CSunSection,CCommand_Timer_Event>::Conform );
	// demo section
	RegisterReaction( GetID((CDemoSection_WorkerInit_Request*)NULL),	CTConform<CSunSection,CDemoSection_WorkerInit_Request>::Conform );
	RegisterReaction( GetID((CDemoSection_WorkerCameraInform*)NULL),	CTConform<CSunSection,CDemoSection_WorkerCameraInform>::Conform );
	RegisterReaction( GetID((CDemoSection_WorkerShutdown_Request*)NULL),	CTConform<CSunSection,CDemoSection_WorkerShutdown_Request>::Conform );
	// preloader
	RegisterReaction( GetID((CPreloaderSection_DecodeError*)NULL),	CTConform<CSunSection,CPreloaderSection_DecodeError>::Conform );
	RegisterReaction( GetID((CPreloaderSection_DecodeResponse*)NULL),	CTConform<CSunSection,CPreloaderSection_DecodeResponse>::Conform );
	// vfs
	RegisterReaction( GetID((CVFSSection_OpenResponse*)NULL),	CTConform<CSunSection,CVFSSection_OpenResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_Error<CVFSSection_OpenRequest>*)NULL),	CTConform<CSunSection,CVFSSection_Error<CVFSSection_OpenRequest> >::Conform );
	RegisterReaction( GetID((CVFSSection_ReadResponse*)NULL),	CTConform<CSunSection,CVFSSection_ReadResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_Error<CVFSSection_ReadRequest>*)NULL),	CTConform<CSunSection,CVFSSection_Error<CVFSSection_ReadRequest> >::Conform );
	RegisterReaction( GetID((CVFSSection_CloseResponse*)NULL),	CTConform<CSunSection,CVFSSection_CloseResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_Error<CVFSSection_CloseRequest>*)NULL),	CTConform<CSunSection,CVFSSection_Error<CVFSSection_CloseRequest> >::Conform );
	// render
	RegisterReaction( GetID((CRenderSection_CreateVertexBuffer_Response*)NULL),	CTConform<CSunSection,CRenderSection_CreateVertexBuffer_Response>::Conform );
	RegisterReaction( GetID((CRenderSection_UpdateVertexBuffer_Response*)NULL),	CTConform<CSunSection,CRenderSection_UpdateVertexBuffer_Response>::Conform );
	RegisterReaction( GetID((CRenderSection_CreateTexture_Response*)NULL),	CTConform<CSunSection,CRenderSection_CreateTexture_Response>::Conform );
	RegisterReaction( GetID((CRenderSection_CreatePixelShader_Response*)NULL),	CTConform<CSunSection,CRenderSection_CreatePixelShader_Response>::Conform );
}

CSunSection::~CSunSection()
{
}

void
CSunSection::Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )
{
	CLog::Print("CSunSection::Reaction( const CCommand_SectionStartup& )\n");
	CLog::Print("  sectionID = %ld\n",GetThisID());
	//{
	//	Store<int> qq( new int(777) );
	//	Handle<int> H( qq.GetHandle() );
	//	const int* rInt = H;
	//	qq.ResetObject( new int(666) );
	//}
}

void
CSunSection::Reaction( long in_SrcSectionID, const CCommand_SetTimer_Response& in_rResponse )
{
	assert( in_rResponse.m_Result==CCommand_SetTimer_Response::SETTIMER_OK );
	assert( in_rResponse.m_MagicID==666 );
}

void
CSunSection::Reaction( long in_SrcSectionID, const CCommand_Timer_Event& in_rEvent )
{
//	CLog::Print("CSunSection::Reaction( const CCommand_Timer_Event& )\n");
	assert( in_rEvent.m_MagicID == 666 );
	// update particles info
	for( size_t i=0; i<m_NParticles; ++i )
	{
		m_Particles[i].m_Age += m_Particles[i].m_DeltaAge;
		if(m_Particles[i].m_Age>1.0f)
		{
			GenerateOneParticle( m_Particles[i], false );
		}
	}
	// fill new VB data
	assert(0!=m_VBHandle);
	CRenderSection_UpdateVertexBuffer_Request req;
	req.m_VBHandle = m_VBHandle;
	size_t OneVertexSize = 3*sizeof(float) + sizeof(unsigned long) + 2*sizeof(float);
	req.m_Data.resize( 6*OneVertexSize*m_NParticles );
	unsigned char* pData = &(req.m_Data[0]);
	unsigned char* pEnd = pData+6*OneVertexSize*m_NParticles;
	for( size_t i=0; i<m_NParticles; ++i )
	{
		assert( pData+6*OneVertexSize <= pEnd );
		unsigned long Diffuse = AgeToDiffuse( m_Particles[i].m_Age );
		CVector Up		= m_Particles[i].m_Size*( m_CameraUp*m_Particles[i].m_Cos    - m_CameraRight*m_Particles[i].m_Sin );
		CVector Right	= m_Particles[i].m_Size*( m_CameraRight*m_Particles[i].m_Cos + m_CameraUp*m_Particles[i].m_Sin    );
		// 0
		*((CVector*)pData) = m_Particles[i].m_Org + Up;
		pData += 3*sizeof(float);
		*((unsigned long*)pData) = Diffuse;
		pData += sizeof(unsigned long);
		*((float*)pData) = 1.0f;
		pData += sizeof(float);
		*((float*)pData) = 0.0f;
		pData += sizeof(float);
		// 1
		*((CVector*)pData) = m_Particles[i].m_Org + Right;
		pData += 3*sizeof(float);
		*((unsigned long*)pData) = Diffuse;
		pData += sizeof(unsigned long);
		*((float*)pData) = 1.0f;
		pData += sizeof(float);
		*((float*)pData) = 1.0f;
		pData += sizeof(float);
		// 2
		*((CVector*)pData) = m_Particles[i].m_Org - Up;
		pData += 3*sizeof(float);
		*((unsigned long*)pData) = Diffuse;
		pData += sizeof(unsigned long);
		*((float*)pData) = 0.0f;
		pData += sizeof(float);
		*((float*)pData) = 1.0f;
		pData += sizeof(float);
		// 3
		*((CVector*)pData) = m_Particles[i].m_Org - Up;
		pData += 3*sizeof(float);
		*((unsigned long*)pData) = Diffuse;
		pData += sizeof(unsigned long);
		*((float*)pData) = 0.0f;
		pData += sizeof(float);
		*((float*)pData) = 1.0f;
		pData += sizeof(float);
		// 4
		*((CVector*)pData) = m_Particles[i].m_Org - Right;
		pData += 3*sizeof(float);
		*((unsigned long*)pData) = Diffuse;
		pData += sizeof(unsigned long);
		*((float*)pData) = 0.0f;
		pData += sizeof(float);
		*((float*)pData) = 0.0f;
		pData += sizeof(float);
		// 5
		*((CVector*)pData) = m_Particles[i].m_Org + Up;
		pData += 3*sizeof(float);
		*((unsigned long*)pData) = Diffuse;
		pData += sizeof(unsigned long);
		*((float*)pData) = 1.0f;
		pData += sizeof(float);
		*((float*)pData) = 0.0f;
		pData += sizeof(float);
	}
	CTCommandSender<CRenderSection_UpdateVertexBuffer_Request>::SendCommand( m_RenderSectionID, req );
}

void
CSunSection::Reaction( long in_SrcSectionID, const CDemoSection_WorkerInit_Request& in_rWorkerInit_Request )
{
	CLog::Print("CSunSection::Reaction( const CDemoSection_WorkerInit_Request& )\n");
	m_VfsSectionID = in_rWorkerInit_Request.m_VFSSectionID;
	assert( m_VfsSectionID != SECTION_ID_ERROR );
	m_RenderSectionID = in_rWorkerInit_Request.m_RenderSectionID;
	assert( m_RenderSectionID != SECTION_ID_ERROR );
	m_MainSectionID = in_SrcSectionID;
	assert( m_MainSectionID != SECTION_ID_ERROR );
	m_PreloaderSectionID = in_rWorkerInit_Request.m_PreloaderSectionID;
	assert( m_PreloaderSectionID != SECTION_ID_ERROR );

	CVFSSection_OpenRequest openRequest;
	openRequest.m_Url = DATA_LOCATION;
	openRequest.m_Url += in_rWorkerInit_Request.m_ConfigFileForThisSection;

	m_ConfigUrl = openRequest.m_Url;
	CLog::Print("  config='%s'\n",m_ConfigUrl.c_str());

	CLog::Print("  sending Open config\n");
	CTCommandSender<CVFSSection_OpenRequest>::SendCommand(
		m_VfsSectionID
		,openRequest
	);
}

void
CSunSection::Reaction( long in_SrcSectionID, const CDemoSection_WorkerCameraInform& in_rCmd )
{
//	CLog::Print("CSunSection::Reaction( const CDemoSection_WorkerCameraInform& )\n");
	m_CameraRight = in_rCmd.m_CameraRight;
	m_CameraRight.Normalize();
	m_CameraUp = in_rCmd.m_CameraUp;
	m_CameraUp.Normalize();
	m_bHaveCamera = true;
	CTCommandSender<CDemoSection_WorkerCameraInform_Response>::SendCommand(
		in_SrcSectionID
		,CDemoSection_WorkerCameraInform_Response()
	);
	CheckForInitializationCompletion();
}

void
CSunSection::Reaction( long in_SrcSectionID, const CDemoSection_WorkerShutdown_Request& )
{
	CLog::Print("CSunSection::Reaction( const CDemoSection_WorkerShutdown_Request& )\n");
	CTCommandSender<CDemoSection_WorkerShutdown_Response>::SendCommand(in_SrcSectionID,CDemoSection_WorkerShutdown_Response());
	Quit();
}

void
CSunSection::Reaction( long in_SrcSectionID, const CPreloaderSection_DecodeError& in_rDecodeError )
{
	CLog::Print("CSunSection::Reaction( const CPreloaderSection_DecodeError& )\n");
	CLog::Print("  Image decoding error %s\n", in_rDecodeError.m_Message);
	CDemoSection_WorkerInit_Response resp;
	resp.m_bSuccess = false;
	CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID, resp);
}

void
CSunSection::Reaction( long in_SrcSectionID, const CPreloaderSection_DecodeResponse& in_rDecodeResponse )
{
	CLog::Print("CSunSection::Reaction( const CPreloaderSection_DecodeResponse& )\n");
	CRenderSection_CreateTexture_Request Cmd(in_rDecodeResponse.m_Width,in_rDecodeResponse.m_Height,true);
	Cmd.m_Data = in_rDecodeResponse.m_ARGBData;
	CLog::Print("  sending CreateTexture\n");
	CTCommandSender<CRenderSection_CreateTexture_Request>::SendCommand(
		m_RenderSectionID,
		Cmd
	);
	CVFSSection_CloseRequest cmd;
	cmd.m_Handle = 	in_rDecodeResponse.m_Handle;
	CTCommandSender<CVFSSection_CloseRequest>::SendCommand(m_VfsSectionID, cmd);
}

void
CSunSection::Reaction( long in_SrcSectionID, const CVFSSection_OpenResponse& in_rOpenResponse )
{
	CLog::Print("CSunSection::Reaction( const CVFSSection_OpenResponse& )\n");
	CLog::Print("  url='%s'\n",in_rOpenResponse.m_Url.c_str());
	if( in_rOpenResponse.m_Url == m_ConfigUrl ) 
	{
		CLog::Print("  Config opened\n");
		assert( 0 != in_rOpenResponse.m_Handle );
		m_ConfigHandle = in_rOpenResponse.m_Handle;
		// read config file
		CVFSSection_ReadRequest req;
		req.m_Handle = m_ConfigHandle;
		req.m_Offset = 0;
		req.m_Size = -1;
		CLog::Print("  sending Read config\n");
		CTCommandSender<CVFSSection_ReadRequest>::SendCommand( m_VfsSectionID , req );
	}
	else if( in_rOpenResponse.m_Url == m_TextureUrl )
	{
		CLog::Print("  Texture file opened\n");
		assert( 0 != in_rOpenResponse.m_Handle );
		m_TextureHandle = in_rOpenResponse.m_Handle;
		// decode texture
		CPreloaderSection_FileDecodeRequest req;
		req.m_Handle = m_TextureHandle;
		req.m_Type = TYPE_JPG;
		req.m_VFSSection = m_VfsSectionID;
		CLog::Print("  sending Decode texture file\n");
		CTCommandSender<CPreloaderSection_FileDecodeRequest>::SendCommand(m_PreloaderSectionID, req);
	}
	else
	{
		assert(false);
	}
}

void
CSunSection::Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_OpenRequest>& in_rOpenError )
{
	CLog::Print("CSunSection::Reaction( const CVFSSection_Error<CVFSSection_OpenRequest>& )\n");
	CLog::Print("  VFS error %d\n", in_rOpenError.m_ErrorCode );
	CDemoSection_WorkerInit_Response resp;
	resp.m_bSuccess = false;
	CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID, resp);
}

void
CSunSection::Reaction( long in_SrcSectionID, const CVFSSection_ReadResponse& in_rReadResponse )
{
	CLog::Print("CSunSection::Reaction( const CVFSSection_ReadResponse& )\n");
	CTCommandSender<CVFSSection_CloseRequest>::SendCommand(
		m_VfsSectionID
		,CVFSSection_CloseRequest(in_rReadResponse.m_Handle)
	);
	if( in_rReadResponse.m_Handle == m_ConfigHandle ) 
	{
		CLog::Print("  Config read\n");
		assert(in_rReadResponse.m_Offset==0);
		ParseConfig( in_rReadResponse.m_pData );
		// open sun texture
		CLog::Print("  sending Open texture file\n");
		CTCommandSender<CVFSSection_OpenRequest>::SendCommand(
			m_VfsSectionID
			,CVFSSection_OpenRequest(m_TextureUrl)
		);
	}
	else
	{
		assert(false);
	}
}

void
CSunSection::Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_ReadRequest>& in_rReadError )
{
	CLog::Print("CSunSection::Reaction( const CVFSSection_Error<CVFSSection_ReadRequest>& )\n");
	CLog::Print("  VFS error %d\n" , in_rReadError.m_ErrorCode );
	CDemoSection_WorkerInit_Response resp;
	resp.m_bSuccess = false;
	CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID, resp);
}

void
CSunSection::Reaction( long in_SrcSectionID, const CVFSSection_CloseResponse& in_rCloseResponse )
{
	CLog::Print("CSunSection::Reaction( const CVFSSection_CloseResponse& )\n");
	if(m_ConfigHandle==in_rCloseResponse.m_Handle)
	{
		m_ConfigHandle = 0;
	}
	else if(m_TextureHandle==in_rCloseResponse.m_Handle)
	{
		m_TextureHandle = 0;
	}
	else
	{
		assert(false);
	}
}

void
CSunSection::Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_CloseRequest>& in_rCloseError )
{
	CLog::Print("CSunSection::Reaction( const CVFSSection_Error<CVFSSection_CloseRequest>& )\n");
	CLog::Print("  VFS error %d\n" , in_rCloseError.m_ErrorCode );
	CDemoSection_WorkerInit_Response resp;
	resp.m_bSuccess = false;
	CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID, resp);
}

void
CSunSection::Reaction( long in_SrcSectionID, const CRenderSection_CreateVertexBuffer_Response& in_rResponse )
{
	CLog::Print("==========================================================================\n");
	CLog::Print("CSunSection::Reaction( const CRenderSection_CreateVertexBuffer_Response& )\n");
	CLog::Print("==========================================================================\n");
	if(0==in_rResponse.m_VBHandle)
	{
		CLog::Print("  VB was not created\n");
		CDemoSection_WorkerInit_Response resp;
		resp.m_bSuccess = false;
		CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID, resp);
	}
	else
	{
		m_VBHandle = in_rResponse.m_VBHandle;
		// send response to main section
		CDemoSection_WorkerInit_Response resp;
		resp.m_bSuccess = true;
		resp.m_Renderables.resize(1);
		resp.m_Renderables[0].m_VBHandle = m_VBHandle;
		resp.m_Renderables[0].m_PSHandle = m_PSHandle;
		resp.m_Renderables[0].m_PrimitiveType = PRIM_TRIANGLE_LIST;
		resp.m_Renderables[0].m_PrimitiveCount = 2*m_Particles.size();
		resp.m_Renderables[0].m_Type = OT_TRANSLUCENT;
		CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID, resp);
		// start timer
		CTCommandSender<CCommand_SetTimer_Request>::SendCommand( SECTION_ID_SYSTEM, CCommand_SetTimer_Request(666,40000) );
	}
}

void
CSunSection::Reaction( long in_SrcSectionID, const CRenderSection_UpdateVertexBuffer_Response& in_rResponse )
{
//	CLog::Print("CSunSection::  setting timer\n");
	assert( in_rResponse.m_bSuccess );
	// start timer again
	CTCommandSender<CCommand_SetTimer_Request>::SendCommand( SECTION_ID_SYSTEM, CCommand_SetTimer_Request(666,40000) );
}

void
CSunSection::Reaction( long in_SrcSectionID, const CRenderSection_CreateTexture_Response& in_rResponse )
{
	CLog::Print("CSunSection::Reaction( const CRenderSection_CreateTexture_Response& )\n");
	if(0==in_rResponse.m_TexHandle)
	{
		CLog::Print("  Texture was not created\n");
		CDemoSection_WorkerInit_Response resp;
		resp.m_bSuccess = false;
		CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID, resp);
	}
	else
	{
		m_THandle = in_rResponse.m_TexHandle;
		//
		CRenderSection_CreatePixelShader_Request Cmd;
		CPixelShader& rShaderDesc = Cmd.m_ShaderDescription;
		rShaderDesc.m_Passes.resize(1);
		{
			// pass 0
			CPixelShaderPass& rPass = rShaderDesc.m_Passes[0];
			rPass.m_CullMode = RCM_CCW;
			rPass.m_SrcBlend = RBM_ONE;
			rPass.m_DstBlend = RBM_ONE;
			rPass.m_bLightingEnabled = false;
			rPass.m_TFactor = 0;
			rPass.m_bAlphaTest = false;
			rPass.m_bDepthWrite = false;
			rPass.m_bDepthEnabled = true;
			rPass.m_ZCmpMode = RCMP_LESSEQUAL;
			{
				// pass 0, textures
				rPass.m_Textures.resize(1);
				rPass.m_Textures[0] = m_THandle;
			}
			rPass.m_Stages.resize(1);
			{
				// pass 0, stage 0
				CPixelShaderPassStage& rStage = rPass.m_Stages[0];
				//
				rStage.m_AlphaArg1 = RTSA_DIFFUSE;
				rStage.m_AlphaArg2 = RTSA_CURRENT;
				rStage.m_AlphaOp = RTSO_DISABLE;
				//
				rStage.m_ColorArg1 = RTSA_TEXTURE;
				rStage.m_ColorArg2 = RTSA_DIFFUSE;
				rStage.m_ColorOp = RTSO_MODULATE;
				//rStage.m_ColorOp = RTSO_SELECTARG2;
				//
				rStage.m_TextureAddressModeU = RTAM_CLAMP;
				rStage.m_TextureAddressModeV = RTAM_CLAMP;
				rStage.m_TextureIndex = 0;
				rStage.m_TextureMagFilter = RTFM_LINEAR;
				rStage.m_TextureMinFilter = RTFM_LINEAR;
				rStage.m_TextureMipFilter = RTFM_NONE;
			}
		}
		CLog::Print("  sending Create PS\n");
		CTCommandSender<CRenderSection_CreatePixelShader_Request>::SendCommand(
			m_RenderSectionID,
			Cmd
		);
	}
}

void
CSunSection::Reaction( long in_SrcSectionID, const CRenderSection_CreatePixelShader_Response& in_rResponse )
{
	CLog::Print("CSunSection::Reaction( const CRenderSection_CreatePixelShader_Response& )\n");
	if(0==in_rResponse.m_ShaderHandle)
	{
		CLog::Print("  Shader was not created\n");
		CDemoSection_WorkerInit_Response resp;
		resp.m_bSuccess = false;
		CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID, resp);
	}
	else
	{
		m_PSHandle = in_rResponse.m_ShaderHandle;
		CheckForInitializationCompletion();
	}
}

void
CSunSection::GenerateOneParticle( CParticle& rParticle, bool in_bRandomAge )
{
	float FMax = (float)RAND_MAX;
	float OOFMax = 1.0f/FMax;
	float FMax2 = (float)(RAND_MAX/2);
	float OOFMax2 = 1.0f/FMax2;
	float R2 = m_SunRadius*m_SunRadius;
	float X,Y,Z;
	float SizeCoeff = 0.0f;
	for(;;)
	{
		X = ((((float)rand())*OOFMax2)-1.0f) * m_SunRadius;
		Y = ((((float)rand())*OOFMax2)-1.0f) * m_SunRadius;
		Z = ((((float)rand())*OOFMax2)-1.0f) * m_SunRadius;
		float r2 = X*X+Y*Y+Z*Z;
		if( r2 <= R2 )
		{
			SizeCoeff = sqrt(r2)/sqrt(R2);
			//SizeCoeff = 1.5f - 1.0f*SizeCoeff;
			//SizeCoeff = 2.25f - 2.0f*SizeCoeff;
			SizeCoeff = 2.75f - 2.5f*SizeCoeff;
			break;
		}
	}
	rParticle.m_Org = CVector(X,Y,Z);
	rParticle.m_Size = m_ParticleRadiusMin + (m_ParticleRadiusMax-m_ParticleRadiusMin)*( ((float)rand())*OOFMax );
	rParticle.m_Size *= SizeCoeff;
	float Angle = float(CONST_2PI) * ((float)rand()) * OOFMax;
	rParticle.m_Sin = sin(Angle);
	rParticle.m_Cos = cos(Angle);
	float Age = m_ParticleLifeMin + (m_ParticleLifeMax-m_ParticleLifeMin) * ( ((float)rand())*OOFMax );
	rParticle.m_DeltaAge = 1.0f/( Age * ((float)VB_UPDATE_FREQUENCY) );
	if(in_bRandomAge)
		rParticle.m_Age = ((float)rand()) * OOFMax;
	else
		rParticle.m_Age = 0.0f;
}

void
CSunSection::ParseConfig( const std::vector<unsigned char>& in_rData )
{
	// TODO: parse sun config
	m_TextureUrl = DATA_LOCATION;
	m_TextureUrl += "sun.jpg";
	m_SunRadius = 2.0f;
	m_NParticles = 100;
	m_ParticleRadiusMin = 0.6f;
	m_ParticleRadiusMax = 1.0f;
	m_ParticleLifeMin = 1.0f;
	m_ParticleLifeMax = 1.5f;
}

void
CSunSection::CheckForInitializationCompletion()
{
//	CLog::Print("CSunSection::CheckForInitializationCompletion()\n");
//	CLog::Print("  m_PSHandle = %lu\n",m_PSHandle);
//	CLog::Print("  m_bHaveCamera = %s\n",m_bHaveCamera?"YES":"no");
//	CLog::Print("  m_bVBCreateRequestSent = %s\n",m_bVBCreateRequestSent?"YES":"no");
	if(0!=m_PSHandle && m_bHaveCamera && !m_bVBCreateRequestSent)
	{
		m_Particles.resize( m_NParticles );
		for( size_t i=0; i<m_NParticles; ++i )
		{
			GenerateOneParticle( m_Particles[i], true );
		}
		// create VB
		CVBFormat Format;
		Format.m_XYZ = true;
		Format.m_Diffuse = true;
		Format.m_Tex0Dimension = CVBFormat::TEXCOORDSET_FLAT;
		CRenderSection_CreateVertexBuffer_Request req( Format, true );
		size_t OneVertexSize = 3*sizeof(float) + sizeof(unsigned long) + 2*sizeof(float);
		req.m_Data.resize( 6*OneVertexSize*m_NParticles );
		unsigned char* pData = &(req.m_Data[0]);
		unsigned char* pEnd = pData+6*OneVertexSize*m_NParticles;
		for( size_t i=0; i<m_NParticles; ++i )
		{
			assert( pData+6*OneVertexSize <= pEnd );
			unsigned long Diffuse = AgeToDiffuse( m_Particles[i].m_Age );
			CVector Up		= m_Particles[i].m_Size*( m_CameraUp*m_Particles[i].m_Cos    - m_CameraRight*m_Particles[i].m_Sin );
			CVector Right	= m_Particles[i].m_Size*( m_CameraRight*m_Particles[i].m_Cos + m_CameraUp*m_Particles[i].m_Sin    );
			// 0
			*((CVector*)pData) = m_Particles[i].m_Org + Up;
			pData += 3*sizeof(float);
			*((unsigned long*)pData) = Diffuse;
			pData += sizeof(unsigned long);
			*((float*)pData) = 1.0f;
			pData += sizeof(float);
			*((float*)pData) = 0.0f;
			pData += sizeof(float);
			// 1
			*((CVector*)pData) = m_Particles[i].m_Org + Right;
			pData += 3*sizeof(float);
			*((unsigned long*)pData) = Diffuse;
			pData += sizeof(unsigned long);
			*((float*)pData) = 1.0f;
			pData += sizeof(float);
			*((float*)pData) = 1.0f;
			pData += sizeof(float);
			// 2
			*((CVector*)pData) = m_Particles[i].m_Org - Up;
			pData += 3*sizeof(float);
			*((unsigned long*)pData) = Diffuse;
			pData += sizeof(unsigned long);
			*((float*)pData) = 0.0f;
			pData += sizeof(float);
			*((float*)pData) = 1.0f;
			pData += sizeof(float);
			// 3
			*((CVector*)pData) = m_Particles[i].m_Org - Up;
			pData += 3*sizeof(float);
			*((unsigned long*)pData) = Diffuse;
			pData += sizeof(unsigned long);
			*((float*)pData) = 0.0f;
			pData += sizeof(float);
			*((float*)pData) = 1.0f;
			pData += sizeof(float);
			// 4
			*((CVector*)pData) = m_Particles[i].m_Org - Right;
			pData += 3*sizeof(float);
			*((unsigned long*)pData) = Diffuse;
			pData += sizeof(unsigned long);
			*((float*)pData) = 0.0f;
			pData += sizeof(float);
			*((float*)pData) = 0.0f;
			pData += sizeof(float);
			// 5
			*((CVector*)pData) = m_Particles[i].m_Org + Up;
			pData += 3*sizeof(float);
			*((unsigned long*)pData) = Diffuse;
			pData += sizeof(unsigned long);
			*((float*)pData) = 1.0f;
			pData += sizeof(float);
			*((float*)pData) = 0.0f;
			pData += sizeof(float);
		}
		CTCommandSender<CRenderSection_CreateVertexBuffer_Request>::SendCommand( m_RenderSectionID, req );
		m_bVBCreateRequestSent = true;
	}
}


















