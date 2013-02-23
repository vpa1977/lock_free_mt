#ifndef RENDER_SECTION_INTERFACE_HPP_INCLUDED
#define RENDER_SECTION_INTERFACE_HPP_INCLUDED

#include "../../MT_Core/Include/command.h"
#include "../../Common/Include/vb_format.h"
#include "../../Common/Include/primitive_types.h"
#include "r8_font_info.h"
#include "algebra.h"
#include "render_info.h"
#include "render_section_init_result.h"
#include "render_pixel_shader.h"
#include "../../Objects/Include/shadow.h"

#include <windows.h>
#include <vector>
#include <memory>
#include <string>

#define CRenderSection_CLSID 312879345

// =================================================================================================================

struct CRenderSection_BindToWindow : public CCommand
{
	CRenderSection_BindToWindow( long in_WindowSectionID, HWND in_HWnd ) :
		m_WindowSectionID(in_WindowSectionID)
		,m_HWnd(in_HWnd)
	{}
	long m_WindowSectionID;
	HWND m_HWnd;
};

struct CRenderSection_BoundToWindow : public CCommand
{
	CRenderSection_BoundToWindow( HWND in_HWnd ) :
		m_HWnd(in_HWnd)
	{}
	HWND m_HWnd;
};

// =================================================================================================================

struct CRenderSection_Get3DEnvironmentInfo : public CCommand {};

struct CRenderSection_3DEnvironmentInfo : public CCommand
{
	CRenderSection_3DEnvironmentInfo() :
		m_bError(true)
	{}
	bool m_bError;
	std::vector< CRenderDisplayMode > m_DisplayModes;
	bool m_CanRenderWindowed;
	CRenderDisplayMode m_CurrentDisplayMode;
	bool m_16Depth;
	bool m_16DepthStencil;
	bool m_32Depth;
	bool m_32DepthStencil;
};

// =================================================================================================================

// Sent by user to render,
// to indicate that window was closed and because of that complete cleanup must be done.
struct CRenderSection_WindowClosed : public CCommand
{
	CRenderSection_WindowClosed( HWND in_HWnd ) : m_HWnd(in_HWnd) {}
	HWND m_HWnd;
};

// Response to previous command.
// Sent by render to user.
struct CRenderSection_WindowClosedCleanupComplete : public CCommand
{
	CRenderSection_WindowClosedCleanupComplete( HWND in_HWnd ) : m_HWnd(in_HWnd) {}
	HWND m_HWnd;
};

// =================================================================================================================

struct CRenderSection_InitRenderResult : public CCommand
{
	CRenderSection_InitRenderResult( INIT_RENDER_RESULT in_Result ) :
		m_Result(in_Result)
	{}
	INIT_RENDER_RESULT	m_Result;
};

// =================================================================================================================

struct CRenderSection_PresentAndClear : public CCommand
{
	CRenderSection_PresentAndClear( unsigned long in_Color=0x00000000 ) :
		m_Color(in_Color)
	{}
	unsigned long m_Color;
};

struct CRenderSection_NewFrame : public CCommand {};

// =================================================================================================================

struct CRenderSection_SetCamera : public CCommand
{
	CRenderSection_SetCamera(
		const CVector& in_Org, const CVector& in_Dir, const CVector& in_Right,
		float in_FovX, float in_FovY,
		float in_Znear, float in_Zfar
	) :
		m_Org(in_Org), m_Dir(in_Dir), m_Right(in_Right),
		m_FovX(in_FovX), m_FovY(in_FovY),
		m_Znear(in_Znear), m_Zfar(in_Zfar)
	{}
	CVector m_Org;
	CVector m_Dir;
	CVector m_Right;
	float m_FovX;
	float m_FovY;
	float m_Znear;
	float m_Zfar;
};

// =================================================================================================================

struct CRenderSection_SetAmbient : public CCommand
{
	CRenderSection_SetAmbient( unsigned long in_Ambient ) : m_Ambient(in_Ambient) {}
	unsigned long m_Ambient;
};

