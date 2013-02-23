#include "../Include/render_section.h"
#include "../Include/rendercritical_section_interface.h"
#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

CRenderLayer_2::CRenderLayer_2() :
	m_pHost(NULL)
	,m_RenderCriticalSectionID(SECTION_ID_ERROR)
{
}

CRenderLayer_2::~CRenderLayer_2()
{
}

void
CRenderLayer_2::Initialize(CRenderSection* in_pHost)
{
	assert(m_pHost==NULL);
	//
	assert(in_pHost);
	m_pHost = in_pHost;
}

void
CRenderLayer_2::Activate(long in_RenderCriticalSectionID)
{
	CLog::Print("  render layer 2 ACTIVATED\n");
	m_bActive = true;
	assert(m_RenderCriticalSectionID==SECTION_ID_ERROR);
	assert(in_RenderCriticalSectionID!=SECTION_ID_ERROR);
	m_RenderCriticalSectionID = in_RenderCriticalSectionID;
}

void
CRenderLayer_2::Deactivate()
{
	CLog::Print("  render layer 2 DEACTIVATED\n");
	m_bActive = false;
	assert(m_RenderCriticalSectionID!=SECTION_ID_ERROR);
	m_RenderCriticalSectionID=SECTION_ID_ERROR;
}

bool
CRenderLayer_2::IsActive() const
{
	return m_bActive;
}

void
CRenderLayer_2::Received_DeviceLost()
{
	assert(m_pHost);
	assert(m_RenderCriticalSectionID!=SECTION_ID_ERROR);
	m_pHost->Send_RestoreDevice(m_RenderCriticalSectionID);
}

// =================================================================================================

void
CRenderSection::Send_RestoreDevice(long in_RenderCriticalSectionID)
{
	CTCommandSender<CRenderCriticalSection_RestoreDevice>::SendCommand(
		in_RenderCriticalSectionID,
		CRenderCriticalSection_RestoreDevice()
	);
}




















