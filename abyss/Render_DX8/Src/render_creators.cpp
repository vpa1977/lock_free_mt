#include "../../MT_Core/Include/section_registrator.h"

#include "../Include/render_section_interface.h"
#include "../Include/render_section.h"

#include "../Include/rendercritical_section_interface.h"
#include "../Include/rendercritical_section.h"

#pragma comment ( lib, "d3d8.lib" )

void Touch_RenderSection() {}

static CTSectionRegistrator<CRenderSection_CLSID,CRenderSection> g_CRenderSectionRegistrator(SINGLE_INSTANCE_ONLY);
static CTSectionRegistrator<CRenderCriticalSection_CLSID,CRenderCriticalSection> g_CRenderCriticalSectionRegistrator(SINGLE_INSTANCE_ONLY);

