#include "../Include/object_section.h"
#include "../Include/vertex_loader.h"
#include "../../MT_Core/Include/section_registrator.h"
#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/log.h"

#include "../../Render_DX8/Include/algebra.h"

static CTSectionRegistrator<CObjectSection_CLSID,CObjectSection>
g_ObjectSectionRegistrator(MULTIPLE_INSTANCES_ALLOWED );


#define DATA_LOCATION "/data/"

CObjectSection::CObjectSection( long in_SectionID ) :
	CSection( in_SectionID )
		
{

	RegisterReaction( GetID((CCommand_SectionStartup*)NULL),	CTConform<CObjectSection,CCommand_SectionStartup>::Conform );
	RegisterReaction( GetID((CCommand_SetTimer_Response*)NULL),	CTConform<CObjectSection,CCommand_SetTimer_Response>::Conform );

	RegisterReaction( GetID((CCommand_Timer_Event*)NULL),	CTConform<CObjectSection,CCommand_Timer_Event>::Conform );
	RegisterReaction( GetID((CCommand_LaunchSectionResult*)NULL),	CTConform<CObjectSection,CCommand_LaunchSectionResult>::Conform );	



	RegisterReaction( GetID((CDemoSection_WorkerInit_Request*)NULL),	CTConform<CObjectSection,CDemoSection_WorkerInit_Request>::Conform );
	RegisterReaction( GetID((CDemoSection_WorkerInit_Response*)NULL),	CTConform<CObjectSection,CDemoSection_WorkerInit_Response>::Conform );
	RegisterReaction( GetID((CDemoSection_WorkerShutdown_Request*)NULL),	CTConform<CObjectSection,CDemoSection_WorkerShutdown_Request>::Conform );
	RegisterReaction( GetID((CDemoSection_WorkerUpdate*)NULL),	CTConform<CObjectSection,CDemoSection_WorkerUpdate>::Conform );


	RegisterReaction( GetID((CPreloaderSection_DecodeResponse*)NULL),	CTConform<CObjectSection,CPreloaderSection_DecodeResponse>::Conform );
	RegisterReaction( GetID((CPreloaderSection_DecodeError*)NULL),	CTConform<CObjectSection,CPreloaderSection_DecodeError>::Conform );

	RegisterReaction( GetID((CVFSSection_OpenResponse*)NULL),	CTConform<CObjectSection,CVFSSection_OpenResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_Error<CVFSSection_OpenRequest>*)NULL),	CTConform<CObjectSection,CVFSSection_Error<CVFSSection_OpenRequest> >::Conform );


	RegisterReaction( GetID((CVFSSection_ReadResponse*)NULL),	CTConform<CObjectSection,CVFSSection_ReadResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_Error<CVFSSection_ReadRequest>*)NULL),	CTConform<CObjectSection,CVFSSection_Error<CVFSSection_ReadRequest> >::Conform );

	
	RegisterReaction( GetID((CVFSSection_CloseResponse*)NULL),	CTConform<CObjectSection,CVFSSection_CloseResponse>::Conform );
	RegisterReaction( GetID((CVFSSection_Error<CVFSSection_CloseRequest>*)NULL),	CTConform<CObjectSection,CVFSSection_Error<CVFSSection_CloseRequest> >::Conform );

	RegisterReaction( GetID((CRenderSection_CreateVertexBuffer_Response*)NULL),	CTConform<CObjectSection,CRenderSection_CreateVertexBuffer_Response>::Conform );
	RegisterReaction( GetID((CRenderSection_CreateTexture_Response*)NULL),	CTConform<CObjectSection,CRenderSection_CreateTexture_Response>::Conform );
	RegisterReaction( GetID((CRenderSection_CreatePixelShader_Response*)NULL),	CTConform<CObjectSection,CRenderSection_CreatePixelShader_Response>::Conform );

}

CObjectSection::~CObjectSection()
{
}

