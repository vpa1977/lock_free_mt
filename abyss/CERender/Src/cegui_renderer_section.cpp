#include "../Include/cegui_renderer_section.h"

using namespace abyss::cegui;

#include "../../MT_Core/Include/section_registrator.h"



static CTSectionRegistrator<CCEGUI_RenderSection_CLSID,CEGUI_Renderer_Section> g_CEGUI_Renderer_SectionRegistrator(SINGLE_INSTANCE_ONLY);

void Touch_CEGUI()
{
}

CEGUI_Renderer_Section::~CEGUI_Renderer_Section()
{
//	DestroyAllTextures();
}

CEGUI_Renderer_Section::CEGUI_Renderer_Section( long in_SectionID )
: CSection(in_SectionID), m_UpdateCount(0)
{
	REGISTER_REACTION(CEGUI_Renderer_Section, CCommand_SectionStartup);
	// Interface commands
	REGISTER_REACTION(CEGUI_Renderer_Section, CEGUI_Render_InitRequest);
	//REGISTER_REACTION(CEGUI_Renderer_Section, CEGUI_Render_DestroyTexture);
	//REGISTER_REACTION(CEGUI_Renderer_Section, CEGUI_Render_DestroyAllTextures);
	//REGISTER_REACTION(CEGUI_Renderer_Section, CEGUI_Render_ClearRenderList);

	REGISTER_REACTION(CEGUI_Renderer_Section, CRenderSection_UpdateVertexBuffer_Response);
	REGISTER_REACTION(CEGUI_Renderer_Section,CRenderSection_CreatePixelShader_Response);
	REGISTER_REACTION(CEGUI_Renderer_Section,CRenderSection_CreateTexture_Response);
	REGISTER_REACTION(CEGUI_Renderer_Section,CPreloaderSection_DecodeResponse);
	REGISTER_REACTION(CEGUI_Renderer_Section,CRenderSection_CreateVertexBuffer_Response);
	REGISTER_REACTION(CEGUI_Renderer_Section,CRenderSection_NewFrame);

	REGISTER_REACTION(CEGUI_Renderer_Section,CEGUI_UpdateRequest);

	REGISTER_REACTION(CEGUI_Renderer_Section, CVFSSection_OpenResponse); 
	REGISTER_REACTION(CEGUI_Renderer_Section, CVFSSection_Error<CVFSSection_OpenRequest> );
}

void CEGUI_Renderer_Section::ClearRenderList()
{
	TextureData::iterator it = m_Textures.begin();
	while (it != m_Textures.end() ) 
	{
		CEGUI_Texture& tex = it->second;
		tex.m_VertexData.clear();
		tex.m_PrimitiveCount = 0;
		//if (tex.m_VertexBufferID > -1) 
		//{
		//	CTCommandSender<CRenderSection_ReleaseVertexBuffer_Request>::SendCommand(m_RenderSectionID, CRenderSection_ReleaseVertexBuffer_Request(tex.m_VertexBufferID));
		//}
		//tex.m_VertexBufferID = -1;
		tex.m_Dirty = true;
		it++;
	}
}

void CEGUI_Renderer_Section::DestroyAllTextures() 
{
	TextureData::iterator it = m_Textures.begin();
	while (it != m_Textures.end() ) 
	{
		const CEGUI_Texture& tex = it->second;
		if (tex.m_TextureID > -1 ) 
		{
			CRenderSection_ReleaseTexture_Request req(tex.m_TextureID);
			CTCommandSender<CRenderSection_ReleaseTexture_Request>::SendCommand(m_RenderSectionID, req);
		}
		if (tex.m_PixelShaderID > -1 ) 
		{
			CRenderSection_ReleasePixelShader_Request req(tex.m_PixelShaderID);
			CTCommandSender<CRenderSection_ReleasePixelShader_Request>::SendCommand(m_RenderSectionID, req);
		}
		if (tex.m_VertexBufferID > -1 ) 
		{
			CRenderSection_ReleaseVertexBuffer_Request req( tex.m_VertexBufferID);
			CTCommandSender<CRenderSection_ReleaseVertexBuffer_Request>::SendCommand(m_RenderSectionID, req);
		}
		it++;
	}
	m_Textures.clear();

}