struct CRenderSection_SetDirectionalLight : public CCommand
{
	CRenderSection_SetDirectionalLight( const CVector& in_Dir, float in_R, float in_G, float in_B ) :
		m_Dir(in_Dir),
		m_R(in_R),
		m_G(in_G),
		m_B(in_B)
	{}
	CVector m_Dir;
	float m_R,m_G,m_B;
};

struct CRenderSection_SetPointLight : public CCommand
{
	CRenderSection_SetPointLight( const CVector& in_Org, float in_R, float in_G, float in_B ) :
		m_Org(in_Org),
		m_R(in_R),
		m_G(in_G),
		m_B(in_B)
	{}
	CVector m_Org;
	float m_R,m_G,m_B;
};

// =================================================================================================================

// =================================================================================================================

struct CRenderSection_CreateVertexBuffer_Request : public CCommand
{
	CRenderSection_CreateVertexBuffer_Request(
		const CVBFormat& in_VBFormat
		,bool in_bDynamic
		,unsigned long in_Magic=0
	) :
		m_VBFormat(in_VBFormat), m_bDynamic(in_bDynamic), m_Magic(in_Magic)
	{}
	std::vector<unsigned char> m_Data;
	CVBFormat m_VBFormat;
	bool m_bDynamic;
	unsigned long m_Magic;
};

struct CRenderSection_CreateVertexBuffer_Response : public CCommand
{
	CRenderSection_CreateVertexBuffer_Response(
		unsigned long in_VBHandle
		,unsigned long in_Magic
	) :
		m_VBHandle(in_VBHandle), m_Magic(in_Magic)
	{}
	unsigned long m_VBHandle; // m_VBHandle=0 - error, VB was not created.
	unsigned long m_Magic;
};

struct CRenderSection_UpdateVertexBuffer_Request : public CCommand
{
	unsigned long m_VBHandle;
	std::vector<unsigned char> m_Data;
};

struct CRenderSection_UpdateVertexBuffer_Response : public CCommand
{
	CRenderSection_UpdateVertexBuffer_Response(
		bool in_bSuccess
		,unsigned long in_VBHandle
	) :
		m_bSuccess(in_bSuccess)
		,m_VBHandle(in_VBHandle)
	{}
	bool m_bSuccess;
	unsigned long m_VBHandle;
};

struct CRenderSection_ReleaseVertexBuffer_Request : public CCommand
{
	CRenderSection_ReleaseVertexBuffer_Request( unsigned long in_VBHandle ) :
		m_VBHandle(in_VBHandle)
	{}
	unsigned long m_VBHandle;
};

// =================================================================================================================

struct CRenderSection_CreateTexture_Request : public CCommand
{
	// Texture ATM is:
	// A8R8G8B8 - with alpha
	// x8R8G8B8 - without alpha
	CRenderSection_CreateTexture_Request(
		unsigned long in_DX
		,unsigned long in_DY
		,bool in_HasAlpha
		,unsigned long in_Magic=0
	) :
		m_DX(in_DX)
		,m_DY(in_DY)
		,m_HasAlpha(in_HasAlpha)
		,m_Magic(in_Magic)
	{}
	std::vector<unsigned char> m_Data;
	unsigned long m_DX;
	unsigned long m_DY;
	bool m_HasAlpha;
	unsigned long m_Magic;
};

struct CRenderSection_CreateTexture_Response : public CCommand
{
	CRenderSection_CreateTexture_Response(
		unsigned long in_TexHandle
		,unsigned long in_Magic
	) :
		m_TexHandle(in_TexHandle)
		,m_Magic(in_Magic)
	{}
	unsigned long m_TexHandle; // m_TexHandle=0 - error, Texture was not created.
	unsigned long m_Magic;
};

struct CRenderSection_ReleaseTexture_Request : public CCommand
{
	CRenderSection_ReleaseTexture_Request( unsigned long in_TexHandle ) :
		m_TexHandle(in_TexHandle)
	{}
	unsigned long m_TexHandle;
};

// =================================================================================================================

struct CRenderSection_CreatePixelShader_Request : public CCommand
{
	CRenderSection_CreatePixelShader_Request() {}
	CRenderSection_CreatePixelShader_Request(
		const CPixelShader& in_ShaderDescription
		,unsigned long in_Magic=0
	) :
		m_ShaderDescription(in_ShaderDescription)
		,m_Magic(in_Magic)
	{}
	CPixelShader m_ShaderDescription;
	unsigned long m_Magic;
};

