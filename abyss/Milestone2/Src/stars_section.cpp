#include "../Include/stars_section.h"
#include "../Include/vertex_loader.h"
#include "../../MT_Core/Include/section_registrator.h"
#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/log.h"
#include "../../Render_DX8/Include/algebra.h"
#include <list>

static CTSectionRegistrator<CStarsSection_CLSID,CStarsSection>
g_StarsSectionRegistrator(SINGLE_INSTANCE_ONLY);

#define DATA_LOCATION "/data/"

CStarsSection::CStarsSection( long in_SectionID ) :
	CSection( in_SectionID )
	,m_RenderSectionID(SECTION_ID_ERROR)
	,m_VfsSectionID(SECTION_ID_ERROR)
	,m_MainSectionID(SECTION_ID_ERROR)
	,m_PreloaderSectionID(SECTION_ID_ERROR)
	,m_ConfigUrl("")
	,m_CatalogUrl("")
	,m_TextureUrl("")
	,m_ConfigHandle(0)
	,m_CatalogHandle(0)
	,m_TextureHandle(0)
	,m_VBHandle(0)
	,m_PSHandle(0)
	,m_THandle(0)
	,m_TriangleCount(0)
{
	// system
	RegisterReaction( GetID((CCommand_SectionStartup*)NULL),	CTConform<CStarsSection,CCommand_SectionStartup>::Conform );
	// demo section
	RegisterReaction( GetID((CDemoSection_WorkerInit_Request*)NULL),	CTConform<CStarsSection,CDemoSection_WorkerInit_Request>::Conform );
	RegisterReaction( GetID((CDemoSection_WorkerShutdown_Request*)NULL),	CTConform<CStarsSection,CDemoSection_WorkerShutdown_Request>::Conform );
	// preloader
	RegisterReaction( GetID((CPreloaderSection_DecodeError*)NULL),	CTConform<CStarsSection,CPreloaderSection_DecodeError>::Conform );
	RegisterReaction( GetID((CPreloaderSection_DecodeResponse*)NULL),	CTConform<CStarsSection,CPreloaderSection_DecodeResponse>::Conform );
	// vfs
	RegisterReaction( GetID((CVFSSection_OpenResponse*)NULL),	CTConform<CStarsSection,CVFSSection_OpenResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_Error<CVFSSection_OpenRequest>*)NULL),	CTConform<CStarsSection,CVFSSection_Error<CVFSSection_OpenRequest> >::Conform );
	RegisterReaction( GetID((CVFSSection_ReadResponse*)NULL),	CTConform<CStarsSection,CVFSSection_ReadResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_Error<CVFSSection_ReadRequest>*)NULL),	CTConform<CStarsSection,CVFSSection_Error<CVFSSection_ReadRequest> >::Conform );
	RegisterReaction( GetID((CVFSSection_CloseResponse*)NULL),	CTConform<CStarsSection,CVFSSection_CloseResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_Error<CVFSSection_CloseRequest>*)NULL),	CTConform<CStarsSection,CVFSSection_Error<CVFSSection_CloseRequest> >::Conform );
	// render
	RegisterReaction( GetID((CRenderSection_CreateVertexBuffer_Response*)NULL),	CTConform<CStarsSection,CRenderSection_CreateVertexBuffer_Response>::Conform );
	RegisterReaction( GetID((CRenderSection_CreateTexture_Response*)NULL),	CTConform<CStarsSection,CRenderSection_CreateTexture_Response>::Conform );
	RegisterReaction( GetID((CRenderSection_CreatePixelShader_Response*)NULL),	CTConform<CStarsSection,CRenderSection_CreatePixelShader_Response>::Conform );
}

CStarsSection::~CStarsSection()
{
}

void
CStarsSection::Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )
{
	CLog::Print("CStarsSection::Reaction( const CCommand_SectionStartup& )\n");
	CLog::Print("  sectionID = %ld\n",GetThisID());
}

