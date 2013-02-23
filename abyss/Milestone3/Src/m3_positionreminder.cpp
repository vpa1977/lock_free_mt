#include "../include/m3_positionreminder.h"
#include "../include/vertex_loader.h"

#include "../../MT_Core/Include/log.h"
#include "../../MT_Core/Include/rtassert.h"
#include <assert.h>
#include <stdio.h>

CPositionReminder::CPositionReminder( CGameObjectOwner* in_pOwner ) :
	CGameObject( in_pOwner )
	,m_bPreloadError(false)
	,m_TextureHandle(0)
	,m_PSHandle(0)
	,m_PrimitiveCount(0)
	,m_PrimitiveType(0)
	,m_VBHandle(0)
{
}

void
CPositionReminder::ReadFileResult( bool in_bSuccess, const std::string& in_rFileName, const std::vector<unsigned char>& in_rData )
{
	if(!m_bPreloadError)
	{
		if(in_bSuccess)
		{
			CVertexLoader Loader;
			Loader.Load(in_rData);

			m_PrimitiveCount = Loader.GetNPrimitives();
			m_PrimitiveType = Loader.GetPrimitiveType();

			CRenderSection_CreateVertexBuffer_Request req( Loader.GetVBFormat(), false, (unsigned long)this );
			req.m_Data = Loader.GetData();
			CRenderProxy::getInstance().CreateVB( req, this );
		}
		else
		{
			m_bPreloadError = true;
			PreloadComplete(false);
		}
	}
}

void
CPositionReminder::DecodeFileResult( bool in_bSuccess, const std::string& in_rFileName, long in_Width, long in_Height, const std::vector<unsigned char>& in_rData )
{
	if(!m_bPreloadError)
	{
		if(in_bSuccess)
		{
			RTASSERT(in_Width>0 && in_Height>0);
			CRenderSection_CreateTexture_Request req(in_Width,in_Height,true,(unsigned long)this);
			req.m_Data = in_rData;
			CRenderProxy::getInstance().CreateTexture(req,this);
		}
		else
		{
			m_bPreloadError = true;
			PreloadComplete(false);
		}
	}
}

void
CPositionReminder::DecodeRawResult( bool in_bSuccess, long in_Width, long in_Height, const std::vector<unsigned char>& in_rData )
{
	RTASSERT(false);
}

void
CPositionReminder::CreateVBResult( unsigned long in_VBHandle )
{
	if(!m_bPreloadError)
	{
		if(in_VBHandle>0)
		{
			m_VBHandle = in_VBHandle;
			// decode texture
			std::string TextureName("/data/");
			TextureName += "position_reminder.tga";
			CLoadProxy::getInstance().DecodeImageFromFile( TextureName.c_str(), this );
		}
		else
		{
			m_bPreloadError = true;
			PreloadComplete(false);
		}
	}
}

void
CPositionReminder::UpdateVBResult( bool in_bSuccess )
{
	RTASSERT(false);
}

void
CPositionReminder::CreateTextureResult( unsigned long in_TexHandle )
{
	if(!m_bPreloadError)
	{
		if(in_TexHandle>0)
		{
			m_TextureHandle = in_TexHandle;
			//
			CRenderSection_CreatePixelShader_Request Cmd;
			CPixelShader& rShaderDesc = Cmd.m_ShaderDescription;
			rShaderDesc.m_Passes.resize(1);
			{
				// pass 0
				CPixelShaderPass& rPass = rShaderDesc.m_Passes[0];
				rPass.m_CullMode = RCM_CCW;
				rPass.m_SrcBlend = RBM_SRCALPHA;
				//rPass.m_SrcBlend = RBM_ONE;
				rPass.m_DstBlend = RBM_INVSRCALPHA;
				//rPass.m_DstBlend = RBM_ZERO;
				rPass.m_bLightingEnabled = false;
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
					rStage.m_AlphaArg1 = RTSA_TEXTURE;
					rStage.m_AlphaArg2 = RTSA_CURRENT;
					rStage.m_AlphaOp = RTSO_SELECTARG1;
					//
					rStage.m_ColorArg1 = RTSA_TEXTURE;
					rStage.m_ColorArg2 = RTSA_CURRENT;
					rStage.m_ColorOp = RTSO_SELECTARG1;
					//
					rStage.m_TextureAddressModeU = RTAM_WRAP;
					rStage.m_TextureAddressModeV = RTAM_WRAP;
					rStage.m_TextureIndex = 0;
					rStage.m_TextureMagFilter = RTFM_LINEAR;
					rStage.m_TextureMinFilter = RTFM_LINEAR;
					rStage.m_TextureMipFilter = RTFM_NONE;
				}
			}
			CRenderProxy::getInstance().CreatePS( Cmd, this );
		}
		else
		{
			m_bPreloadError = true;
			PreloadComplete(false);
		}
	}
}

void
CPositionReminder::CreatePSResult( unsigned long in_ShaderHandle )
{
	if(!m_bPreloadError)
	{
		if(in_ShaderHandle>0)
		{
			m_PSHandle = in_ShaderHandle;
			CheckPreloadComplete();
		}
		else
		{
			m_bPreloadError = true;
			PreloadComplete(false);
		}
	}
}

void
CPositionReminder::CreateFontResult(unsigned long in_FontHandle)
{
	RTASSERT(false);
}

void
CPositionReminder::vStartPreload()
{
	CLog::Print("POSITION REMINDER starting preload.\n");

//	// decode texture
//	std::string TextureName("/data/");
//	TextureName += "position_reminder.tga";
//	CLoadProxy::getInstance().DecodeImageFromFile( TextureName.c_str(), this );
	// read model
	std::string ModelName("/data/");
	ModelName += "position_reminder.txt";
	CLoadProxy::getInstance().ReadFile( ModelName.c_str(), this );
	//PreloadComplete(true);
}

void
CPositionReminder::vUpdateBeforeChildren( unsigned long in_Time )
{
	if(!m_bPreloadError)
	{
		CRenderSection_RenderVB Cmd( m_Matrix, m_VBHandle, m_PSHandle, 0, m_PrimitiveType, m_PrimitiveCount );
		CRenderProxy::getInstance().RenderVB( Cmd );
	}
}

void
CPositionReminder::CheckPreloadComplete()
{
	if( m_PSHandle>0 && m_VBHandle>0 )
	{
		CMatrix m1; m1.ConstructScaling( CVector(1.0f,1.0f,1.0f) );
		CMatrix m2; m2.ConstructTranslation( CVector(-5.6f,-5.6f,0.0f) );
		m_Matrix = m1*m2;
		CLog::Print("POSITION REMINDER preload complete.\n");
		PreloadComplete(true);
	}
	else
	{
		CLog::Print("POSITION REMINDER preload FAILED.\n");
		PreloadComplete(false);
	}
}
