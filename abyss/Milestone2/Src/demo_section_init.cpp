#include "../Include/demo_section.h"
#include "../../MT_Core/Include/log.h"

#include <assert.h>

#define TEST_WINDOW_WIDTH	640
#define TEST_WINDOW_HEIGHT	480


#define N_EXCLUSIVE (1+1+1) // window, vfs, input
#define N_AVAILABLE 3
#define N_SECTIONS ( N_EXCLUSIVE + N_AVAILABLE )
void
CDemoSection::Received_MTInfo(const CCommand_MTInfo& in_rInfo)
{
	m_NThreadsToRun = 2*in_rInfo.m_NProcessors;

	m_NThreadsToRun = 6;

}

long
CDemoSection::GetNThreadsToRun()
{
	return m_NThreadsToRun;
}

bool
CDemoSection::GetNeedVFS()
{
	return true;
}

bool
CDemoSection::GetNeedPreloader()
{
	return true;
}

void
CDemoSection::GetGameWindowParams( CWindowParam& out_rParams )
{
	out_rParams.m_Style = WS_CONSTANT;
	out_rParams.m_ClientWidth = TEST_WINDOW_WIDTH;
	out_rParams.m_ClientHeight = TEST_WINDOW_HEIGHT;
	out_rParams.m_Caption = "Test 2.";
}

void
CDemoSection::Received_3DInfo( const CRenderSection_3DEnvironmentInfo& in_rInfo )
{
	m_3DInfo = in_rInfo;
}

void
CDemoSection::GetRenderInitParams( CRenderSection_InitRender& out_rParams )
{
	out_rParams.m_DX = 640;
	out_rParams.m_DY = 480;
	out_rParams.m_bWindowed = true;
	out_rParams.m_FullscreenFormat = RDMF_UNKNOWN;
	out_rParams.m_bDepthBufferRequired = true;
	out_rParams.m_bStencilBufferRequired = false;
	out_rParams.m_FullscreenRefreshRate = 60;
	out_rParams.m_bVSync = false;
}

void
CDemoSection::Received_InitRenderResult( INIT_RENDER_RESULT in_bResult )
{
}