void CEGUI_Renderer_Section::DestroyTexture(long textureID) 
{
	assert(m_Textures.find(textureID) != m_Textures.end());
	const CEGUI_Texture& tex = (m_Textures.find(textureID)->second);
	if (tex.m_TextureID > -1 ) 
	{
		CRenderSection_ReleaseTexture_Request req(tex.m_TextureID);
		
		CTCommandSender<CRenderSection_ReleaseTexture_Request>::SendCommand(m_RenderSectionID, req);
	}
	if (tex.m_PixelShaderID > -1 ) 
	{
		CRenderSection_ReleasePixelShader_Request req(tex.m_PixelShaderID);
		
		CTCommandSender<CRenderSection_ReleasePixelShader_Request>::SendCommand(m_RenderSectionID, req);
	}
	if (tex.m_VertexBufferID > -1 ) 
	{
		CRenderSection_ReleaseVertexBuffer_Request req( tex.m_VertexBufferID);
		
		CTCommandSender<CRenderSection_ReleaseVertexBuffer_Request>::SendCommand(m_RenderSectionID, req);

	}
	m_Textures.erase(textureID);

}

void CEGUI_Renderer_Section::Reaction(long in_SrcSectionID, const CEGUI_UpdateRequest& in ) 
{
	if (m_UpdateCount > 0 ) return;
	assert(m_UpdateCount == 0 );

	m_UpdateCount = 0;
	m_RequestSource = in_SrcSectionID;
	if (in.m_bClearRenderList) 
	{
		ClearRenderList();
	}
	if (in.m_bDestroyAllTextures) 
	{
		DestroyAllTextures();		
	}
	if (in.m_DestroyTextureList.size() > 0 ) 
	{
		for (int i = 0 ; i < in.m_DestroyTextureList.size(); i++ ) 
		{
			DestroyTexture( in.m_DestroyTextureList[i]  ) ;
		}
	}
	if (in.m_FileTextures.size() > 0 ) 
	{	
		file_textures::const_iterator it = in.m_FileTextures.begin();
		while (it != in.m_FileTextures.end() ) 
		{
			CEGUI_Texture texture;
			texture.m_Ready = false;
			long texID = it->first;
			texture.m_CEGUI_ID = texID;
			m_Textures[  texID] = texture;

			CVFSSection_OpenRequest req;
			req.m_Magic = it->first;
			req.m_Url = it->second;
			CTCommandSender<CVFSSection_OpenRequest>::SendCommand( m_VFSSectionID, req);
			m_UpdateCount++;
			it++;
		}
		// create file textures.
	}
	if (in.m_MemoryTextures.size() > 0 ) 
	{
		// create memory textures
		memory_textures::const_iterator it = in.m_MemoryTextures.begin();
		while (it != in.m_MemoryTextures.end() ) 
		{
			CreateMemoryTexture(it->second);
			m_UpdateCount++;
			it++;
		}
	}
	// transfer quads into textures.
	//in.m_NormalQueue.
	render_queue::const_iterator it = in.m_NormalQueue.begin();
	while (it!= in.m_NormalQueue.end() ) 
	{
		long in_TexID = it->first;
		const quads& vertexData = it->second;
		TextureData::iterator texIter = m_Textures.find( in_TexID);
		if (texIter == m_Textures.end() ) 
		{
			// texture can be null, but should be present
			assert(0 && "Texture is not found ");
		}
		CEGUI_Texture& next = texIter->second;
		next.m_Dirty = true;
		next.m_VertexData.insert(next.m_VertexData.end(), vertexData.begin(), vertexData.end());
		//assert(next.m_VertexData.size() % ( sizeof(float) * 6 + sizeof(long) ) == 0 );
		it++;
	}
	UpdateRenderBuffers();
	TryRender();
}

