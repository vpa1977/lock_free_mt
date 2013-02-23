#include "../include/m3_gameunit.h"
#include "../include/vertex_loader.h"
#include "../include/intersect.h"

#include "../../MT_Core/Include/log.h"
#include "../../MT_Core/Include/rtassert.h"
#include <assert.h>
#include <stdio.h>


CGameUnit::CGameUnit( unsigned long in_N, CGameObjectOwner* in_pOwner ) :
	CGameObject( in_pOwner )
	,m_N(in_N)
	,m_InternalState(0)
	,m_X(0)
	,m_Y(0)
	,m_LastUpdateTime(0)
	,m_bMoving(false)
	,m_StartMoveTime(0)
	,m_DeltaTime(0)
	,m_DestinationX(0)
	,m_DestinationY(0)
	,m_bHighlighted(false)
{
	assert(m_N>=1 && m_N<=15);
}

void
CGameUnit::ReadFileResult( bool in_bSuccess, const std::string& in_rFileName, const std::vector<unsigned char>& in_rData )
{
	if (in_bSuccess ) 
	{
		CVertexLoader loader;
		loader.Load(in_rData);
		CRenderSection_CreateVertexBuffer_Request req(loader.GetVBFormat(), false);
		//req.m_VBFormat = 
		req.m_Data = loader.GetData();
		
		m_iPrimitiveCount = loader.GetNPrimitives();
		m_iPrimitiveType = loader.GetPrimitiveType();
		
		CRenderProxy::getInstance().CreateVB(req , this );
		m_IntersectDataHolder = loader.GetIntersectData();
	}
	else
	{
		PreloadComplete(false);
	}
}

void
CGameUnit::DecodeFileResult( bool in_bSuccess, const std::string& in_rFileName, long in_Width, long in_Height, const std::vector<unsigned char>& in_rData )
{
	if (in_bSuccess && m_InternalState == 1)
	{
			CRenderSection_CreateTexture_Request req(in_Width,in_Height,false);
			req.m_Data = in_rData;
			req.m_DX = in_Width;
			req.m_DY = in_Height;
			CRenderProxy::getInstance().CreateTexture(req,this);
			
		    
	}
	else
	if (in_bSuccess && m_InternalState == 2) 
	{

///		
			CRenderSection_CreateTexture_Request req(in_Width,in_Height,false);
			req.m_Data = in_rData;
			req.m_DX = in_Width;
			req.m_DY = in_Height;
			CRenderProxy::getInstance().CreateTexture(req,this);

	}
	else
	{
		PreloadComplete(false);
	}

}

void
CGameUnit::DecodeRawResult( bool in_bSuccess, long in_Width, long in_Height, const std::vector<unsigned char>& in_rData )
{
}

void
CGameUnit::CreateVBResult( unsigned long in_VBHandle )
{
	if (in_VBHandle == -1 ) 
	{
		PreloadComplete(false);
		return;
	}
	m_hObjectVB = in_VBHandle;
	// both of textures loaded and vb created.
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
			rPass.m_Textures[0] = m_hTextureNormal;
		}
		rPass.m_Stages.resize(1);
		{
			// pass 0, stage 0
			CPixelShaderPassStage& rStage = rPass.m_Stages[0];
			//
			rStage.m_AlphaArg1 = RTSA_TEXTURE;
			rStage.m_AlphaArg2 = RTSA_CURRENT;
			rStage.m_AlphaOp = RTSO_DISABLE;
			//
			rStage.m_ColorArg1 = RTSA_TEXTURE;
			rStage.m_ColorArg2 = RTSA_CURRENT;
			rStage.m_ColorOp = RTSO_MODULATE;
			//
			rStage.m_TextureAddressModeU = RTAM_WRAP;
			rStage.m_TextureAddressModeV = RTAM_WRAP;
			rStage.m_TextureIndex = 0;
			rStage.m_TextureMagFilter = RTFM_LINEAR;
			rStage.m_TextureMinFilter = RTFM_LINEAR;
			rStage.m_TextureMipFilter = RTFM_NONE;
		}
	}
	m_InternalState = 4;
	CRenderProxy::getInstance().CreatePS(Cmd, this );

}

void
CGameUnit::UpdateVBResult( bool in_bSuccess )
{
}

void
CGameUnit::CreateTextureResult( unsigned long in_TexHandle )
{
	if (in_TexHandle == -1 ) 
	{
		PreloadComplete(false);
		return;
	}
	if (m_InternalState == 1 ) 
	{
        m_hTextureNormal = in_TexHandle;
		char buf[5];
		sprintf(buf, "%d", m_N);
		std::string bTextureName("/data/");
		bTextureName+=buf;
		bTextureName +="l.jpg";
		// done initializing names.
		m_InternalState = 2;
		CLoadProxy::getInstance().DecodeImageFromFile((const char*)bTextureName.data() , this );
	}
	else
	if (m_InternalState == 2 )
	{
		char buf[256];
		sprintf(buf, "/data/object%d.txt",m_N);
		m_hTextureLight = in_TexHandle;
		CLoadProxy::getInstance().ReadFile( buf, this );
		m_InternalState =3;

	}
	else
	{
		PreloadComplete(false);
	}
}

