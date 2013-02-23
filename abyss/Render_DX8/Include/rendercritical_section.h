#ifndef RENDERCRITICAL_SECTION_HPP_INCLUDED
#define RENDERCRITICAL_SECTION_HPP_INCLUDED

#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../MT_Core/Include/command_sender.h"
#include "rendercritical_section_interface.h"
#include "render_section_init.h"

#include <d3d8.h>

#pragma warning(disable:4250)

class CRenderCriticalSection :
	public CSection
	,public CTCommandSender<CRenderCriticalSection_BoundToWindow>
	,public CTCommandSender<CRenderCriticalSection_3DEnvironmentInfo>
	,public CTCommandSender<CRenderCriticalSection_InitRenderResult>
	,public CTCommandSender<CRenderSection_CloseRenderResult>
	,public CTCommandSender<CRenderCriticalSection_RestoreDeviceComplete>
{
public:
	CRenderCriticalSection( long in_SectionID );
	~CRenderCriticalSection();
	std::string GetName()
	{
		return "CRenderCriticalSection";
	}
	void Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand );
	//
	void Reaction( long in_SrcSectionID, const CRenderSection_InitRender& in_rInit );
	void Reaction( long in_SrcSectionID, const CRenderSection_CloseRender& in_rClose );
	//
	void Reaction( long in_SrcSectionID, const CRenderCriticalSection_BindToWindow& in_rBind );
	void Reaction( long in_SrcSectionID, const CRenderCriticalSection_UnbindAndDie& in_rDie );
	void Reaction( long in_SrcSectionID, const CRenderCriticalSection_Get3DEnvironmentInfo& in_rGetInfo );
	void Reaction( long in_SrcSectionID, const CRenderCriticalSection_RestoreDevice& in_rRestore );
//	//
private:
	HWND m_HWnd;
	IDirect3D8* m_pD3D8;
	IDirect3DDevice8* m_pD3D8Device;
	D3DPRESENT_PARAMETERS m_PP;

	bool m_b16bitDepthOnly;

	bool TryToCreateDevice( const CRenderSection_InitRender& in_rInit, D3DFORMAT in_BBFormat, RENDER_VERTEX_PROCESS_TYPE in_VertexProcessingType );

	bool ChooseDepthFormat( D3DFORMAT in_RenderFormat, bool in_bStencil, D3DFORMAT& out_rDepthFormat );
	bool CheckDepthFormat( D3DFORMAT in_RenderFormat, D3DFORMAT in_DepthFormat );

	void ReleaseRenderObjects();
};

#endif