/*

1. polu4it' komandu CDemoSection_WorkerInit_Request
2. pro4itat' config fail
3. sozdat` object-i
4. poslat` otvet CDemoSection_WorkerInit_Response

5.1. Ustanovit` timer
5.2. Podozhdat' timer event
5.3. pros4itat` novie pozicii objectov
5.4. poslat` CDemoSection_WorkerUpdate
5.5. goto 5.1.


+ Movement 
+ Resource Release
+ Resource release on exit
+ Satellite support
*/

void CObjectSection::Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )
{
	CLog::Print("CObjectSection::Reaction( const CCommand_SectionStartup& )\n");
	CLog::Print("  sectionID = %ld\n",GetThisID());
	CLog::TraceEntry("void CObjectSection::Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )");
	m_bDisk = false;
	m_Renderable.m_PSHandle = 0;
	m_Renderable.m_VBHandle = 0;
	m_ElapsedTime = 0;
	m_fPoint =(float) -CONST_PI;
	m_fSelfPoint =(float) -CONST_PI;
	m_SatelliteSectionID.clear();
	m_iSattelites = 0;
	m_iSatellitesLaunched = 0;
	CLog::TraceExit("void CObjectSection::Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )");

}
// ================================================================================================================
// system
void CObjectSection::Reaction( long in_SrcSectionID, const CCommand_SetTimer_Response& )
{
	//CLog::TraceEntry("void CObjectSection::Reaction( long in_SrcSectionID, const CCommand_SetTimer_Response& )");
	//CLog::TraceExit("void CObjectSection::Reaction( long in_SrcSectionID, const CCommand_SetTimer_Response& )");
}
// ================================================================================================================
void CObjectSection::ComposeWorkerUpdate(CMatrix& result) 
{
		// x**2 + 1/2 * y** 2 = R**2
		CMatrix nextMatrix;
			
		m_fPoint += m_fVelocity;
		if (m_fPoint > 2*CONST_PI ) 
		{
			m_fPoint =(float) -2*CONST_PI;
		}

		m_fSelfPoint += m_fSelfSpeed;
		if (m_fSelfPoint > 2*CONST_PI ) 
		{
			m_fSelfPoint=(float) -2*CONST_PI;
		}

		CMatrix selfRotate;
		selfRotate.ConstructRotationY(m_fSelfPoint);

		CMatrix selfAngle;
		selfAngle.ConstructRotationZ(m_fAngle);


		nextMatrix.ConstructScaling(CVector(m_fSize, m_fSize, m_fSize ));
		CMatrix nextMatrixRadius; 
		nextMatrixRadius.ConstructTranslation( CVector(m_fRadius , 0 , 0 ));

		CMatrix nextMatrixRotation;
		nextMatrixRotation.ConstructRotationY(m_fPoint );
		
		
		result =nextMatrix * selfRotate * selfAngle* nextMatrixRadius * nextMatrixRotation; //* nextMatrixRotation1;
	
		
}
void CObjectSection::Reaction( long in_SrcSectionID, const CCommand_Timer_Event& event)
{
//	CLog::Print("CObjectSection(%ld):  timer event\n",GetThisID());
	if (event.m_MagicID == this->GetThisID() ) 
	{
		// send update command. 
		m_ElapsedTime ++;
		CDemoSection_WorkerUpdate workerUpdate;
//
		///
		CMatrix newMatrix;
		ComposeWorkerUpdate(newMatrix );
		workerUpdate.m_Positions.push_back(newMatrix);
		std::vector<CMatrix> source;
		std::vector<CMatrix> dest;
		SatPositions::iterator it = m_SatPositions.begin();
		while (it != m_SatPositions.end() ) 
		{
			std::vector<CMatrix>& vec  = it->second;
			for (int i = 0 ; i < vec.size() ; i ++ ) source.push_back(vec[i]);
			it++;
		}
		UpdatePositions(dest, source);
		for (int i = 0 ; i < dest.size() ; i ++ ) workerUpdate.m_Positions.push_back(dest[i]);;
		///

//		CLog::Print("CObjectSection(%ld):  sending update\n",GetThisID());
		CTCommandSender<CDemoSection_WorkerUpdate>::SendCommand(m_MainSectionID , workerUpdate );
		
		

		CCommand_SetTimer_Request req(this->GetThisID(), 50*1000);
		
//		CLog::Print("CObjectSection(%ld):  new timer\n",GetThisID());
		CTCommandSender<CCommand_SetTimer_Request>::SendCommand( SECTION_ID_SYSTEM, req );
//		CLog::Print("CObjectSection(%ld):  new timer set\n",GetThisID());


	}
}