void CEGUI_Renderer_Section::UpdateRenderBuffers()
{
	TextureData::iterator it = m_Textures.begin();
	while (it != m_Textures.end() ) 
	{
		CEGUI_Texture& tex = it->second;
		if (tex.m_VertexData.size() ) 
			tex.m_bActive = true;
		else 
			tex.m_bActive = false;
		if (tex.m_Dirty && tex.m_bActive ) 
		{
			if (tex.m_VertexBufferID==-1  )
			{
				CVBFormat fmt;
				fmt.m_XYZRHW = true;
				fmt.m_XYZ = false;
				fmt.m_Diffuse = true;
				fmt.m_Tex0Dimension = CVBFormat::TEXCOORDSET_FLAT;
				CRenderSection_CreateVertexBuffer_Request req(fmt, true, tex.m_CEGUI_ID);
				
				req.m_Data.resize(tex.m_VertexData.size() * sizeof(QuadVertex));
				int i = 0;
				std::list<QuadVertex>::iterator iter = tex.m_VertexData.begin();
				while (iter != tex.m_VertexData.end())
				{
					QuadVertex qvert = (*iter);
					memcpy(&req.m_Data[i*sizeof(QuadVertex)], &qvert, sizeof(QuadVertex));
					iter++;
					i++;
				}
				tex.m_PrimitiveCount =i/3;
				tex.m_Dirty = false;
				CTCommandSender<CRenderSection_CreateVertexBuffer_Request>::SendCommand(m_RenderSectionID, req);
			}
			else
			{
				CRenderSection_UpdateVertexBuffer_Request req;
				req.m_VBHandle = tex.m_VertexBufferID;
				req.m_Data.resize(tex.m_VertexData.size() * sizeof(QuadVertex));
				int i = 0;
				std::list<QuadVertex>::iterator iter = tex.m_VertexData.begin();
				while (iter != tex.m_VertexData.end())
				{
					QuadVertex qvert = (*iter);
					memcpy(&req.m_Data[i*sizeof(QuadVertex)], &qvert, sizeof(QuadVertex));
					iter++;
					i++;
				}
				tex.m_PrimitiveCount =i/3;
				tex.m_Dirty = false;
				CTCommandSender<CRenderSection_UpdateVertexBuffer_Request>::SendCommand(m_RenderSectionID, req);
			}

			m_UpdateCount++;
		}
		it++;
	}
}


void CEGUI_Renderer_Section::Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in)
{
}

void CEGUI_Renderer_Section::Reaction( long in_SrcSectionID, const CEGUI_Render_InitRequest &in)
{
	m_PreloaderSectionID = in.m_PreloaderSectionID;
	m_RenderSectionID = in.m_RenderSectionID;
	m_SrcID  = in.m_SrcID;
	m_VFSSectionID = in.m_VFSSectionID;
	CEGUI_Render_InitResponse resp;
	CTCommandSender<CEGUI_Render_InitResponse>::SendCommand(in_SrcSectionID, resp);
}

void CEGUI_Renderer_Section::Reaction(long in_SrcSectionID, const CRenderSection_NewFrame& in)
{
	CTCommandSender<CRenderSection_NewFrame>::SendCommand(m_SrcID, in);
}
void CEGUI_Renderer_Section::Reaction(long in_SrcSectionID, const CVFSSection_OpenResponse& in_rResponse)
{
	CPreloaderSection_FileDecodeRequest req;
	req.m_Handle = in_rResponse.m_Handle;
	req.m_Magic = in_rResponse.m_Magic;
	req.m_Type = TYPE_TGA;
	req.m_VFSSection = in_SrcSectionID;
	CTCommandSender<CPreloaderSection_FileDecodeRequest>::SendCommand(m_PreloaderSectionID, req);

}

void CEGUI_Renderer_Section::Reaction( long src , const CVFSSection_Error<CVFSSection_OpenRequest>& in ) 
{
	m_UpdateCount--;
	TryRender();
}


void CEGUI_Renderer_Section::CreateMemoryTexture(const CEGUI_Render_CreateTextureFromMemory &in)
{
	
	CEGUI_Texture texture;
	texture.m_Ready = false;
	texture.m_CEGUI_ID = in.m_TextureID;
	m_Textures[  in.m_TextureID] = texture;

	CRenderSection_CreateTexture_Request req( in.m_Width,in.m_Height, in.m_PixelFormat == Texture::PF_RGBA);
	req.m_Magic= in.m_TextureID;
	req.m_Data = in.m_Data;
	req.m_DX = in.m_Width;
	req.m_DY = in.m_Height;
	CTCommandSender<CRenderSection_CreateTexture_Request>::SendCommand(m_RenderSectionID, req);
	

}


void CEGUI_Renderer_Section::Reaction(long in_SrcSectionID , const CRenderSection_UpdateVertexBuffer_Response & in)
{
	m_UpdateCount--;
	TryRender();

	
}
void CEGUI_Renderer_Section::TryRender()
{
	if (m_UpdateCount) return;
	TextureData::iterator it = m_Textures.begin();
	while (it != m_Textures.end() ) 
	{
		CEGUI_Texture& tex = it->second;
		if (tex.m_Ready && tex.m_PrimitiveCount && tex.m_bActive) {
			CRenderSection_RenderVB render(CMatrix(), tex.m_VertexBufferID, tex.m_PixelShaderID, 0, PRIM_TRIANGLE_LIST , tex.m_PrimitiveCount);
			CTCommandSender<CRenderSection_RenderVB>::SendCommand(m_RenderSectionID, render);
			//tex.m_PrimitiveCount = 0;
		}
		it++;
	}
	CTCommandSender<CEGUI_UpdateResponse>::SendCommand(m_RequestSource, CEGUI_UpdateResponse());


}

