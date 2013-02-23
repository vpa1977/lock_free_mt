#include "../include/m3_starfield.h"
#include "../../MT_Core/Include/log.h"
#include "../../MT_Core/Include/rtassert.h"
#include <assert.h>
#include <stdio.h>
#include <list>

CStarField::CStarField( CGameObjectOwner* in_pOwner ) :
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
CStarField::ReadFileResult( bool in_bSuccess, const std::string& in_rFileName, const std::vector<unsigned char>& in_rData )
{
	if(!m_bPreloadError)
	{
		if(in_bSuccess)
		{
			CVBFormat VBFormat;
			VBFormat.m_XYZ = true;
			VBFormat.m_Tex0Dimension = CVBFormat::TEXCOORDSET_FLAT;
			CRenderSection_CreateVertexBuffer_Request req( VBFormat, false, (unsigned long)this );
			ParseStarsCatalog( in_rData, req.m_Data );
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
CStarField::DecodeFileResult( bool in_bSuccess, const std::string& in_rFileName, long in_Width, long in_Height, const std::vector<unsigned char>& in_rData )
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
CStarField::DecodeRawResult( bool in_bSuccess, long in_Width, long in_Height, const std::vector<unsigned char>& in_rData )
{
	RTASSERT(false);
}

void
CStarField::CreateVBResult( unsigned long in_VBHandle )
{
	if(!m_bPreloadError)
	{
		if(in_VBHandle>0)
		{
			m_VBHandle = in_VBHandle;
			// decode texture
			std::string TextureName("/data/");
			TextureName += "star.jpg";
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
CStarField::UpdateVBResult( bool in_bSuccess )
{
	RTASSERT(false);
}

void
CStarField::CreateTextureResult( unsigned long in_TexHandle )
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
				rPass.m_SrcBlend = RBM_ONE;
				rPass.m_DstBlend = RBM_ONE;
				rPass.m_bLightingEnabled = false;
				rPass.m_TFactor = 0;
				rPass.m_bAlphaTest = false;
				rPass.m_bDepthWrite = false;
				rPass.m_bDepthEnabled = false;
				rPass.m_ZCmpMode = RCMP_ALWAYS;
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
					rStage.m_AlphaOp = RTSO_DISABLE;
					//
					rStage.m_ColorArg1 = RTSA_TEXTURE;
					rStage.m_ColorArg2 = RTSA_CURRENT;
					rStage.m_ColorOp = RTSO_SELECTARG1;
					//
					rStage.m_TextureAddressModeU = RTAM_CLAMP;
					rStage.m_TextureAddressModeV = RTAM_CLAMP;
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
CStarField::CreatePSResult( unsigned long in_ShaderHandle )
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
CStarField::CreateFontResult(unsigned long in_FontHandle)
{
	RTASSERT(false);
}

void
CStarField::vStartPreload()
{
	CLog::Print("STAR FIELD starting preload.\n");

	// read model
	std::string ModelName("/data/");
	ModelName += "catalog.txt";
	CLoadProxy::getInstance().ReadFile( ModelName.c_str(), this );
}

void
CStarField::vUpdateBeforeChildren( unsigned long in_Time )
{
	if(!m_bPreloadError)
	{
//		CLog::Print("CStarField::vUpdateBeforeChildren( %lu );\n",in_Time);
		m_Matrix.ConstructScaling( CVector(5.0f,5.0f,5.0f) );
		float AngleSelf = CONST_2PI*float(in_Time%(10000))/10000.0f;
//		CLog::Print("  AngleSelf = %f\n",AngleSelf);
		CMatrix RotationSelf; RotationSelf.ConstructRotationY(AngleSelf);
		CMatrix RotationTilt; RotationTilt.ConstructRotationZ(CONST_PI_6);
		float AngleAxisDrift = -CONST_2PI*float(in_Time%(40000))/40000.0f;
		CMatrix RotationDrift; RotationDrift.ConstructRotationY(AngleAxisDrift);
		m_Matrix *= RotationSelf;
		m_Matrix *= RotationTilt;
		m_Matrix *= RotationDrift;

		CRenderSection_RenderVB Cmd( m_Matrix, m_VBHandle, m_PSHandle, 0, m_PrimitiveType, m_PrimitiveCount );
		CRenderProxy::getInstance().RenderVB( Cmd );
	}
}

struct CStar
{
	CStar( float in_Shirota=0, float in_Dolgota=0, float in_Size=0 ) :
		m_Shirota(in_Shirota)
		,m_Dolgota(in_Dolgota)
		,m_Size(in_Size)
	{}
	float m_Shirota;
	float m_Dolgota;
	float m_Size;
};

void
CStarField::ParseStarsCatalog( const std::vector<unsigned char>& in_rData, std::vector<unsigned char>& out_rVBData )
{
	CLog::Print("ParseStarsCatalog()\n");
	std::list< CStar > StarsArray;
	const char* pBegin = (const char*)&in_rData[0];
	const char* pEnd = pBegin+in_rData.size();
	for( ; pBegin!=pEnd; )
	{
		const char* pEndLine = pBegin;
		while( pEndLine!=pEnd && *pEndLine!='\r' && *pEndLine!='\n')
			pEndLine++;
		// line is:  [pBegin..pEndLine)
		if(pBegin!=pEndLine)
		{
			// decode line
			std::string S(pBegin,pEndLine-pBegin);
			const char* pS = S.c_str();
			if(*pS != '#')
			{
				float DolgotaHour = 0.0f;
				float DolgotaMin = 0.0f;
				float DolgotaSec = 0.0f;
				float ShirotaDeg = 0.0f;
				float ShirotaMin = 0.0f;
				float ShirotaSec = 0.0f;
				float Magnitude  = 0.0f;
				bool Ok = false;
				do
				{
					// comma after traditional name
					const char* pComma = strchr(pS,',');
					if(!pComma)
						break;
					pS = pComma+1;
					// comma after nomenclature name
					pComma = strchr(pS,',');
					if(!pComma)
						break;
					pS = pComma+1;
					// comma after equinox
					pComma = strchr(pS,',');
					if(!pComma)
						break;
					pS = pComma+1;
					// comma after Dolgota Deg
					pComma = strchr(pS,',');
					if(!pComma)
						break;
					DolgotaHour = atof( std::string(pS,pComma-pS).c_str() );
					pS = pComma+1;
					// comma after Dolgota Min
					pComma = strchr(pS,',');
					if(!pComma)
						break;
					DolgotaMin = atof( std::string(pS,pComma-pS).c_str() );
					pS = pComma+1;
					// comma after Dolgota Sec
					pComma = strchr(pS,',');
					if(!pComma)
						break;
					DolgotaSec = atof( std::string(pS,pComma-pS).c_str() );
					pS = pComma+1;
					// comma after Shirota Deg
					pComma = strchr(pS,',');
					if(!pComma)
						break;
					ShirotaDeg = atof( std::string(pS,pComma-pS).c_str() );
					pS = pComma+1;
					// comma after Shirota Min
					pComma = strchr(pS,',');
					if(!pComma)
						break;
					ShirotaMin = atof( std::string(pS,pComma-pS).c_str() );
					pS = pComma+1;
					// comma after Shirota Sec
					pComma = strchr(pS,',');
					if(!pComma)
						break;
					ShirotaSec = atof( std::string(pS,pComma-pS).c_str() );
					pS = pComma+1;
					// comma after Motion 1
					pComma = strchr(pS,',');
					if(!pComma)
						break;
					pS = pComma+1;
					// comma after Motion 2
					pComma = strchr(pS,',');
					if(!pComma)
						break;
					pS = pComma+1;
					// comma after velocity
					pComma = strchr(pS,',');
					if(!pComma)
						break;
					pS = pComma+1;
					// comma after parallax
					pComma = strchr(pS,',');
					if(!pComma)
						break;
					pS = pComma+1;
					// comma after Magnitude
					pComma = strchr(pS,',');
					if(!pComma)
						break;
					Magnitude = atof( std::string(pS,pComma-pS).c_str() );
					Ok = true;
				} while(false);
				if(Ok)
				{
					//CLog::Print("  dolgota = %1.2f %1.2f %1.2f\n",DolgotaHour,DolgotaMin,DolgotaSec);
					//CLog::Print("  shirota = %1.2f %1.2f %1.2f\n",ShirotaDeg,ShirotaMin,ShirotaSec);
					//CLog::Print("  magnitude = %1.2f\n",Magnitude);
					float Dolgota = DolgotaHour + DolgotaMin/60.0f + DolgotaSec/3600.0f;
					Dolgota *= (CONST_2PI/24.0f);
					float Shirota = ShirotaDeg + ShirotaMin/60.0f + ShirotaSec/3600.0f;
					Shirota *= (CONST_PI_2/90.0f);
					Magnitude = (Magnitude<-2.0f) ? -2.0f : ((Magnitude>6.0f)?6.0f:Magnitude);
					assert(Magnitude>=-2.0f && Magnitude<=6.0f);
					float k=-0.9f/8.0f;
					float b=1.0f-(0.9f/4.0f);
					Magnitude = k*Magnitude+b;
					Magnitude = (Magnitude<0.1f) ? 0.1f : ((Magnitude>1.0f)?1.0f:Magnitude);
					assert(Magnitude>=0.1f && Magnitude<=1.0f);
					StarsArray.push_back( CStar(Shirota,Dolgota,Magnitude) );
				}
			}
		}
		while( pEndLine!=pEnd && (*pEndLine=='\r' || *pEndLine=='\n') )
			pEndLine++;
		pBegin = pEndLine;
	}

	// generate VB data
	CLog::Print("  %ld stars\n",StarsArray.size());
	out_rVBData.resize( StarsArray.size() * 6 * ( 3*sizeof(float) + 2*sizeof(float) ) );
	unsigned char* pDstBegin = (unsigned char*)&out_rVBData[0];
	unsigned char* pDstEnd = pDstBegin+out_rVBData.size();
	for( std::list<CStar>::const_iterator It = StarsArray.begin(); It!=StarsArray.end(); ++It )
	{
		//
		CVector Dir(
			cos(It->m_Shirota)*sin(It->m_Dolgota),
			sin(It->m_Shirota),
			cos(It->m_Shirota)*cos(It->m_Dolgota)
		);
		Dir.Normalize();
		//
		CVector QQ;
		if( CONST_PI_4 < fabs(It->m_Shirota) )
			QQ = CVector(1,0,0);
		else
			QQ = CVector(0,1,0);
		//
		CVector Right( Dir*QQ );
		Right.Normalize();
		CVector Up( Right*Dir );
		Up.Normalize();
		//
		Dir   *= 10.0f;
		Right *= 0.1f*It->m_Size;
		Up    *= 0.1f*It->m_Size;
		//
		assert( pDstBegin + 6 * ( 3*sizeof(float) + 2*sizeof(float) ) <= pDstEnd );
		CVector VUp = Dir+Up;
		CVector VRight = Dir+Right;
		CVector VDown = Dir-Up;
		CVector VLeft = Dir-Right;
		// v1
		*( (CVector*)pDstBegin ) = VUp;
		pDstBegin += 3*sizeof(float);
		*( (float*)pDstBegin ) = 1.0f;
		pDstBegin += sizeof(float);
		*( (float*)pDstBegin ) = 0.0f;
		pDstBegin += sizeof(float);
		// v2
		*( (CVector*)pDstBegin ) = VRight;
		pDstBegin += 3*sizeof(float);
		*( (float*)pDstBegin ) = 1.0f;
		pDstBegin += sizeof(float);
		*( (float*)pDstBegin ) = 1.0f;
		pDstBegin += sizeof(float);
		// v3
		*( (CVector*)pDstBegin ) = VDown;
		pDstBegin += 3*sizeof(float);
		*( (float*)pDstBegin ) = 0.0f;
		pDstBegin += sizeof(float);
		*( (float*)pDstBegin ) = 1.0f;
		pDstBegin += sizeof(float);
		// v4
		*( (CVector*)pDstBegin ) = VDown;
		pDstBegin += 3*sizeof(float);
		*( (float*)pDstBegin ) = 0.0f;
		pDstBegin += sizeof(float);
		*( (float*)pDstBegin ) = 1.0f;
		pDstBegin += sizeof(float);
		// v5
		*( (CVector*)pDstBegin ) = VLeft;
		pDstBegin += 3*sizeof(float);
		*( (float*)pDstBegin ) = 0.0f;
		pDstBegin += sizeof(float);
		*( (float*)pDstBegin ) = 0.0f;
		pDstBegin += sizeof(float);
		// v6
		*( (CVector*)pDstBegin ) = VUp;
		pDstBegin += 3*sizeof(float);
		*( (float*)pDstBegin ) = 1.0f;
		pDstBegin += sizeof(float);
		*( (float*)pDstBegin ) = 0.0f;
		pDstBegin += sizeof(float);
	}
	m_PrimitiveType = PRIM_TRIANGLE_LIST;
	m_PrimitiveCount = 2*StarsArray.size();
}


void
CStarField::CheckPreloadComplete()
{
	if( m_PSHandle>0 && m_VBHandle>0 )
	{
		CLog::Print("STAR FIELD preload complete.\n");
		PreloadComplete(true);
	}
	else
	{
		CLog::Print("STAR FIELD preload FAILED.\n");
		PreloadComplete(false);
	}
}