void CObjectSection::UpdatePositions(std::vector<CMatrix>& positions, const std::vector<CMatrix>& source ) 
{
	if (source.size() > 0 ) 
	{
		for (int i = 0 ; i < source.size() ; i ++ ) 
		{
			CMatrix nextMatrixRadius; 
			nextMatrixRadius.ConstructTranslation( CVector(m_fRadius , 0 , 0 ));

			CMatrix nextMatrixRotation;
			nextMatrixRotation.ConstructRotationY(m_fPoint );
			CMatrix matrix = source[i];
			matrix = matrix * nextMatrixRadius * nextMatrixRotation;
			positions.push_back(matrix);
		}
	}
}


void CObjectSection::Reaction( long in_SrcSectionID, const CDemoSection_WorkerUpdate& update )
{
	/*CDemoSection_WorkerUpdate workerUpdate;
	CMatrix newMatrix;
	ComposeWorkerUpdate(newMatrix );
	workerUpdate.m_Positions.push_back(newMatrix);
	UpdatePositions(workerUpdate.m_Positions , update.m_Positions);

	CLog::Println("Update Celestial object with %d renderables " , workerUpdate.m_Positions.size());
	CTCommandSender<CDemoSection_WorkerUpdate>::SendCommand(m_MainSectionID , workerUpdate );
	*/
	m_SatPositions[in_SrcSectionID] = update.m_Positions;
}

// ================================================================================================================
// demo section
void CObjectSection::Reaction( long in_SrcSectionID, const CDemoSection_WorkerInit_Request& in_rWorkerInit_Request)
{
	CLog::TraceEntry("CObjectSection::Reaction( long in_SrcSectionID, const CDemoSection_WorkerInit_Request& )");
	CLog::Print("CObjectSection::Reaction( const CDemoSection_WorkerInit_Request& )\n");
	CLog::Print("  config file: '%s' (%ld)\n",in_rWorkerInit_Request.m_ConfigFileForThisSection.c_str(),GetThisID());
	
	m_VfsSectionID = in_rWorkerInit_Request.m_VFSSectionID;
	m_RenderSectionID = in_rWorkerInit_Request.m_RenderSectionID;
	m_MainSectionID = in_SrcSectionID;
	//in_rWorkerInit_Request.m_ConfigFileForThisSection;
	m_PreloaderSectionID = in_rWorkerInit_Request.m_PreloaderSectionID;
//	m_Slave = in_rWorkerInit_Request.m_Slave;
	CVFSSection_OpenRequest openRequest;
	openRequest.m_Url = DATA_LOCATION;
	openRequest.m_Url += in_rWorkerInit_Request.m_ConfigFileForThisSection;

	m_ConfigUrl = openRequest.m_Url;

	CTCommandSender<CVFSSection_OpenRequest>::SendCommand(m_VfsSectionID,  openRequest);
	CLog::TraceExit("void CObjectSection::Reaction( long in_SrcSectionID, const CDemoSection_WorkerInit_Request& )");
}
// ================================================================================================================

// preloader
void CObjectSection::Reaction( long in_SrcSectionID, const CPreloaderSection_DecodeError& in_rDecodeError)
{
	CLog::Println("Image decoding error %s" , in_rDecodeError.m_Message);
	CDemoSection_WorkerInit_Response resp;
	resp.m_bSuccess = false;
	CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID, resp);
}