void
CStarsSection::Reaction( long in_SrcSectionID, const CDemoSection_WorkerInit_Request& in_rWorkerInit_Request )
{
	CLog::Print("CStarsSection::Reaction( const CDemoSection_WorkerInit_Request& )\n");
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
CStarsSection::Reaction( long in_SrcSectionID, const CDemoSection_WorkerShutdown_Request& )
{
	CLog::Print("CStarsSection::Reaction( const CDemoSection_WorkerShutdown_Request& )\n");
}

void
CStarsSection::Reaction( long in_SrcSectionID, const CPreloaderSection_DecodeError& in_rDecodeError )
{
	CLog::Print("CStarsSection::Reaction( const CPreloaderSection_DecodeError& )\n");
	CLog::Println("Image decoding error %s" , in_rDecodeError.m_Message);
	CDemoSection_WorkerInit_Response resp;
	resp.m_bSuccess = false;
	CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID, resp);
}

void
CStarsSection::Reaction( long in_SrcSectionID, const CPreloaderSection_DecodeResponse& in_rDecodeResponse )
{
	CLog::Print("CStarsSection::Reaction( const CPreloaderSection_DecodeResponse& )\n");
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
CStarsSection::Reaction( long in_SrcSectionID, const CVFSSection_OpenResponse& in_rOpenResponse )
{
	CLog::Print("CStarsSection::Reaction( const CVFSSection_OpenResponse& )\n");
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
	else if( in_rOpenResponse.m_Url == m_CatalogUrl )
	{
		CLog::Print("  Catalog opened\n");
		assert( 0 != in_rOpenResponse.m_Handle );
		m_CatalogHandle = in_rOpenResponse.m_Handle;
		// read star catalog
		CVFSSection_ReadRequest req;
		req.m_Handle = m_CatalogHandle;
		req.m_Offset = 0;
		req.m_Size = -1;
		CLog::Print("  sending Read catalog\n");
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
CStarsSection::Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_OpenRequest>& in_rOpenError )
{
	CLog::Print("CStarsSection::Reaction( const CVFSSection_Error<CVFSSection_OpenRequest>& )\n");
	CLog::Print("VFS error %d\n" , in_rOpenError.m_ErrorCode );
	CDemoSection_WorkerInit_Response resp;
	resp.m_bSuccess = false;
	CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID, resp);
}

void
CStarsSection::Reaction( long in_SrcSectionID, const CVFSSection_ReadResponse& in_rReadResponse )
{
	CLog::Print("CStarsSection::Reaction( const CVFSSection_ReadResponse& )\n");
	CTCommandSender<CVFSSection_CloseRequest>::SendCommand(
		m_VfsSectionID
		,CVFSSection_CloseRequest(in_rReadResponse.m_Handle)
	);
	if( in_rReadResponse.m_Handle == m_ConfigHandle ) 
	{
		CLog::Print("  Config read\n");
		assert(in_rReadResponse.m_Offset==0);
		ParseConfig( in_rReadResponse.m_pData );
		// open star catalog
		CLog::Print("  sending Open catalog\n");
		CTCommandSender<CVFSSection_OpenRequest>::SendCommand(
			m_VfsSectionID
			,CVFSSection_OpenRequest(m_CatalogUrl)
		);
		// open star texture
		CLog::Print("  sending Open texture file\n");
		CTCommandSender<CVFSSection_OpenRequest>::SendCommand(
			m_VfsSectionID
			,CVFSSection_OpenRequest(m_TextureUrl)
		);
	}
	else if( in_rReadResponse.m_Handle == m_CatalogHandle )
	{
		CLog::Print("  Catalog read\n");
		assert(in_rReadResponse.m_Offset==0);
		CVBFormat VBFormat;
		VBFormat.m_XYZ = true;
		VBFormat.m_Tex0Dimension = CVBFormat::TEXCOORDSET_FLAT;
		CRenderSection_CreateVertexBuffer_Request req( VBFormat, false );
		ParseStarsCatalog( in_rReadResponse.m_pData, req.m_Data );
		CLog::Print("  sending Create VB\n");
		CTCommandSender<CRenderSection_CreateVertexBuffer_Request>::SendCommand(
			m_RenderSectionID
			,req
		);
	}
	else
	{
		assert(false);
	}
}

void
CStarsSection::Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_ReadRequest>& in_rReadError )
{
	CLog::Print("CStarsSection::Reaction( const CVFSSection_Error<CVFSSection_ReadRequest>& )\n");
	CLog::Print("VFS error %d\n" , in_rReadError.m_ErrorCode );
	CDemoSection_WorkerInit_Response resp;
	resp.m_bSuccess = false;
	CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID, resp);
}

