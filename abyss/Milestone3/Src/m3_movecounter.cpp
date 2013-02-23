#include "../Include/m3_movecounter.h"
#include "../Include/render_proxy.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>
#include <stdio.h>


void
CMoveCounter::vStartPreload()
{
	PreloadComplete(true);
}

void
CMoveCounter::vUpdateBeforeChildren( unsigned long in_Time )
{
	//CRenderProxy::getInstance().RenderText(0, 36,"0", 2 , 0xFF00FFFF);
	
	//CRenderProxy::getInstance().RenderText(0,  0,"!\"#$%&'()*+,-./", 2 , 0xFF00FFFF);
	//CRenderProxy::getInstance().RenderText(0, 30,"0123456789", 2 , 0xFF00FFFF);
	//CRenderProxy::getInstance().RenderText(0, 60,":;<=>?@ABCDEFGHI", 2 , 0xFF00FFFF);
	//CRenderProxy::getInstance().RenderText(0, 90,"JKLMNOPQRSTUVWXYZ", 2 , 0xFF00FFFF);
	//CRenderProxy::getInstance().RenderText(0,120,"[\]^_`", 2 , 0xFF00FFFF);
	//CRenderProxy::getInstance().RenderText(0,150,"abcdefghijklmnopqrstuvwxyz", 2 , 0xFF00FFFF);
	//CRenderProxy::getInstance().RenderText(0,180,"{|}~", 2 , 0xFF00FFFF);
	
	char buffer[256];
	if( !m_bComplete ) 
	{
		sprintf(buffer, "Move: %d" , m_iCount);
		CRenderProxy::getInstance().RenderText(180,20,buffer, 2 , 0xFF00FFFF);
	}
	else
	{
		sprintf(buffer, "Completed in  %d moves !" , m_iCount);
		CRenderProxy::getInstance().RenderText(90,20,buffer, 2 , 0xFF00FFFF);
	}
}