void CObjectSection::Reaction( long in_SrcSectionID, const CPreloaderSection_DecodeResponse& in_rDecodeResponse )
{
	CRenderSection_CreateTexture_Request Cmd(in_rDecodeResponse.m_Width,in_rDecodeResponse.m_Height,true);
	Cmd.m_Data = in_rDecodeResponse.m_ARGBData;

	//#define DUMMY_SIZE 8
	//CRenderSection_CreateTexture_Request Cmd(DUMMY_SIZE,DUMMY_SIZE,true);
	//std::vector<unsigned char> buf;
	//buf.resize(DUMMY_SIZE*DUMMY_SIZE*sizeof(unsigned long));
	//Cmd.m_Data = buf;

	CTCommandSender<CRenderSection_CreateTexture_Request>::SendCommand(
		m_RenderSectionID,
		Cmd
	);
	
	CVFSSection_CloseRequest cmd ; 
	cmd.m_Handle = 	in_rDecodeResponse.m_Handle;
	CTCommandSender<CVFSSection_CloseRequest>::SendCommand(m_VfsSectionID, cmd);

}
// ================================================================================================================
// vfs
void CObjectSection::Reaction( long in_SrcSectionID, const CVFSSection_OpenResponse& in_rOpenResponse)
{
	CLog::TraceEntry("void CObjectSection::Reaction( long in_SrcSectionID, const CVFSSection_OpenResponse& in_rOpenResponse)");
	if (in_rOpenResponse.m_Url == m_ConfigUrl ) 
	{
		m_ConfigHandle = in_rOpenResponse.m_Handle;

		// handle config url
		CVFSSection_ReadRequest req;
		req.m_Handle = in_rOpenResponse.m_Handle;
		req.m_Offset = 0;
		req.m_Size = -1;
		CTCommandSender<CVFSSection_ReadRequest>::SendCommand(m_VfsSectionID , req);

	}
	else
	if (in_rOpenResponse.m_Url == m_ModelUrl ) 
	{

		m_ModelHandle = in_rOpenResponse.m_Handle;
		// handle model url
		CVFSSection_ReadRequest req;
		req.m_Handle = in_rOpenResponse.m_Handle;
		req.m_Offset = 0;
		req.m_Size = -1;
		CTCommandSender<CVFSSection_ReadRequest>::SendCommand(m_VfsSectionID , req);

	}
	else
	if (in_rOpenResponse.m_Url == m_TextureUrl ) 
	{
		// handle texture open 
		m_TextureHandle = in_rOpenResponse.m_Handle;
		CPreloaderSection_FileDecodeRequest req;
		req.m_Handle = m_TextureHandle;
		req.m_Type = TYPE_JPG;
		req.m_VFSSection = m_VfsSectionID;
		CTCommandSender<CPreloaderSection_FileDecodeRequest>::SendCommand(m_PreloaderSectionID, req);
		//CPreloaderSection_DecodeResponse resp;
		//Reaction(0,resp );
	}
	CLog::TraceExit("void CObjectSection::Reaction( long in_SrcSectionID, const CVFSSection_OpenResponse& in_rOpenResponse)");
}


// ================================================================================================================
void CObjectSection::Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_OpenRequest>& in_rOpenError)
{
	
	CLog::Println("VFS error %d" , in_rOpenError.m_ErrorCode );
	CDemoSection_WorkerInit_Response resp;
	resp.m_bSuccess = false;
	CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID, resp);

}
// ================================================================================================================

void CObjectSection::Reaction( long in_SrcSectionID, const CVFSSection_ReadResponse& in_rReadResponse)
{
	CLog::TraceEntry("void CObjectSection::Reaction( long in_SrcSectionID, const CVFSSection_ReadResponse& in_rReadResponse)");
	assert(in_rReadResponse.m_Size);
	
	long handle = in_rReadResponse.m_Handle;

	if (handle == m_ConfigHandle) 
	{
		
		ParseConifg(in_rReadResponse);
		//m_TextureUrl = "/data/tex.tga";
		//m_ModelUrl = "/data/sphere.txt";
		CVFSSection_OpenRequest reqModel;
		reqModel.m_Url = m_ModelUrl;
		CTCommandSender<CVFSSection_OpenRequest>::SendCommand(m_VfsSectionID, reqModel);
		CVFSSection_OpenRequest reqTexture;
		reqTexture.m_Url = m_TextureUrl;
		CTCommandSender<CVFSSection_OpenRequest>::SendCommand(m_VfsSectionID, reqTexture);
	}
	else
	if (handle == m_ModelHandle ) 
	{
		ParseModel(in_rReadResponse);	
	}

	CVFSSection_CloseRequest cmd ; 
	cmd.m_Handle = 	in_rReadResponse.m_Handle;
	CTCommandSender<CVFSSection_CloseRequest>::SendCommand(m_VfsSectionID, cmd);
	

	CLog::TraceExit("void CObjectSection::Reaction( long in_SrcSectionID, const CVFSSection_ReadResponse& in_rReadResponse)");
}

