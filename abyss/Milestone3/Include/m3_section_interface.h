#ifndef M3_SECTION_INTERFACE_HPP_INCLUDED
#define M3_SECTION_INTERFACE_HPP_INCLUDED

#define M3Section_CLSID 209716551

#include "../../MT_Core/Include/command.h"

// ================================================================================

struct CM3Section_WaitingForPreloadToComplete : public CCommand {};

struct CM3Section_StartRender : public CCommand {};

// ================================================================================

#endif












