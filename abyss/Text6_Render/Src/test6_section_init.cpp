#include "../Include/test6_section.h"
#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/section_registrator.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

#define TEST_WINDOW_WIDTH			640
#define TEST_WINDOW_HEIGHT			480

void
CTest6Section::Received_MTInfo(const CCommand_MTInfo& in_rInfo)
{
	CLog::Print("CTest6Section::Received_MTInfo()\n");
	CLog::Print("  NProcessors = %ld\n",in_rInfo.m_NProcessors);
	m_NThreadsToRun = 2*in_rInfo.m_NProcessors;
	CLog::Print("  NThreadsToRun = %ld\n",m_NThreadsToRun);
}

long
CTest6Section::GetNThreadsToRun()
{
	assert(m_NThreadsToRun>0);
	return m_NThreadsToRun;
}

bool
CTest6Section::GetNeedVFS()
{
//	return true;
	return false;
}

void
CTest6Section::Received_SectionLaunchResult(const CCommand_LaunchSectionResult& in_rResult)
{
	CLog::Print("CTest6Section::Received_SectionLaunchResult()\n");
}

void
CTest6Section::GetGameWindowParams( CWindowParam& out_rParams )
{
	CLog::Print("CTest6Section::GetGameWindowParams()\n");
	out_rParams.m_Style = WS_CONSTANT;
	//out_rParams.m_Style = WS_RESIZEABLE;
	//out_rParams.m_Style = WS_MINMAX;
	out_rParams.m_ClientWidth = TEST_WINDOW_WIDTH;
	out_rParams.m_ClientHeight = TEST_WINDOW_HEIGHT;
	out_rParams.m_Caption = "Render test.";
}

void
CTest6Section::Received_3DInfo( const CRenderSection_3DEnvironmentInfo& in_rInfo )
{
	CLog::Print("CTest6Section::Received_3DInfo()\n");
	m_3DInfo = in_rInfo;
}

void
CTest6Section::GetRenderInitParams( CRenderSection_InitRender& out_rParams )
{
	CLog::Print("CTest6Section::GetRenderInitParams()\n");
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
CTest6Section::Received_InitRenderResult( INIT_RENDER_RESULT in_bResult )
{
	CLog::Print("CTest6Section::Received_InitRenderResult()\n");
}
