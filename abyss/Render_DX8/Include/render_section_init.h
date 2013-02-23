#ifndef RENDER_SECTION_INIT_HPP_INCLUDED
#define RENDER_SECTION_INIT_HPP_INCLUDED

#include "../../MT_Core/Include/command.h"
#include "render_info.h"
#include <d3d8.h>

struct CRenderSection_InitRender : public CCommand
{
	CRenderSection_InitRender(
		unsigned long in_DX=0, unsigned long in_DY=0,
		bool in_bWindowed=true,
		RENDER_DISPLAY_MODE_FORMAT in_FullscreenFormat=RDMF_UNKNOWN,
		bool in_bDepthBufferRequired=true,
		bool in_bStencilBufferRequired=false,
		unsigned long in_FullscreenRefreshRate=60,
		bool in_bVSync=false
	) :
		m_DX(in_DX),
		m_DY(in_DY),
		m_bWindowed(in_bWindowed),
		m_FullscreenFormat(in_FullscreenFormat),
		m_bDepthBufferRequired(in_bDepthBufferRequired),
		m_bStencilBufferRequired(in_bStencilBufferRequired),
		m_FullscreenRefreshRate(in_FullscreenRefreshRate),
		m_bVSync(in_bVSync)
	{}
	unsigned long m_DX;
	unsigned long m_DY;
	bool m_bWindowed;
	RENDER_DISPLAY_MODE_FORMAT m_FullscreenFormat;
	bool m_bDepthBufferRequired;
	bool m_bStencilBufferRequired;
	unsigned long m_FullscreenRefreshRate;
	bool m_bVSync;
};

struct CRenderSection_CloseRender : public CCommand {};

struct CRenderSection_CloseRenderResult : public CCommand
{
	CRenderSection_CloseRenderResult(bool in_Ok) :
		m_Ok(in_Ok)
	{}
	bool m_Ok;
};

#endif