#ifndef CEGUI_RENDERER_INTERFACE_HPP

#define CEGUI_RENDERER_INTERFACE_HPP

#include "CEGUIBase.h"
#include "CEGUIRenderer.h"
#include "CEGUITexture.h"
#include "../../MT_Core/Include/command.h"
#include <string>
#include <vector>

using namespace CEGUI;

#define CCEGUI_RenderSection_CLSID 3234656


struct QuadVertex {
	float x, y, z, rhw;		//!< The transformed position for the vertex.
	long diffuse;			//!< colour of the vertex
	float tu1, tv1;			//!< texture coordinates
};




struct CEGUI_Texture
{
	CEGUI_Texture() : m_Ready(false), m_CEGUI_ID(-1), m_TextureID(-1), 
		m_PixelShaderID(-1),m_VertexBufferID(-1), m_PrimitiveCount(0), m_bActive(false)
	{
	}
	bool m_Ready;
	bool m_Dirty;
	bool m_bActive;
	long m_CEGUI_ID;
	long m_TextureID;
	long m_PixelShaderID;
	long m_VertexBufferID;
	long m_PrimitiveCount;
	std::list<QuadVertex> m_VertexData;
	
	long m_DirectVertexBufferID;
	long m_DirectPrimitiveCount;
	std::list<QuadVertex> m_DirectVertexData;

};

typedef std::vector<QuadVertex> quads;
typedef std::map< long ,  quads > render_queue;
typedef std::map< long , std::string> file_textures;



// no response commands 
struct CEGUI_Render_InitRequest : public CCommand
{
	long m_RenderSectionID;
	long m_PreloaderSectionID;
	long m_VFSSectionID;
	long m_SrcID;
};

struct CEGUI_Render_InitResponse : public CCommand
{
	Rect m_DisplayArea;
	unsigned int m_MaxTextureSize;
};


struct CEGUI_Render_AddQuad : public CCommand
{
	bool m_bFirst; // add quad to the top of the queue
	Rect m_DestRect;
	float z;
	long m_TextureID;
	Rect m_TextureRect;
	ColourRect m_Colours;
	QuadSplitMode m_QuadSplitMode;
};
// create texture from image

struct CEGUI_Render_CreateTexture : public CCommand
{
	long m_TextureID;
	unsigned char m_Type;
	std::vector<unsigned char> m_Data;
};

struct CEGUI_Render_CreateTextureFromMemory : public CCommand
{
	std::vector<unsigned char> m_Data;
	long m_TextureID;
	unsigned int m_Width;
	unsigned int m_Height;
	Texture::PixelFormat m_PixelFormat;
};

typedef std::map< long , CEGUI_Render_CreateTextureFromMemory> memory_textures;

struct CEGUI_Render_DestroyTexture : public CCommand
{
	long m_TextureID;
};

struct CEGUI_Render_DestroyAllTextures : public CCommand
{
};

struct CEGUI_Render_ClearRenderList : public CCommand
{
};


struct CEGUI_UpdateRequest : public CCommand
{
	CEGUI_UpdateRequest() : m_bClearRenderList(false), m_bDestroyAllTextures(false)
	{
	}
	bool m_bClearRenderList;
	bool m_bDestroyAllTextures;
	std::vector<long> m_DestroyTextureList;
	render_queue m_NormalQueue;
	render_queue m_ImmediateQueue;
	file_textures m_FileTextures;
	memory_textures m_MemoryTextures;
};

struct CEGUI_UpdateResponse : public CCommand
{
};




struct CEGUI_Render_DoRender : public CCommand
{
	boost::shared_ptr< std::vector<CEGUI_Render_AddQuad> > m_Quads;
};





#endif