struct CRenderSection_CreatePixelShader_Response : public CCommand
{
	CRenderSection_CreatePixelShader_Response(
		unsigned long in_ShaderHandle
		,unsigned long in_Magic
	) :
		m_ShaderHandle(in_ShaderHandle)
		,m_Magic(in_Magic)
	{}
	unsigned long m_ShaderHandle; // m_ShaderHandle=0 - error, Shader was not created.
	unsigned long m_Magic;
};

struct CRenderSection_ReleasePixelShader_Request : public CCommand
{
	CRenderSection_ReleasePixelShader_Request( unsigned long in_ShaderHandle ) :
		m_ShaderHandle(in_ShaderHandle)
	{}
	unsigned long m_ShaderHandle;
};

// =================================================================================================================

struct CRenderSection_CreateFont_Request : public CCommand
{
	CRenderSection_CreateFont_Request(
		unsigned long in_Magic=0
	) :
		m_Magic(in_Magic)
	{}
	CFontDescription m_FontDescription;
	unsigned long m_Magic;
};

struct CRenderSection_CreateFont_Response : public CCommand
{
	CRenderSection_CreateFont_Response(
		unsigned long in_FontHandle
		,unsigned long in_Magic
	) :
		m_FontHandle(in_FontHandle)
		,m_Magic(in_Magic)
	{}
	unsigned long m_FontHandle; // m_FontHandle=0 - error. Font was not created.
	// NOTE: System creates it's own internal font with handle = 1.
	//       User fonts start with handle = 2.
	unsigned long m_Magic;
};

struct CRenderSection_RenderString : public CCommand
{
	CRenderSection_RenderString(
		long in_X, long in_Y,
		const std::string& in_rString,
		unsigned long in_FontHandle,
		unsigned long in_Color
	) :
		m_X(in_X)
		,m_Y(in_Y)
		,m_String(in_rString)
		,m_FontHandle(in_FontHandle)
		,m_Color(in_Color)
	{}
	long m_X;
	long m_Y;
	std::string m_String;
	unsigned long m_FontHandle;
	unsigned long m_Color;
};

// =================================================================================================================

struct CRenderSection_RenderTestObject : public CCommand
{
	CRenderSection_RenderTestObject( const CMatrix& in_M ) : m_M(in_M) {}
	CMatrix m_M;
};

struct CRenderSection_RenderVB : public CCommand
{
	CRenderSection_RenderVB(
		const CMatrix& in_M,
		unsigned long in_VBHandle,
		unsigned long in_PixelShaderHandle,
		unsigned long in_FirstVertex,
		unsigned long in_PrimitiveType,
		unsigned long in_PrimitiveCount
	) :
		m_M(in_M),
		m_VBHandle(in_VBHandle),
		m_PixelShaderHandle(in_PixelShaderHandle),
		m_FirstVertex(in_FirstVertex),
		m_PrimitiveType(in_PrimitiveType),
		m_PrimitiveCount(in_PrimitiveCount)
	{}
	CMatrix m_M;
	unsigned long m_VBHandle;
	unsigned long m_PixelShaderHandle;
	unsigned long m_FirstVertex;
	unsigned long m_PrimitiveType;
	unsigned long m_PrimitiveCount;
};

// =================================================================================================================

struct CRenderSection_RenderDot : public CCommand
{
	CRenderSection_RenderDot(
		long in_X, long in_Y,
		unsigned long in_Color
	) :
		m_X(in_X)
		,m_Y(in_Y)
		,m_Color(in_Color)
	{}
	long m_X;
	long m_Y;
	unsigned long m_Color;
};

// =================================================================================================================

struct CRenderSection_RenderObject_Request : public CCommand {
	CRenderSection_RenderObject_Request(CShadow* in_pShadow) : m_pShadow(in_pShadow) {}
	CShadow* m_pShadow;
};

struct CRenderSection_RenderObject_Response : public CCommand {
	CRenderSection_RenderObject_Response() {}
	std::vector<CShadow*> m_pShadows;
};

// =================================================================================================================

#endif



















