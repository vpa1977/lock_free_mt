#ifndef RENDERCRITICAL_SECTION_INTERFACE_HPP_INCLUDED
#define RENDERCRITICAL_SECTION_INTERFACE_HPP_INCLUDED

#include "../../MT_Core/Include/command.h"
#include "render_info.h"
#include "render_section_init_result.h"
#include "r8_vertex_process_flags.h"
#include <d3d8.h>

#define CRenderCriticalSection_CLSID 9890023

// =================================================================================================================

struct CRenderCriticalSection_BindToWindow : public CCommand
{
	CRenderCriticalSection_BindToWindow( HWND in_HWnd ) :
		m_HWnd(in_HWnd)
	{}
	HWND m_HWnd;
};

struct CRenderCriticalSection_BoundToWindow : public CCommand
{
	CRenderCriticalSection_BoundToWindow( HWND in_HWnd ) :
		m_HWnd(in_HWnd)
	{}
	HWND m_HWnd;
};

struct CRenderCriticalSection_UnbindAndDie : public CCommand {};

// =================================================================================================================

// Sent by render to render_critical,
// to indicate that window was closed and because of that complete cleanup must be done.
struct CRenderCriticalSection_WindowClosed : public CCommand
{
	CRenderCriticalSection_WindowClosed( HWND in_HWnd ) :
		m_HWnd(in_HWnd)
	{}
	HWND m_HWnd;
};

// Response to previous command.
// Sent by render_critical to render.
struct CRenderCriticalSection_WindowClosedCleanupComplete : public CCommand
{
	CRenderCriticalSection_WindowClosedCleanupComplete( HWND in_HWnd ) :
		m_HWnd(in_HWnd)
	{}
	HWND m_HWnd;
};

// ===========================================================================================

struct CRenderCriticalSection_Get3DEnvironmentInfo : public CCommand {};

struct CRenderCriticalSection_3DEnvironmentInfo : public CCommand
{
	CRenderCriticalSection_3DEnvironmentInfo() :
		m_bError(false)
		,m_CanRenderWindowed(false)
		,m_16Depth(false)
		,m_16DepthStencil(false)
		,m_32Depth(false)
		,m_32DepthStencil(false)
	{}
	bool m_bError;
	//
	std::vector< CRenderDisplayMode > m_DisplayModes;
	//
	bool m_CanRenderWindowed;
	CRenderDisplayMode m_CurrentDisplayMode;
	//
	bool m_16Depth;
	bool m_16DepthStencil;
	bool m_32Depth;
	bool m_32DepthStencil;
};

// ===========================================================================================

struct CRenderCriticalSection_InitRenderResult : public CCommand
{
	CRenderCriticalSection_InitRenderResult(
		INIT_RENDER_RESULT in_Result,
		IDirect3D8* in_pD3D,
		IDirect3DDevice8* in_pDevice,
		RENDER_VERTEX_PROCESS_TYPE in_VertexProcessingType,
		D3DFORMAT in_RenderMode
	) :
		m_Result(in_Result),
		m_pD3D(in_pD3D),
		m_pDevice(in_pDevice),
		m_VertexProcessingType(in_VertexProcessingType),
		m_RenderMode(in_RenderMode)
	{}
	INIT_RENDER_RESULT			m_Result;
	IDirect3D8*					m_pD3D;
	IDirect3DDevice8*			m_pDevice;
	RENDER_VERTEX_PROCESS_TYPE	m_VertexProcessingType;
	D3DFORMAT					m_RenderMode;
};

// ===========================================================================================

struct CRenderCriticalSection_RestoreDevice : public CCommand {};

struct CRenderCriticalSection_RestoreDeviceComplete : public CCommand
{
	CRenderCriticalSection_RestoreDeviceComplete(bool in_bSuccess) :
		m_bSuccess(in_bSuccess)
	{}
	bool m_bSuccess;
};

// ===========================================================================================

#endif