// ================================================================================================================
void CObjectSection::Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_ReadRequest>& in_rError )
{
	CLog::Println("VFS error %d" , in_rError.m_ErrorCode);
	CDemoSection_WorkerInit_Response resp;
	resp.m_bSuccess = false;
	CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID, resp);
}

// ================================================================================================================
void CObjectSection::Reaction( long in_SrcSectionID, const CVFSSection_CloseResponse& in_rCloseResponse)
{
	CLog::Println("VFS File closed: %d", in_rCloseResponse.m_Handle);
}
// ================================================================================================================
void CObjectSection::Reaction( long in_SrcSectionID, const CVFSSection_Error<CVFSSection_CloseRequest>& in_rError )
{
	CLog::Println("VFS error %d" , in_rError.m_ErrorCode);
	CDemoSection_WorkerInit_Response resp;
	resp.m_bSuccess = false;
	CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID, resp);
}

// ================================================================================================================
// render
void CObjectSection::Reaction( long in_SrcSectionID, const CRenderSection_CreateVertexBuffer_Response& in_rCmd )
{
	CLog::Print("DemoSection::Reaction( const CRenderSection_CreateVertexBuffer_Response& )\n");
	CLog::Print("  VB handle = %lu\n",in_rCmd.m_VBHandle);
	assert( in_rCmd.m_VBHandle > 0 );
	m_Renderable.m_VBHandle = in_rCmd.m_VBHandle;
	
	assertReadiness();


}
// ================================================================================================================
void CObjectSection::Reaction( long in_SrcSectionID, const CRenderSection_CreateTexture_Response& in_rCmd )
{
	CLog::Print("void CObjectSection::Reaction( long in_SrcSectionID, const CRenderSection_CreateTexture_Response& )\n");
	CLog::Print("  ThisID() = %ld\n",GetThisID());
	
	long texHandle = in_rCmd.m_TexHandle;

	m_TexHandle = texHandle; // just in case

	CLog::Print("  texHandle = %lu\n",texHandle);
	//
	CRenderSection_CreatePixelShader_Request Cmd;
	CPixelShader& rShaderDesc = Cmd.m_ShaderDescription;
	rShaderDesc.m_Passes.resize(1);
	{
		// pass 0
		CPixelShaderPass& rPass = rShaderDesc.m_Passes[0];
		rPass.m_CullMode = m_bDisk ? RCM_NONE : RCM_CCW;
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
			rPass.m_Textures[0] = texHandle;
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
			rStage.m_ColorArg2 = RTSA_CURRENT;//RTSA_CURRENT;cn ytn
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
		m_RenderSectionID,
		Cmd
	);

}
// ================================================================================================================
void CObjectSection::Reaction( long in_SrcSectionID, const CRenderSection_CreatePixelShader_Response& in_rCmd)
{
	CLog::Print("void CObjectSection::Reaction( long in_SrcSectionID, const CRenderSection_CreatePixelShader_Response& )");
	m_Renderable.m_PSHandle = in_rCmd.m_ShaderHandle;
	CLog::Print("  m_PSHandle = %lu\n",m_Renderable.m_PSHandle);

	assertReadiness();
}

// ================================================================================================================

struct Vertex
{
	float x;
	float y;
	float z;
	long alpha;
	float u;
	float w;
};

