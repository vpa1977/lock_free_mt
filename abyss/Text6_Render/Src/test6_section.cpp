#include "../Include/test6_section.h"
#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/section_registrator.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

static CTSectionRegistrator<CTest6Section_CLSID,CTest6Section> g_CTest6SectionRegistrator( SINGLE_INSTANCE_ONLY, STARTUP_SECTION );

CTest6Section::CTest6Section( long in_SectionID ) :
	CGameBaseSection( in_SectionID )
	,m_NThreadsToRun(-1)
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
	//CLog::Print("CTest6Section::CTest6Section()\n");
	RegisterReaction( GetID((CRenderSection_NewFrame*)NULL),					CTConform<CTest6Section,CRenderSection_NewFrame>::Conform );
	RegisterReaction( GetID((CRenderSection_CreateVertexBuffer_Response*)NULL),	CTConform<CTest6Section,CRenderSection_CreateVertexBuffer_Response>::Conform );
	RegisterReaction( GetID((CRenderSection_CreateTexture_Response*)NULL),		CTConform<CTest6Section,CRenderSection_CreateTexture_Response>::Conform );
	RegisterReaction( GetID((CRenderSection_CreatePixelShader_Response*)NULL),	CTConform<CTest6Section,CRenderSection_CreatePixelShader_Response>::Conform );
	RegisterReaction( GetID((CTest6Section_Render*)NULL),						CTConform<CTest6Section,CTest6Section_Render>::Conform );
}

CTest6Section::~CTest6Section()
{
	//CLog::Print("CTest6Section::~CTest6Section()\n");
}

unsigned long g_Tex[] = {
	0xff,0xff00,0xff0000,0xffff00,0xff,0xff00,0xff0000,0xffff00,
	0xffff00,0xff,0xff00,0xff0000,0xffff00,0xff,0xff00,0xff0000,
	0xff0000,0xffff00,0xff,0xff00,0xff0000,0xffff00,0xff,0xff00,
	0xff00,0xff0000,0xffff00,0xff,0xff00,0xff0000,0xffff00,0xff,
	0xff,0xff00,0xff0000,0xffff00,0xff,0xff00,0xff0000,0xffff00,
	0xffff00,0xff,0xff00,0xff0000,0xffff00,0xff,0xff00,0xff0000,
	0xff0000,0xffff00,0xff,0xff00,0xff0000,0xffff00,0xff,0xff00,
	0xff00,0xff0000,0xffff00,0xff,0xff00,0xff0000,0xffff00,0xff,
};

void
CTest6Section::Reaction( long in_SrcSectionID, const CRenderSection_CreateVertexBuffer_Response& in_rCmd )
{
	CLog::Print("CTest6Section::Reaction( const CRenderSection_CreateVertexBuffer_Response& )\n");
	CLog::Print("  VB handle = %lu\n",in_rCmd.m_VBHandle);
	assert( in_rCmd.m_VBHandle > 0 );
	m_VBHandle = in_rCmd.m_VBHandle;
	//
	CRenderSection_CreateTexture_Request Cmd(8,8,false);
	Cmd.m_Data.resize( 8*8*sizeof(unsigned long) );
	unsigned long* pData = (unsigned long*)&(Cmd.m_Data[0]);
	memcpy(pData,g_Tex,8*8*sizeof(unsigned long));
	CTCommandSender<CRenderSection_CreateTexture_Request>::SendCommand(
		GetRenderSectionID(),
		Cmd
	);
}

void
CTest6Section::Reaction( long in_SrcSectionID, const CRenderSection_CreateTexture_Response& in_rCmd )
{
	CLog::Print("CTest6Section::Reaction( const CRenderSection_CreateTexture_Response& )\n");
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
		//rPass.m_bLightingEnabled = false;
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
			//rStage.m_ColorArg1 = RTSA_DIFFUSE;
			rStage.m_ColorArg1 = RTSA_TEXTURE;
			rStage.m_ColorArg2 = RTSA_CURRENT;
			rStage.m_ColorOp = RTSO_MODULATE;
			//rStage.m_ColorOp = RTSO_SELECTARG1;
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
CTest6Section::Reaction( long in_SrcSectionID, const CRenderSection_CreatePixelShader_Response& in_rCmd )
{
	CLog::Print("CTest6Section::Reaction( const CRenderSection_CreatePixelShader_Response& )\n");
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
	m_M0.ConstructScaling( CVector(2,2,2) );
	m_M1.ConstructRotationY( 0 );
	m_M1 *= CMatrix().ConstructTranslation( CVector(4,0,0) );
	m_M2.ConstructRotationY( CONST_PI_2 );
	m_M2 *= CMatrix().ConstructTranslation( CVector(0,0,4) );
	m_M3.ConstructRotationY( CONST_PI_2*2 );
	m_M3 *= CMatrix().ConstructTranslation( CVector(-4,0,0) );
	m_M4.ConstructRotationY( CONST_PI_2*3 );
	m_M4 *= CMatrix().ConstructTranslation( CVector(0,0,-4) );
	// start rendering
	m_NFramesToRender = 2;
	RenderFrame();
	CTCommandSender<CTest6Section_Render>::SendCommand(
		GetThisID(),
		CTest6Section_Render()
	);
}

void
CTest6Section::Received_WindowClosed()
{
	CLog::Print("CTest6Section::Received_WindowClosed()\n");
}

unsigned long
CalcNPrimitives(unsigned long in_PrimitiveType, unsigned long in_NVertices)
{
	assert(in_PrimitiveType>=PRIM_POINT_LIST && in_PrimitiveType<=PRIM_TRIANGLE_FAN);
	switch(in_PrimitiveType)
	{
		case PRIM_POINT_LIST:
			return in_NVertices;
		case PRIM_LINE_LIST:
			return in_NVertices/2;
		case PRIM_LINE_STRIP:
			return in_NVertices-1;
		case PRIM_TRIANGLE_LIST:
			return in_NVertices/3;
		case PRIM_TRIANGLE_STRIP:
		case PRIM_TRIANGLE_FAN:
			return in_NVertices-2;
	}
	return 0;
}

void
CTest6Section::StartTheGame()
{
	CLog::Print("CTest6Section::StartTheGame()\n");
	//
	CVBFormat VBFormat;
	std::vector<unsigned char> Data;
	m_PrimitiveType = 0;
	bool Success = LoadDataFromFile("object.txt",VBFormat,Data,m_PrimitiveType,m_NVertices);
	assert(Success);
	CLog::Print("  m_PrimitiveType=%lu\n",m_PrimitiveType);
	CLog::Print("  m_NVertices=%lu\n",m_NVertices);
	m_NPrimitives = CalcNPrimitives(m_PrimitiveType,m_NVertices);
	CLog::Print("  m_NPrimitives=%lu\n",m_NPrimitives);
	CRenderSection_CreateVertexBuffer_Request Cmd( VBFormat, false );
	Cmd.m_Data = Data;
	CTCommandSender<CRenderSection_CreateVertexBuffer_Request>::SendCommand(
		GetRenderSectionID(),
		Cmd
	);
}






