/////////////////////////////////////
void CEGUI_Renderer_Section::Reaction(long in_SrcSectionID , const CRenderSection_CreatePixelShader_Response & in )
{
	if (m_Textures.find(in.m_Magic) == m_Textures.end())
	{
		assert(0 && "Unknown texture");
		return;
	}
	CEGUI_Texture& tex = (m_Textures.find(in.m_Magic)->second);	
	tex.m_PixelShaderID  = in.m_ShaderHandle;
	tex.m_Ready = true;
	tex.m_Dirty = true;
	m_UpdateCount--;
	TryRender();
}

void CEGUI_Renderer_Section::Reaction(long in_SrcSectionID , const CRenderSection_CreateTexture_Response & in )
{

	TextureData::iterator it = m_Textures.find(in.m_Magic);
	if (it == m_Textures.end() ) {
		assert(0 && "Texture should be present" ) ;
		return;
	}

	CEGUI_Texture& tex = it->second;
	tex.m_TextureID = in.m_TexHandle;
	

	
	CRenderSection_CreatePixelShader_Request Cmd;
	CPixelShader& rShaderDesc = Cmd.m_ShaderDescription;
	rShaderDesc.m_Passes.resize(1);
	{
		// pass 0
		CPixelShaderPass& rPass = rShaderDesc.m_Passes[0];
		rPass.m_CullMode = RCM_CCW;
		
		rPass.m_SrcBlend = RBM_SRCALPHA;
		rPass.m_DstBlend = RBM_INVSRCALPHA;
		//rPass.m_SrcBlend = RBM_ONE;
		//rPass.m_DstBlend = RBM_ZERO;

		
		rPass.m_bLightingEnabled = false;
		rPass.m_TFactor = 0;
		rPass.m_bAlphaTest = true;
		rPass.m_bDepthWrite = true;
		rPass.m_bDepthEnabled = true;
		rPass.m_ZCmpMode = RCMP_LESSEQUAL;

		{
			// pass 0, textures
			rPass.m_Textures.resize(1);
			rPass.m_Textures[0] = tex.m_TextureID;
		}
		rPass.m_Stages.resize(1);
		{
			// pass 0, stage 0
			CPixelShaderPassStage& rStage = rPass.m_Stages[0];
			//
			rStage.m_AlphaArg1 = RTSA_TEXTURE;
			rStage.m_AlphaArg2 = RTSA_DIFFUSE;
			//rStage.m_AlphaOp = RTSO_SELECTARG1;
			rStage.m_AlphaOp = RTSO_MODULATE;
			//
			rStage.m_ColorArg1 = RTSA_TEXTURE;
			rStage.m_ColorArg2 = RTSA_CURRENT;//RTSA_CURRENT;
			//rStage.m_ColorOp = RTSO_MODULATE;
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
	


	Cmd.m_Magic = in.m_Magic;
	CTCommandSender<CRenderSection_CreatePixelShader_Request>::SendCommand(m_RenderSectionID, Cmd);



}

void CEGUI_Renderer_Section::Reaction(long in_SrcSectionID , const CRenderSection_CreateVertexBuffer_Response & in )
{
	if (m_Textures.find(in.m_Magic) == m_Textures.end())
	{
		assert(0 && "unknown texture");
		return;
	}
	CEGUI_Texture& tex = (m_Textures.find(in.m_Magic)->second);	
	tex.m_VertexBufferID= in.m_VBHandle;
	
	m_UpdateCount--;
	TryRender();
}


void CEGUI_Renderer_Section::Reaction(long in_SrcSectionID , const CPreloaderSection_DecodeResponse & in )
{

	CRenderSection_CreateTexture_Request req( in.m_Width,in.m_Height, true);
	req.m_Magic= in.m_Magic;
	req.m_Data = in.m_ARGBData;
	req.m_DX = in.m_Width;
	req.m_DY = in.m_Height;
	CTCommandSender<CRenderSection_CreateTexture_Request>::SendCommand(m_RenderSectionID, req);
}
/*
void CEGUI_Renderer_Section::Reaction( long in_SrcSectionID, const CEGUI_Render_AddQuad &in)
{
	
	float z = 1-in.z;
	
	QuadVertex v[6];
	Rect position = in.m_DestRect;
	position.offset(Point(-0.5f, -0.5f));
	unsigned long topLeftCol  = in.m_Colours.d_top_left.getARGB();
	unsigned long topRightCol = in.m_Colours.d_top_right.getARGB();
	unsigned long bottomLeftCol =in.m_Colours.d_bottom_left.getARGB();
	unsigned long bottomRightCol = in.m_Colours.d_bottom_right.getARGB();



	v[0].x = position.d_left;
	v[0].y = position.d_top;
	v[0].z = z;
	v[0].rhw = 1.0f;
	v[0].diffuse = topLeftCol;
	v[0].tu1 = in.m_TextureRect.d_left;
	v[0].tv1 = in.m_TextureRect.d_top;


        // top-left to bottom-right diagonal
	if (in.m_QuadSplitMode == TopLeftToBottomRight)
    {
       v[1].x = position.d_right;
       v[1].y = position.d_bottom;
       v[1].z = z;
       v[1].rhw = 1.0f;
       v[1].diffuse = bottomRightCol;
       v[1].tu1 = in.m_TextureRect.d_right;
       v[1].tv1 = in.m_TextureRect.d_bottom;
    }
    // bottom-left to top-right diagonal
    else
    {
        v[1].x = position.d_right;
        v[1].y = position.d_top;
        v[1].z = z;
        v[1].rhw = 1.0f;
        v[1].diffuse = topRightCol;
        v[1].tu1 = in.m_TextureRect.d_right;
        v[1].tv1 = in.m_TextureRect.d_top;
    }

			// setup Vertex 3...
	v[2].x = position.d_left;
	v[2].y = position.d_bottom;
	v[2].z = z;
	v[2].rhw = 1.0f;
	v[2].diffuse = bottomLeftCol;
	v[2].tu1 = in.m_TextureRect.d_left;
	v[2].tv1 =in.m_TextureRect.d_bottom;


	v[3].x = position.d_right;
	v[3].y = position.d_top;
	v[3].z = z;
	v[3].rhw = 1.0f;
	v[3].diffuse = topRightCol;
	v[3].tu1 = in.m_TextureRect.d_right;
	v[3].tv1 = in.m_TextureRect.d_top;


			// setup Vertex 5...
		v[4].x = position.d_right;
		v[4].y = position.d_bottom;
		v[4].z = z;
		v[4].rhw = 1.0f;
		v[4].diffuse = bottomRightCol;
		v[4].tu1 = in.m_TextureRect.d_right;
		v[4].tv1 = in.m_TextureRect.d_bottom;
		//++buffmem;

		// setup Vertex 6...

        // top-left to bottom-right diagonal
        if (in.m_QuadSplitMode == TopLeftToBottomRight)
        {
           v[5].x = position.d_left;
           v[5].y = position.d_top;
           v[5].z = z;
           v[5].rhw = 1.0f;
           v[5].diffuse = topLeftCol;
           v[5].tu1 = in.m_TextureRect.d_left;
           v[5].tv1 = in.m_TextureRect.d_top;
        }
        // bottom-left to top-right diagonal
        else
        {
            v[5].x = position.d_left;
            v[5].y = position.d_bottom;
            v[5].z = z;
            v[5].rhw = 1.0f;
            v[5].diffuse = bottomLeftCol;
            v[5].tu1 =in.m_TextureRect.d_left;
            v[5].tv1 = in.m_TextureRect.d_bottom;
        }


		TextureData::iterator it = m_Textures.find(in.m_TextureID);
		if (it != m_Textures.end())
		{

			{
				it->second.m_PrimitiveCount = 0;
				it->second.m_Dirty = true;
				for (int i = 0 ; i< 6 ; i ++ ) 
				{
					it->second.m_VertexData.push_back(v[i]);
				}
			}


		}
		else
		{
			// This texture is not created. 
			assert(0);
		}	

}
*/
/*
void CEGUI_Renderer_Section::Reaction( long in_SrcSectionID,const  CEGUI_Render_DoRender &in)
{
	const std::vector<CEGUI_Render_AddQuad>& quads = *in.m_Quads.get();
	for (int i = 0 ; i < quads.size() ; i ++ ) 
	{
		const CEGUI_Render_AddQuad& quad = quads[i];
		Reaction(in_SrcSectionID, quad);
	}
	if (m_UpdateCount > 0 ) 
	{
		// do nothing - we still have not rendered previous frame
		return;
	}
	TextureData::iterator it = m_Textures.begin();
	while (it != m_Textures.end() ) 
	{
		CEGUI_Texture& tex = it->second;
		if (tex.m_Ready && tex.m_Dirty && (tex.m_VertexData.size() > 0)) {
	
			if (tex.m_VertexBufferID < 0) 
			{
				//CTCommandSender<CRenderSection_ReleaseVertexBuffer_Request>::SendCommand(m_RenderSectionID, CRenderSection_ReleaseVertexBuffer_Request(tex.m_VertexBufferID));
			
				CVBFormat fmt;
				fmt.m_XYZRHW = true;
				fmt.m_XYZ = false;
				fmt.m_Diffuse = true;
				fmt.m_Tex0Dimension = CVBFormat::TEXCOORDSET_FLAT;
				CRenderSection_CreateVertexBuffer_Request req(fmt, true, tex.m_CEGUI_ID);
				
				req.m_Data.resize(tex.m_VertexData.size() * sizeof(QuadVertex));
				int i = 0;
				std::list<QuadVertex>::iterator iter = tex.m_VertexData.begin();
				while (iter != tex.m_VertexData.end())
				{
					QuadVertex qvert = (*iter);
					memcpy(&req.m_Data[i*sizeof(QuadVertex)], &qvert, sizeof(QuadVertex));
					iter++;
					i++;
				}
				tex.m_PrimitiveCount =i/3;
				tex.m_Dirty = false;
				CTCommandSender<CRenderSection_CreateVertexBuffer_Request>::SendCommand(m_RenderSectionID, req);
			}
			else
			{

				CRenderSection_UpdateVertexBuffer_Request req;
				req.m_VBHandle = tex.m_VertexBufferID;
				req.m_Data.resize(tex.m_VertexData.size() * sizeof(QuadVertex));
				int i = 0;
				std::list<QuadVertex>::iterator iter = tex.m_VertexData.begin();
				while (iter != tex.m_VertexData.end())
				{
					QuadVertex qvert = (*iter);
					memcpy(&req.m_Data[i*sizeof(QuadVertex)], &qvert, sizeof(QuadVertex));
					iter++;
					i++;
				}
				tex.m_PrimitiveCount =i/3;
				tex.m_Dirty = false;
				CTCommandSender<CRenderSection_UpdateVertexBuffer_Request>::SendCommand(m_RenderSectionID, req);
			}
			m_UpdateCount++;
		}
		it++;
	}
	if (!m_UpdateCount) 
	{
			TextureData::iterator it = m_Textures.begin();
			while (it != m_Textures.end() ) 
			{
				CEGUI_Texture& tex = it->second;
				if (tex.m_Ready && tex.m_PrimitiveCount) {
					CRenderSection_RenderVB render(CMatrix(), tex.m_VertexBufferID, tex.m_PixelShaderID, 0, PRIM_TRIANGLE_LIST , tex.m_PrimitiveCount);
					CTCommandSender<CRenderSection_RenderVB>::SendCommand(m_RenderSectionID, render);
					//tex.m_PrimitiveCount = 0;
				}
				it++;
			}
			CTCommandSender<CRenderSection_PresentAndClear>::SendCommand(m_RenderSectionID, CRenderSection_PresentAndClear(0));
	}

}
*/
/*
void CEGUI_Renderer_Section::Reaction( long in_SrcSectionID, const CEGUI_Render_CreateTexture &in)
{
	CEGUI_Texture texture;
	texture.m_Ready = false;
	texture.m_CEGUI_ID = in.m_TextureID;
	if (m_Textures.find(in.m_TextureID) != m_Textures.end() ) 
	{
		CEGUI_Render_DestroyTexture destr;
		destr.m_TextureID = in.m_TextureID;
		Reaction(in_SrcSectionID, destr);
	}
	m_Textures[  in.m_TextureID] = texture;
	CPreloaderSection_DecodeRequest req;
	req.m_Data = in.m_Data;
	req.m_Type = in.m_Type;
	req.m_Magic = in.m_TextureID;
	CTCommandSender<CPreloaderSection_DecodeRequest>::SendCommand(m_PreloaderSectionID, req);
}
*/