void CObjectSection::ParseModel(const CVFSSection_ReadResponse& in_rReadResponse) 
{
	// TODO !!!!!
	CVertexLoader loader;
	if ( loader.Load(in_rReadResponse) )
	{
			
		if (m_bDisk ) 
		{
			CVBFormat format;
			format.m_Diffuse = true;
			format.m_XYZ =true ;
			//format.m_Normal = true;
			format.m_Tex0Dimension =CVBFormat::TEXCOORDSET_FLAT;
			float alpha = 0;
			
			std::vector<char> m_Data;
			long count = 0;
			long index = 0;
			while (alpha < CONST_2PI ) 
			{
				float xsmall = m_fSize * sin(alpha);
				float ysmall = (m_fSize) * cos(alpha);

				float xlarge = (0.55+m_fSize) * sin(alpha);
				float ylarge = (0.45+m_fSize) * cos(alpha);

				alpha += 0.3;

				float xsmall1 = m_fSize * sin(alpha);
				float ysmall1 = (m_fSize) * cos(alpha);

				float xlarge1 = (0.55+m_fSize) * sin(alpha);
				float ylarge1 = (0.45+m_fSize) * cos(alpha);
			
				
				long alphaColor = 0xFFFFFF;
				{

					Vertex v;
					v.x =xsmall;
					v.y = 0;
					v.z = ysmall;
					v.alpha = alphaColor;
					v.u = 0;
					v.w = 0;
					m_Data.resize(m_Data.size() + sizeof(Vertex));
					memcpy( (char*)&m_Data[index], &v, sizeof(Vertex));
					index += sizeof(Vertex);

					/*m_Data.push_back(xsmall);
					m_Data.push_back(0);
					m_Data.push_back(ysmall);
					m_Data.push_back(alphaColor);
					m_Data.push_back(0);
					m_Data.push_back(0);*/


					v.x = xlarge;
					v.y = 0;
					v.z = ylarge;
					v.alpha = alphaColor;
					v.u = 0;
					v.w = 1;
					m_Data.resize(m_Data.size() + sizeof(Vertex));
					memcpy( (char*)&m_Data[index], &v, sizeof(Vertex));
					index += sizeof(Vertex);

					/*m_Data.push_back(xlarge);
					m_Data.push_back(0);
					m_Data.push_back(ylarge);
					m_Data.push_back(alphaColor);
					m_Data.push_back(63);
					m_Data.push_back(1);
*/

					v.x = xlarge1;
					v.y = 0;
					v.z = ylarge1;
					v.alpha = alphaColor;
					v.u = 0;
					v.w = 1;
					m_Data.resize(m_Data.size() + sizeof(Vertex));
					memcpy( (char*)&m_Data[index], &v, sizeof(Vertex));
					index += sizeof(Vertex);


					/*m_Data.push_back(xlarge1);
					m_Data.push_back(0);
					m_Data.push_back(ylarge1);
	 				m_Data.push_back(alphaColor);
					m_Data.push_back(63);
					m_Data.push_back(1);*/

					v.x =xsmall;
					v.y = 0;
					v.z = ysmall;
					v.alpha = alphaColor;
					v.u = 0;
					v.w = 0;
					m_Data.resize(m_Data.size() + sizeof(Vertex));
					memcpy( (char*)&m_Data[index], &v, sizeof(Vertex));
					index += sizeof(Vertex);

/*					m_Data.push_back(xsmall);
					m_Data.push_back(0);
					m_Data.push_back(ysmall);
					m_Data.push_back(alphaColor);
					m_Data.push_back(0);
					m_Data.push_back(0);*/

					v.x =xsmall1;
					v.y = 0;
					v.z = ysmall1;
					v.alpha = alphaColor;
					v.u = 0;
					v.w = 0;
					m_Data.resize(m_Data.size() + sizeof(Vertex));
					memcpy( (char*)&m_Data[index], &v, sizeof(Vertex));
					index += sizeof(Vertex);

/*
	 				m_Data.push_back(xsmall1);
					m_Data.push_back(0);
					m_Data.push_back(ysmall1);
					m_Data.push_back(alphaColor);
					m_Data.push_back(0);
					m_Data.push_back(0);
*/

					v.x =xlarge1;
					v.y = 0;
					v.z = ylarge1;
					v.alpha = alphaColor;
					v.u = 0;
					v.w = 1;
					m_Data.resize(m_Data.size() + sizeof(Vertex));
					memcpy( (char*)&m_Data[index], &v, sizeof(Vertex));
					index += sizeof(Vertex);

				/*
					m_Data.push_back(xlarge1);
					m_Data.push_back(0);
					m_Data.push_back(ylarge1);
					m_Data.push_back(alphaColor);
					m_Data.push_back(63);
					m_Data.push_back(1);

*/
					count+=2;
				}

				

			}
			
			CRenderSection_CreateVertexBuffer_Request Cmd(format, false );
			m_Renderable.m_PrimitiveType = PRIM_TRIANGLE_LIST;
			m_Renderable.m_PrimitiveCount = count;
			Cmd.m_Data.resize(m_Data.size());
			memcpy( (char*)&Cmd.m_Data[0] , (char*)&m_Data[0], m_Data.size() );
			CTCommandSender<CRenderSection_CreateVertexBuffer_Request>::SendCommand(
				m_RenderSectionID,
				Cmd
			);


		}
		else
		{

		
			CRenderSection_CreateVertexBuffer_Request Cmd(loader.GetVBFormat(), false );
			Cmd.m_Data = loader.GetData();
			m_Renderable.m_PrimitiveType = loader.GetPrimitiveType();
			m_Renderable.m_PrimitiveCount = loader.GetNPrimitives();

			CTCommandSender<CRenderSection_CreateVertexBuffer_Request>::SendCommand(
				m_RenderSectionID,
				Cmd
			);
		}
	}
	else
	{
			CLog::Println("Init model error");
			CDemoSection_WorkerInit_Response resp;
			resp.m_bSuccess = false;
			CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID, resp);
	}
}

