#include "../Include/texture.h"
#include "../Include/renderer.h"
#include "CEGUIExceptions.h"
#include "CEGUISystem.h"

using namespace CEGUI;


#include "../include/cegui_renderer_interface.h"
#include "../../preloader/include/section_preloader_interface.h"

static long TEX_ID_COUNT = 0;

CObjectTexture::CObjectTexture(Renderer* owner)
: Texture(owner)
{
	d_id = TEX_ID_COUNT;
	TEX_ID_COUNT ++;
	d_width = 0;
	d_height = 0;
	d_orgHeight = 0;
	d_orgWidth = 0;
}


void CObjectTexture::loadFromFile(const String& filename, const String& resourceGroup)
{	
	RawDataContainer texFile;
	System::getSingleton().getResourceProvider()->loadRawDataContainer(filename, texFile, resourceGroup);
    const char* buffer = (const char*) texFile.getDataPtr();

	//// ONLY TGA LOAD CAN BE USED
	short width, height;
	buffer += sizeof(byte) + 1 + 1+9;
	// Read the width, height and bits per pixel (16, 24 or 32)
	memcpy(&width, buffer, sizeof(short));
	buffer += sizeof(short);
	memcpy(&height, buffer, sizeof(short));
	buffer += sizeof(short);
	d_height= height;
	d_width = width;
	d_orgHeight = d_height;
	d_orgWidth = d_width;

/*	CEGUI_Render_CreateTexture creTex;
	creTex.m_Data.resize(texFile.getSize());
	memcpy(& creTex.m_Data[0] , (const char*) texFile.getDataPtr() , texFile.getSize());
	creTex.m_Type = TYPE_TGA;
	creTex.m_TextureID = d_id;
	((CObjectRendererProxy*)getRenderer())->CreateTexure(creTex);*/
	System::getSingleton().getResourceProvider()->unloadRawDataContainer(texFile);
	CObjectRendererProxy * ptr= ((CObjectRendererProxy*)getRenderer());
	ptr->CreateTexure(d_id,filename.c_str());

}

void CObjectTexture::loadFromMemory(const void * buffPtr, uint buffWidth, uint buffHeight, PixelFormat pfmt)
{
	d_height= buffHeight;
	d_width = buffWidth;
	d_orgHeight = d_height;
	d_orgWidth = d_width;

	CEGUI_Render_CreateTextureFromMemory creTex;
	creTex.m_Height = buffHeight;
	creTex.m_Width = buffWidth;
	creTex.m_PixelFormat = pfmt;
	creTex.m_Data.resize( buffWidth * buffHeight * sizeof(unsigned long));
	creTex.m_TextureID = d_id;
	memcpy(&creTex.m_Data[0], buffPtr, creTex.m_Data.size());
	((CObjectRendererProxy*)getRenderer())->CreateTextureMemory(creTex);
}

