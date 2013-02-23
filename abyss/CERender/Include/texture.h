#ifndef OBJECT_TEXTURE_H
#define OBJECT_TEXTURE_H

#include "CEGUIBase.h"
#include "CEGUIRenderer.h"
#include "CEGUITexture.h"
#include <d3d8.h>
#include <list>
#ifdef DIRECTX81_GUIRENDERER_EXPORTS
#define DIRECTX81_GUIRENDERER_API __declspec(dllexport)
#else
#define DIRECTX81_GUIRENDERER_API __declspec(dllimport)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
class  CObjectRendererProxy;
/*!
\brief
	Texture class that is created by CObjectRenderer objects
*/
class  CObjectTexture : public  Texture
{
private:


	/*************************************************************************
		Construction & Destruction (by Renderer object only)
	*************************************************************************/
	

public:
	CObjectTexture(Renderer* owner);

	virtual ~CObjectTexture(void)
	{
	}
	/*!
	\brief
		Returns the current pixel width of the texture

	\return
		ushort value that is the current width of the texture in pixels
	*/
	virtual	ushort	getWidth(void) const		{return d_width;}


	/*!
	\brief
		Returns the current pixel height of the texture

	\return
		ushort value that is the current height of the texture in pixels
	*/
	virtual	ushort	getHeight(void) const		{return d_height;}

    virtual	ushort getOriginalWidth(void) const { return d_orgWidth; }
    virtual	ushort getOriginalHeight(void) const { return d_orgHeight; }

	/*!
	\brief
		Loads the specified image file into the texture.  The texture is resized as required to hold the image.

	\param filename
		The filename of the image file that is to be loaded into the texture

    \param resourceGroup
        Resource group identifier passed to the resource provider.

	\return
		Nothing.
	*/
	virtual void	loadFromFile(const String& filename, const String& resourceGroup);


	/*!
	\brief
		Loads (copies) an image in memory into the texture.  The texture is resized as required to hold the image.

	\param buffPtr
		Pointer to the buffer containing the image data

	\param buffWidth
		Width of the buffer (in pixels as specified by \a pixelFormat )

	\param buffHeight
		Height of the buffer (in pixels as specified by \a pixelFormat )

    \param pixelFormat
        PixelFormat value describing the format contained in \a buffPtr

	\return
		Nothing.
	*/
	virtual void	loadFromMemory(const void* buffPtr, uint buffWidth, uint buffHeight, PixelFormat pixelFormat);


public:
	CObjectTexture& operator =(const CObjectTexture& in)
	{
		d_width = in.d_width;
		d_height = in.d_height;
		d_orgHeight = in.d_orgHeight;
		d_orgWidth = in.d_width;
		d_id = in.d_id;
		return (*this);

	}

	long GetID() { return d_id; }

private:
	uint d_width;
	uint d_height;
	uint d_orgHeight;
	uint d_orgWidth;
	long d_id;

private:
	CObjectRendererProxy* m_pRender;


};

} // End of  CEGUI namespace section


#endif	// end of guard _texture_h_