// ================================================================================================================
static int counter = 0;

void CObjectSection::ParseConifg(const CVFSSection_ReadResponse& in_rReadResponse) 
{
	char* buffer;
	buffer = (char*)malloc (in_rReadResponse.m_Size+1);
	memset(buffer, 0, in_rReadResponse.m_Size + 1 ) ;
	memcpy(buffer, &(in_rReadResponse.m_pData[0]), in_rReadResponse.m_Size);

	std::vector<std::string> vLines;
	
	char* nextLine = strtok(buffer, "\r\n");
	while (nextLine ) 
	{
		vLines.push_back(nextLine);
		nextLine = strtok(NULL, "\r\n" );
	}
	free(buffer);

	//extract property name  | property value pair
	for (unsigned int i = 0 ; i < vLines.size() ; i ++ ) 
	{
		std::string propertyName = strtok((char*)vLines[i].data() , "=" );
		std::string propertyValue;
		const char * next = strtok(NULL, "=");
		
		  

		if (next ) 
		{
			propertyValue = next;
			// say hi.
			if (propertyName == "model" ) 
			{
				// handle model
				m_ModelUrl = DATA_LOCATION + propertyValue;
			}
			else
			if (propertyName == "texture" ) 
			{
				// handle texture
				m_TextureUrl = DATA_LOCATION + propertyValue;

			}
			else
			if (propertyName == "radius" ) 
			{
				m_fRadius = (float)atof(propertyValue.data());
			}
			else
			if (propertyName == "velocity") 
			{
				m_fVelocity = (float)atof(propertyValue.data());
			}
			else
			if (propertyName == "size") 
			{
				m_fSize = (float)atof(propertyValue.data());
			}
			else
			if (propertyName == "satellite" ) 
			{
					m_iSattelites ++;
					
					long satteliteSectionMagic = 1233455 +counter;
					counter++;
					m_sSatelliteSectionConfig[satteliteSectionMagic] = propertyValue;
					CTCommandSender< CCommand_LaunchSection >::SendCommand(
							SECTION_ID_SYSTEM,
							CCommand_LaunchSection( CObjectSection_CLSID, satteliteSectionMagic )
						);
			
			
			}
			else
			if (propertyName == "selfSpeed") 
			{
				m_fSelfSpeed = (float)atof(propertyValue.data());
			}
			else
			if (propertyName == "angle" ) 
			{
				m_fAngle = (float)atof(propertyValue.data());
			}
			else 
			if (propertyName == "point" ) 
			{
				m_fPoint = (float)atof(propertyValue.data());
			}
			else 
			if (propertyName == "disk" ) 
			{
				m_bDisk= true;
			}

			else
			{
				CLog::Println("Property not recognized ");
			}

		}

	}
}

