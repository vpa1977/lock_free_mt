#ifndef CRENDERER_HPP
#define CRENDERER_HPP

#include "../../MT_Core/Include/log.h"
#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../MT_Core/Include/command_sender.h"

#include "CEGUI.h"
#include "CEGUIBase.h"
#include "CEGUIRenderer.h"
#include "CEGUITexture.h"
#include <d3d8.h>
#include <list>
#include <vector>
#include <set>
#include <map>
#include "cegui_renderer_interface.h"
#include "cegui_renderer_section.h"
#include "texture.h"

#ifdef DIRECTX81_GUIRENDERER_EXPORTS
#define DIRECTX81_GUIRENDERER_API __declspec(dllexport)
#else
#define DIRECTX81_GUIRENDERER_API __declspec(dllimport)
#endif

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


using namespace abyss::cegui;

// This is a public interface to a renderer object 
// due to single threaded nature of CEGUI we are 
// instantiating it in the user section which handles
// main loop of the application. 
namespace CEGUI
{
/*************************************************************************
	Forward refs
*************************************************************************/


/*!
\brief
	Renderer class to interface with Microsoft DirectX 8.1
*/
class   CObjectRendererProxy :
		public Renderer

{
public:

	
	// add's a quad to the list to be rendered
	virtual	void	addQuad(const Rect& dest_rect, float z, const Texture* tex, const Rect& texture_rect, const ColourRect& colours, QuadSplitMode quad_split_mode);

	// perform final rendering for all queued renderable quads.
	virtual	void	doRender(void);

	// clear the queue
	virtual	void	clearRenderList(void);


	/*!
	\brief
		Enable or disable the queueing of quads from this point on.

		This only affects queueing.  If queueing is turned off, any calls to addQuad will cause the quad to be rendered directly.  Note that
		disabling queueing will not cause currently queued quads to be rendered, nor is the queue cleared - at any time the queue can still
		be drawn by calling doRender, and the list can be cleared by calling clearRenderList.  Re-enabling the queue causes subsequent quads
		to be added as if queueing had never been disabled.

	\param setting
		true to enable queueing, or false to disable queueing (see notes above).

	\return
		Nothing
	*/
	virtual void	setQueueingEnabled(bool setting)		{d_queueing = setting;}


	// create an empty texture
	virtual	Texture*	createTexture(void);

	// create a texture and load it with the specified file.
	virtual	Texture*	createTexture(const String& filename, const String& resourceGroup);

	// create a texture and set it to the specified size
	virtual	Texture*	createTexture(float size);

	// destroy the given texture
	virtual	void		destroyTexture(Texture* texture);

	// destroy all textures still active
	virtual void		destroyAllTextures(void);

	/*!
	\brief
		Return whether queueing is enabled.

	\return
		true if queueing is enabled, false if queueing is disabled.
	*/
	virtual bool	isQueueingEnabled(void) const	{return d_queueing;}


	/*!
	\brief
	Return the current width of the display in pixels

	\return
	float value equal to the current width of the display in pixels.
	*/
	virtual float	getWidth(void) const		{return d_display_area.getWidth();}


	/*!
	\brief
	Return the current height of the display in pixels

	\return
	float value equal to the current height of the display in pixels.
	*/
	virtual float	getHeight(void) const		{return d_display_area.getHeight();}


	/*!
	\brief
	Return the size of the display in pixels

	\return
	Size object describing the dimensions of the current display.
	*/
	virtual Size	getSize(void) const			{return d_display_area.getSize();}


	/*!
	\brief
	Return a Rect describing the screen

	\return
	A Rect object that describes the screen area.  Typically, the top-left values are always 0, and the size of the area described is
	equal to the screen resolution.
	*/
	virtual Rect	getRect(void) const			{return d_display_area;}


	/*!
	\brief
		Return the maximum texture size available

	\return
		Size of the maximum supported texture in pixels (textures are always assumed to be square)
	*/
	virtual	uint	getMaxTextureSize(void) const		{return d_maxTextureSize;}


	/*!
	\brief
		Return the horizontal display resolution dpi

	\return
		horizontal resolution of the display in dpi.
	*/
	virtual	uint	getHorzScreenDPI(void) const	{return 96;}


	/*!
	\brief
		Return the vertical display resolution dpi

	\return
		vertical resolution of the display in dpi.
	*/
	virtual	uint	getVertScreenDPI(void) const	{return 96;}

public:

	void CreateTexure(long id, const char* file_name)
	{
//		CTCommandSender<CEGUI_Render_CreateTexture>::SendCommand(GetProxyID() , in);
		m_FileTextures[ id ] = std::string(file_name);
	}
	void CreateTextureMemory(const CEGUI_Render_CreateTextureFromMemory& in )
	{
//		CTCommandSender<CEGUI_Render_CreateTextureFromMemory>::SendCommand(GetProxyID() , in);
		m_MemoryTextures[ in.m_TextureID ] = in;
	}

	void SetRenderArea(Rect dispArea)
	{
		
		d_display_area.d_bottom = dispArea.d_bottom;
		d_display_area.d_top = dispArea.d_top;
		d_display_area.d_left = dispArea.d_left;
		d_display_area.d_right = dispArea.d_right;
		d_maxTextureSize = 65535*1000;
	}

	void ComposeRequest(CEGUI_UpdateRequest& in_Request)
	{
		
		CEGUI::System::getSingleton().renderGUI();
		in_Request.m_bClearRenderList = m_bClearRenderList;
		m_bClearRenderList = false;
		in_Request.m_bDestroyAllTextures = m_bDestroyAllTextures;
		m_bDestroyAllTextures = false;
		in_Request.m_DestroyTextureList = m_DestroyTexturesList;
		m_DestroyTexturesList.clear();
		in_Request.m_FileTextures = m_FileTextures;
		m_FileTextures.clear();
		in_Request.m_MemoryTextures = m_MemoryTextures;
		m_MemoryTextures.clear();
		in_Request.m_NormalQueue = m_NormalQueue;
		in_Request.m_ImmediateQueue = m_OneFrameQueue;
		m_OneFrameQueue.clear();
		m_NormalQueue.clear();
	}

	CObjectRendererProxy()
	{
		m_bDestroyAllTextures = false;
		m_bClearRenderList =false;
		d_identifierString = "DX8-CEGUI";

	}
	virtual ~CObjectRendererProxy()
	{
		// test
		m_Textures.clear();
	}


private:
	bool m_bClearRenderList;
	bool m_bDestroyAllTextures;
	std::vector<long> m_DestroyTexturesList;
	render_queue m_NormalQueue;
	render_queue m_OneFrameQueue;
	file_textures m_FileTextures;
	memory_textures m_MemoryTextures;
	std::vector<CObjectTexture*> m_Textures;
	//boost::shared_ptr< std::vector<CEGUI_Render_AddQuad> > m_Quads;
private:
	Rect d_display_area;
	bool d_queueing;
	uint d_maxTextureSize;
};

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _DirectX81GUIRenderer_h_
