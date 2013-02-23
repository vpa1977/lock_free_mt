#include "../include/hamsters.h"
#include "../Include/hamster_section.h"

#define N_EXCLUSIVE (1+1+1)	// window+rendercrit, vfs, input
#define N_WORKERS 1			// we have only one worker section.
							// Count preloader also here ?
#define N_SECTIONS ( N_EXCLUSIVE + N_WORKERS )

void
CHamsterSection::Received_MTInfo(const CCommand_MTInfo& in_rInfo)
{
	m_NThreadsToRun = N_SECTIONS;
}

long
CHamsterSection::GetNThreadsToRun()
{
	return m_NThreadsToRun;
}

bool
CHamsterSection::GetNeedVFS()
{
	return true;
	//return false;
}

bool
CHamsterSection::GetNeedPreloader()
{
	return true;
	//return false;
}

void
CHamsterSection::GetGameWindowParams( CWindowParam& out_rParams )
{
	out_rParams.m_Style = WS_CONSTANT;
	out_rParams.m_ClientWidth = TEST_WINDOW_WIDTH;
	out_rParams.m_ClientHeight = TEST_WINDOW_HEIGHT;
	out_rParams.m_Caption = "Milestone 4.";
}

void
CHamsterSection::Received_3DInfo( const CRenderSection_3DEnvironmentInfo& in_rInfo )
{
	m_3DInfo = in_rInfo;
}

void
CHamsterSection::GetRenderInitParams( CRenderSection_InitRender& out_rParams )
{
	out_rParams.m_DX = TEST_WINDOW_WIDTH;
	out_rParams.m_DY = TEST_WINDOW_HEIGHT;
	out_rParams.m_bWindowed = true;
	out_rParams.m_FullscreenFormat = RDMF_UNKNOWN;
	out_rParams.m_bDepthBufferRequired = true;
	out_rParams.m_bStencilBufferRequired = false;
	out_rParams.m_FullscreenRefreshRate = 60;
	out_rParams.m_bVSync = false;
}

void
CHamsterSection::Received_InitRenderResult( INIT_RENDER_RESULT in_bResult )
{
	RTASSERT( IRR_OK == in_bResult );
}


void
CHamsterSection::Received_WindowClosed()
{
	CLog::Print("============================= WINDOW CLOSED ======================\n");
	
}
