void
CStarsSection::Reaction( long in_SrcSectionID, const CVFSSection_CloseResponse& in_rCloseResponse )
{
	CLog::Print("CStarsSection::Reaction( const CVFSSection_CloseResponse& )\n");
	if(m_ConfigHandle==in_rCloseResponse.m_Handle)
	{
		m_ConfigHandle = 0;
	}
	else if(m_CatalogHandle==in_rCloseResponse.m_Handle)
	{
		m_CatalogHandle = 0;
	}
	else if(m_TextureHandle==in_rCloseResponse.m_Handle)
	{
		m_TextureHandle = 0;
	}
	else
	{
	}
}

void
CStarsSection::Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_CloseRequest>& in_rCloseError )
{
	CLog::Print("CStarsSection::Reaction( const CVFSSection_Error<CVFSSection_CloseRequest>& )\n");
	CLog::Print("VFS error %d\n" , in_rCloseError.m_ErrorCode );
	CDemoSection_WorkerInit_Response resp;
	resp.m_bSuccess = false;
	CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID, resp);
}

void
CStarsSection::Reaction( long in_SrcSectionID, const CRenderSection_CreateVertexBuffer_Response& in_rResponse )
{
	CLog::Print("CStarsSection::Reaction( const CRenderSection_CreateVertexBuffer_Response& )\n");
	if(0==in_rResponse.m_VBHandle)
	{
		CLog::Print("VB was not created\n");
		CDemoSection_WorkerInit_Response resp;
		resp.m_bSuccess = false;
		CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID, resp);
	}
	else
	{
		m_VBHandle = in_rResponse.m_VBHandle;
		CheckForInitializationCompletion();
	}
}

void
CStarsSection::Reaction( long in_SrcSectionID, const CRenderSection_CreateTexture_Response& in_rResponse )
{
	CLog::Print("CStarsSection::Reaction( const CRenderSection_CreateTexture_Response& )\n");
	if(0==in_rResponse.m_TexHandle)
	{
		CLog::Print("Texture was not created\n");
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
			rPass.m_bDepthEnabled = false;
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
		CLog::Print("  sending Create PS\n");
		CTCommandSender<CRenderSection_CreatePixelShader_Request>::SendCommand(
			m_RenderSectionID,
			Cmd
		);
	}
}

void
CStarsSection::Reaction( long in_SrcSectionID, const CRenderSection_CreatePixelShader_Response& in_rResponse )
{
	CLog::Print("CStarsSection::Reaction( const CRenderSection_CreatePixelShader_Response& )\n");
	if(0==in_rResponse.m_ShaderHandle)
	{
		CLog::Print("Shader was not created\n");
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
CStarsSection::ParseConfig( const std::vector<unsigned char>& in_rData )
{
	// TODO: parse stars config (get catalog name and texture file name)
	m_CatalogUrl = DATA_LOCATION;
	m_CatalogUrl += "catalog.txt";
	m_TextureUrl = DATA_LOCATION;
	m_TextureUrl += "star.jpg";
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
CStarsSection::ParseStarsCatalog( const std::vector<unsigned char>& in_rData, std::vector<unsigned char>& out_rVBData )
{
	CLog::Print("  CStarsSection::ParseStarsCatalog()\n");
	// TODO: parse stars catalog and construct data for vertex buffer
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
	m_TriangleCount = 2*StarsArray.size();
}

void
CStarsSection::CheckForInitializationCompletion()
{
	CLog::Print("CStarsSection::CheckForInitializationCompletion()\n");
	if(0!=m_VBHandle && 0!=m_PSHandle)
	{
		assert(0!=m_TriangleCount);
		CDemoSection_WorkerInit_Response resp;
		resp.m_bSuccess = true;
		resp.m_Renderables.resize(1);
		CDemoSection_WorkerInit_Response::CRenderable& rRenderable = resp.m_Renderables[0];
		rRenderable.m_InitialPosition = CMatrix();
		rRenderable.m_PrimitiveCount = m_TriangleCount;
		rRenderable.m_PrimitiveType = PRIM_TRIANGLE_LIST;
		rRenderable.m_PSHandle = m_PSHandle;
		rRenderable.m_VBHandle = m_VBHandle;
		rRenderable.m_Type = OT_CENTERED_ON_CAMERA;
		CLog::Print("  sending Response\n");
		CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID, resp);
	}
}


