void
CGameUnit::CreatePSResult( unsigned long in_ShaderHandle )
{
	if (in_ShaderHandle == -1 ) 
	{
		PreloadComplete(false);
		return;
	}
	if (m_InternalState == 4 )
	{
		m_hShaderNormal = in_ShaderHandle;
		m_InternalState = 5;

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
				rPass.m_Textures[0] = m_hTextureLight;
			}
			rPass.m_Stages.resize(1);
			{
				// pass 0, stage 0
				CPixelShaderPassStage& rStage = rPass.m_Stages[0];
				//
				rStage.m_AlphaArg1 = RTSA_TEXTURE;
				rStage.m_AlphaArg2 = RTSA_CURRENT;
				rStage.m_AlphaOp = RTSO_DISABLE;
				//
				rStage.m_ColorArg1 = RTSA_TEXTURE;
				rStage.m_ColorArg2 = RTSA_CURRENT;
				rStage.m_ColorOp = RTSO_MODULATE;
				//
				rStage.m_TextureAddressModeU = RTAM_WRAP;
				rStage.m_TextureAddressModeV = RTAM_WRAP;
				rStage.m_TextureIndex = 0;
				rStage.m_TextureMagFilter = RTFM_LINEAR;
				rStage.m_TextureMinFilter = RTFM_LINEAR;
				rStage.m_TextureMipFilter = RTFM_NONE;
			}
		}
		CRenderProxy::getInstance().CreatePS(Cmd, this );
	}
	else
	if (m_InternalState == 5 ) 
	{
		m_hShaderLight = in_ShaderHandle;

		long M = m_N-1;
		assert(M>=0 && M<=14);
		SetPosition(M%4,M/4);

		PreloadComplete(true);
	}
	else
	{
		PreloadComplete(false);
	}
}

void
CGameUnit::CreateFontResult(unsigned long in_FontHandle)
{
}

void CGameUnit::SetPosition(int X, int Y)
{
	m_X = X;
	m_Y = Y;
	m_Matrix.ConstructScaling( CVector(0.9f,0.9f,0.9f) );
	m_Matrix *= CMatrix().ConstructTranslation( CVector( -3.0f+X*2.0f, 3.0f-Y*2.0f, 0.0f ) );
	m_bMoving = false;
}

void
CGameUnit::MoveToPosition(long X, long Y, unsigned long in_DeltaTime)
{
	assert( !m_bMoving );
	assert( in_DeltaTime > 0 );
	assert( (X==m_X || Y==m_Y) && (X!=m_X || Y!=m_Y) );
	assert( abs(X-m_X)<=1 );
	assert( abs(Y-m_Y)<=1 );
	m_bMoving = true;
	m_StartMoveTime = m_LastUpdateTime;
	m_DeltaTime = in_DeltaTime;
	m_DestinationX = X;
	m_DestinationY = Y;
}

void
CGameUnit::vStartPreload()
{
	char buf[5];
	
	CLog::Print("START PRELOAD UNIT %lu.\n",m_N);
	
	sprintf(buf, "%d", m_N);

	
	std::string bTextureName("/data/");
	bTextureName+=buf;
	bTextureName +=".jpg";
	// done initializing names.
	CLoadProxy::getInstance().DecodeImageFromFile((const char*)bTextureName.data() , this );
	m_InternalState = 1;
}

void
CGameUnit::vUpdateBeforeChildren( unsigned long in_Time )
{
	if (m_bPreloadComplete ) 
	{
		m_LastUpdateTime = in_Time;
		if(m_bMoving)
		{
			if(in_Time>=m_StartMoveTime+m_DeltaTime)
			{
				m_X = m_DestinationX;
				m_Y = m_DestinationY;
				m_bMoving = false;
			}
			float Q = float(in_Time-m_StartMoveTime)/float(m_DeltaTime);
			if(Q<0.0f)
				Q=0.0f;
			else
				if(Q>1.0f)
					Q=1.0f;
			float OldX = -3.0f+m_X*2.0f;
			float OldY = 3.0f-m_Y*2.0f;
			float DestX = -3.0f+m_DestinationX*2.0f;
			float DestY = 3.0f-m_DestinationY*2.0f;
			float XX = OldX + Q*(DestX-OldX);
			float YY = OldY + Q*(DestY-OldY);
			m_Matrix.ConstructScaling( CVector(0.9f,0.9f,0.9f) );
			m_Matrix *= CMatrix().ConstructTranslation( CVector( XX, YY, 0.0f ) );
		}
		long shaderHandle = 0;
		if (m_bHighlighted) 
		{
			shaderHandle = m_hShaderLight;
		}
		else
		{
			shaderHandle = m_hShaderNormal;
		}
		CRenderSection_RenderVB Cmd(m_Matrix , m_hObjectVB , shaderHandle , 0, m_iPrimitiveType , m_iPrimitiveCount);
		CRenderProxy::getInstance().RenderVB(Cmd);
	}
	else
	{
		RTASSERT(0);
	}
}

bool CGameUnit::GetHighlight(const CVector& in_Org, const CVector& in_Dir)
{
	if (m_bPreloadComplete) 
	{
		CMatrix MInv(m_Matrix); MInv.Invert();
		CVector ClickOrgLocal = in_Org*MInv;
		CVector ClickDirLocal = in_Dir*MInv-CVector(0,0,0)*MInv;

		return IntersectLineTriangleMesh
		(
			ClickOrgLocal,
			ClickDirLocal,
			m_IntersectDataHolder,
			m_iPrimitiveType
		);
	}
	return false;
}


bool CGameUnit::vAcceptMouse(const CVector& in_Org, const CVector& in_Dir, bool in_bClickDetected)
{
	//m_ClickOrgLocal = in_Org;
	//m_ClickDirLocal = in_Dir;
	//m_Matrix.ConstructTranslation( in_Org + in_Dir*50.0f );
	
	return false;
}

