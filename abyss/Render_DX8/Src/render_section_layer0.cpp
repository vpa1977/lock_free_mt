#include "../Include/render_section.h"
#include "../Include/rendercritical_section_interface.h"
#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

// TODO : BIG FUCKING TODO : Received_UnbindFromWindow() event can come ANYTIME !!!

CRenderLayer_0::CRenderLayer_0() :
	m_State(STATE_0_JUST_INITIALIZED),
	m_pHost(NULL),
	m_pLayer1(NULL),
	m_WindowSectionID(SECTION_ID_ERROR),
	m_RenderCriticalSectionID(SECTION_ID_ERROR),
	m_HWnd(NULL),
	m_CallerSectionID(SECTION_ID_ERROR)
{
}

CRenderLayer_0::~CRenderLayer_0()
{
}

void
CRenderLayer_0::Initialize(CRenderSection* in_pHost, CRenderLayer_1* in_pLayer1)
{
	assert(m_State==STATE_0_JUST_INITIALIZED);
	assert(m_pHost==NULL);
	assert(m_pLayer1==NULL);
	assert(m_WindowSectionID==SECTION_ID_ERROR);
	assert(m_RenderCriticalSectionID==SECTION_ID_ERROR);
	assert(m_HWnd==NULL);
	//
	assert(in_pHost);
	m_pHost = in_pHost;
	assert(in_pLayer1);
	m_pLayer1 = in_pLayer1;
}

void
CRenderLayer_0::Received_BindToWindow( long in_CallerSectionID, long in_WindowSectionID, HWND in_HWnd )
{
	assert(m_State==STATE_0_JUST_INITIALIZED);
	assert(m_pHost);
	assert(m_WindowSectionID==SECTION_ID_ERROR);
	assert(m_RenderCriticalSectionID==SECTION_ID_ERROR);
	assert(m_HWnd==NULL);
	assert(m_CallerSectionID==SECTION_ID_ERROR);
	//
	assert(in_WindowSectionID!=SECTION_ID_ERROR);
	m_WindowSectionID = in_WindowSectionID;
	assert(in_HWnd!=NULL);
	m_HWnd = in_HWnd;
	assert(in_CallerSectionID>SECTION_ID_ERROR);
	m_CallerSectionID=in_CallerSectionID;
	m_pHost->Send_LaunchRenderCriticalSection( in_WindowSectionID );
	m_State = STATE_1_SENT_LAUNCH;
}

void
CRenderLayer_0::Received_SectionLaunchResult( long in_RenderCriticalSectionID )
{
	assert(m_State==STATE_1_SENT_LAUNCH);
	assert(m_pHost);
	assert(m_WindowSectionID!=SECTION_ID_ERROR);
	assert(m_RenderCriticalSectionID==SECTION_ID_ERROR);
	assert(m_HWnd!=NULL);
	//
	assert(in_RenderCriticalSectionID!=SECTION_ID_ERROR);
	CLog::Print("  m_RenderCriticalSectionID = %ld\n",in_RenderCriticalSectionID);
	m_RenderCriticalSectionID=in_RenderCriticalSectionID;
	m_pHost->Send_BindRenderCriticalSection(m_RenderCriticalSectionID,m_HWnd);
	m_State = STATE_2_SENT_CRITICAL_BIND;
}

void
CRenderLayer_0::Received_RenderCriticalBoundToWindow()
{
	assert(m_State==STATE_2_SENT_CRITICAL_BIND);
	assert(m_pHost);
	assert(m_pLayer1!=NULL);
	assert(m_WindowSectionID!=SECTION_ID_ERROR);
	assert(m_RenderCriticalSectionID!=SECTION_ID_ERROR);
	assert(m_HWnd!=NULL);
	assert(m_CallerSectionID>SECTION_ID_ERROR);
	//
	m_pHost->Send_RenderBound(m_CallerSectionID,m_HWnd);
	m_CallerSectionID = SECTION_ID_ERROR;
	m_State = STATE_3_BOUND;
	//
	m_pLayer1->Activate(m_RenderCriticalSectionID);
}

void
CRenderLayer_0::Received_UnbindFromWindow( HWND in_HWnd )
{
	assert(m_State==STATE_3_BOUND);
	assert(m_pHost);
	assert(m_WindowSectionID!=SECTION_ID_ERROR);
	assert(m_RenderCriticalSectionID!=SECTION_ID_ERROR);
	assert(m_HWnd!=NULL);
	//
	assert(m_pLayer1);
	m_pLayer1->Deactivate();
	//
	m_pHost->Send_UnbindAndDieRenderCriticalSection(m_RenderCriticalSectionID);
	m_State = STATE_0_JUST_INITIALIZED;
	m_WindowSectionID = SECTION_ID_ERROR;
	m_RenderCriticalSectionID = SECTION_ID_ERROR;
	m_HWnd = NULL;
}

// ============================================================================================

void
CRenderSection::Send_LaunchRenderCriticalSection(long in_WindowSectionID)
{
	CTCommandSender<CCommand_LaunchSection>::SendCommand(
		SECTION_ID_SYSTEM,
		CCommand_LaunchSection(
			CRenderCriticalSection_CLSID,
			RENDER_CRITICAL_LAUNCH_MAGIC,
			SF_BOUND|SF_EXCLUSIVE|SF_PARTNER,
			in_WindowSectionID
		)
	);
}

void
CRenderSection::Send_BindRenderCriticalSection(long in_RenderCriticalSectionID, HWND in_HWnd)
{
	CTCommandSender<CRenderCriticalSection_BindToWindow>::SendCommand(
		in_RenderCriticalSectionID,
		CRenderCriticalSection_BindToWindow( in_HWnd )
	);
}

void
CRenderSection::Send_UnbindAndDieRenderCriticalSection(long in_RenderCriticalSectionID)
{
	CTCommandSender<CRenderCriticalSection_UnbindAndDie>::SendCommand(
		in_RenderCriticalSectionID,
		CRenderCriticalSection_UnbindAndDie()
	);
}

void
CRenderSection::Send_RenderBound(long in_CallerSectionID, HWND in_HWnd)
{
	CTCommandSender<CRenderSection_BoundToWindow>::SendCommand(
		in_CallerSectionID,
		CRenderSection_BoundToWindow(in_HWnd)
	);
}