// ================================================================================================================


void CObjectSection::assertReadiness(void)
{
	CLog::Print("Asserting ready");
	if (m_Renderable.m_PSHandle && m_Renderable.m_VBHandle  ) 
	{
		CLog::Print("PS & VB handle ready");
		if (m_iSattelites != m_iSatellitesLaunched )
		{
				CLog::Print("Satellites are not");
				return;
		}
		// gtg with ready update
		CLog::Print("Preparing ready response");		
		CDemoSection_WorkerInit_Response response;
		response.m_Renderables.push_back(m_Renderable);
		ComposeWorkerUpdate(response.m_Renderables[0].m_InitialPosition);

		if (m_iSattelites) {
			std::vector<CMatrix> source;
			std::vector<CMatrix> dest;
			for (int i = 0 ; i < m_SatRenderable.size() ; i ++ ) 
			{
				source.push_back(m_SatRenderable[i].m_InitialPosition);
			}
			UpdatePositions(dest, source);
			for (int i = 0 ; i< m_SatRenderable.size() ; i ++ ) 
			{
				m_SatRenderable[i].m_InitialPosition = dest[i];
				response.m_Renderables.push_back(m_SatRenderable[i]);

			}
		}
		CLog::Println("Starting Up Celestial object with %d renderables " , response.m_Renderables.size());
		
		response.m_bSuccess =true;
		CTCommandSender<CDemoSection_WorkerInit_Response>::SendCommand(m_MainSectionID , response );
			CCommand_SetTimer_Request req(this->GetThisID(), 1);
				CTCommandSender<CCommand_SetTimer_Request>::SendCommand(
					SECTION_ID_SYSTEM,
					req
			);
	}
}
// === satellite and shutdown
void CObjectSection::Reaction( long in_SrcSectionID, const CCommand_LaunchSectionResult& in_rResult )
{
	assert(in_rResult.m_ErrorCode == SLR_OK);
	CDemoSection_WorkerInit_Request request(m_sSatelliteSectionConfig[in_rResult.m_Magic],m_VfsSectionID , m_PreloaderSectionID, m_RenderSectionID);
//	request.m_Slave = false;
	CTCommandSender<CDemoSection_WorkerInit_Request>::SendCommand(in_rResult.m_NewSectionID , request );
}

void CObjectSection::Reaction( long in_SrcSectionID, const CDemoSection_WorkerInit_Response& resp )
{
	if (resp.m_bSuccess ) 
	{
		m_SatelliteSectionID.push_back(in_SrcSectionID);
		std::vector<CMatrix> iniPos;
		for (int i = 0 ; i < resp.m_Renderables.size() ; i ++ ) 
		{
			m_SatRenderable.push_back(resp.m_Renderables[i]) ;
			iniPos.push_back(resp.m_Renderables[i].m_InitialPosition);
		}

		m_SatPositions[in_SrcSectionID] = iniPos;

		
	
		m_iSatellitesLaunched ++;
	}
	else
	{
		assert(0);
	}
	assertReadiness();
}
void CObjectSection::Reaction( long in_SrcSectionID, const CDemoSection_WorkerShutdown_Request& req)
{
	if (m_iSattelites > 0) 
	{
		for (int i = 0 ; i < m_SatelliteSectionID.size() ; i ++ ) 
			CTCommandSender<CDemoSection_WorkerShutdown_Request>::SendCommand(m_SatelliteSectionID[i], req);
	}
	CDemoSection_WorkerShutdown_Response resp;
	
	CTCommandSender<CDemoSection_WorkerShutdown_Response>::SendCommand(m_MainSectionID, resp);
	Quit();
}
