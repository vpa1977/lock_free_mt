#include "../Include/render_section.h"
#include "../Include/rendercritical_section_interface.h"
#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

CRenderLayer_1::CRenderLayer_1() :
	m_pHost(NULL)
	,m_pLayer2(NULL)
	,m_bActive(false)
	,m_RenderCriticalSectionID(SECTION_ID_ERROR)
	,m_RequesterOf3DInfo(SECTION_ID_ERROR)
	,m_RequesterOfInit(SECTION_ID_ERROR)
	,m_RequesterOfClose(SECTION_ID_ERROR)
{
}

CRenderLayer_1::~CRenderLayer_1()
{
}

void
CRenderLayer_1::Initialize(CRenderSection* in_pHost, CRenderLayer_2* in_pLayer2)
{
	assert(m_pHost==NULL);
	assert(m_pLayer2==NULL);
	//
	assert(in_pHost);
	m_pHost = in_pHost;
	assert(in_pLayer2);
	m_pLayer2 = in_pLayer2;
}

void
CRenderLayer_1::Activate(long in_RenderCriticalSectionID)
{
	CLog::Print("  render layer 1 ACTIVATED\n");
	assert(m_RenderCriticalSectionID==SECTION_ID_ERROR);
	m_bActive = true;
	m_RenderCriticalSectionID = in_RenderCriticalSectionID;
}

void
CRenderLayer_1::Deactivate()
{
	assert(m_pLayer2);
	m_pLayer2->Deactivate();
	//
	m_bActive = false;
	m_RenderCriticalSectionID = SECTION_ID_ERROR;
	CLog::Print("  render layer 1 DEACTIVATED\n");
}

void
CRenderLayer_1::Received_Get3DInfo(long in_RequesterOf3DInfo)
{
	assert(m_pHost);
	assert(m_RenderCriticalSectionID!=SECTION_ID_ERROR);
	assert(m_RequesterOf3DInfo==SECTION_ID_ERROR);
	m_RequesterOf3DInfo = in_RequesterOf3DInfo;
	m_pHost->Send_Get3DEnvironmentInfo( m_RenderCriticalSectionID );
}

void
CRenderLayer_1::Received_3DInfo( const CRenderCriticalSection_3DEnvironmentInfo& in_rInfo )
{
	assert(m_pHost);
	assert(m_RequesterOf3DInfo!=SECTION_ID_ERROR);
	CRenderSection_3DEnvironmentInfo Info;
	Info.m_bError = in_rInfo.m_bError;
	if( !Info.m_bError )
	{
		Info.m_DisplayModes = in_rInfo.m_DisplayModes;
		Info.m_CanRenderWindowed = in_rInfo.m_CanRenderWindowed;
		Info.m_CurrentDisplayMode = in_rInfo.m_CurrentDisplayMode;
		Info.m_16Depth = in_rInfo.m_16Depth;
		Info.m_16DepthStencil = in_rInfo.m_16DepthStencil;
		Info.m_32Depth = in_rInfo.m_32Depth;
		Info.m_32DepthStencil = in_rInfo.m_32DepthStencil;
	}
	m_pHost->Send_3DEnvironmentInfo( m_RequesterOf3DInfo, Info );
	m_RequesterOf3DInfo=SECTION_ID_ERROR;
}

void
CRenderLayer_1::Received_InitRender( long in_RequesterOfInit, const CRenderSection_InitRender& in_rInit )
{
	assert(m_pHost);
	assert(m_RenderCriticalSectionID!=SECTION_ID_ERROR);
	assert(m_RequesterOfInit==SECTION_ID_ERROR);
	assert(m_RequesterOfClose==SECTION_ID_ERROR);
	m_RequesterOfInit = in_RequesterOfInit;
	m_pHost->Send_InitRender(m_RenderCriticalSectionID,in_rInit);
}

void
CRenderLayer_1::Received_InitRenderResult( const CRenderCriticalSection_InitRenderResult& in_rResult )
{
	assert(m_pHost);
	assert(m_RequesterOfInit!=SECTION_ID_ERROR);
	assert(m_pLayer2);
	if( in_rResult.m_Result==IRR_OK )
	{
		assert(in_rResult.m_pDevice!=NULL);
		m_pLayer2->Activate(m_RenderCriticalSectionID);
	}
	CRenderSection_InitRenderResult Result( in_rResult.m_Result );
	m_pHost->Send_InitRenderResult( m_RequesterOfInit, Result );
	m_RequesterOfInit = SECTION_ID_ERROR;
}

void
CRenderLayer_1::Received_CloseRender( long in_RequesterID )
{
	assert(m_pHost);
	assert(m_pLayer2);
	assert(m_RequesterOfInit==SECTION_ID_ERROR);
	assert(m_RequesterOfClose==SECTION_ID_ERROR);
	m_pLayer2->Deactivate();
	m_RequesterOfClose = in_RequesterID;
	m_pHost->Send_CloseRender( m_RenderCriticalSectionID );
}

void
CRenderLayer_1::Received_CloseRenderResult( const CRenderSection_CloseRenderResult& in_rResult )
{
	assert(m_pHost);
	assert(m_RequesterOfClose!=SECTION_ID_ERROR);
	m_pHost->Send_CloseRenderResult( m_RequesterOfClose, in_rResult );
}

// ============================================================================================

void
CRenderSection::Send_Get3DEnvironmentInfo(long in_RenderCriticalSectionID)
{
	CTCommandSender<CRenderCriticalSection_Get3DEnvironmentInfo>::SendCommand(
		in_RenderCriticalSectionID,
		CRenderCriticalSection_Get3DEnvironmentInfo()
	);
}

void
CRenderSection::Send_3DEnvironmentInfo(long in_RequesterSectionID, const CRenderSection_3DEnvironmentInfo& in_rInfo )
{
	CTCommandSender<CRenderSection_3DEnvironmentInfo>::SendCommand(
		in_RequesterSectionID,
		in_rInfo
	);
}

void
CRenderSection::Send_InitRender(long in_RenderCriticalSectionID, const CRenderSection_InitRender& in_rInit )
{
	CTCommandSender<CRenderSection_InitRender>::SendCommand(
		in_RenderCriticalSectionID,
		in_rInit
	);
}

void
CRenderSection::Send_InitRenderResult(long in_RequesterSectionID, const CRenderSection_InitRenderResult& in_rResult )
{
	CTCommandSender<CRenderSection_InitRenderResult>::SendCommand(
		in_RequesterSectionID,
		in_rResult
	);
}

void
CRenderSection::Send_CloseRender(long in_RenderCriticalSectionID )
{
	CTCommandSender<CRenderSection_CloseRender>::SendCommand(
		in_RenderCriticalSectionID,
		CRenderSection_CloseRender()
	);
}

void
CRenderSection::Send_CloseRenderResult(long in_RequesterSectionID, const CRenderSection_CloseRenderResult& in_rResult )
{
	CTCommandSender<CRenderSection_CloseRenderResult>::SendCommand(
		in_RequesterSectionID,
		in_rResult
	);
}








