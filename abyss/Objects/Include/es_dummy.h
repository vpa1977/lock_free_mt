#ifndef ENTITY_SHADOW_DUMMY_HPP_INCLUDED
#define ENTITY_SHADOW_DUMMY_HPP_INCLUDED

#include "../../Common/Include/link.h"
#include "shadow.h"
#include "../../MT_Core/Include/log.h"

class CEntityShadow_Dummy : public CShadow, public CDeathNotificator<1>
{
public:
	CEntityShadow_Dummy() { CLog::Print("CEntityShadow_Dummy(%p)::CEntityShadow_Dummy()\n",this); }
	~CEntityShadow_Dummy() { CLog::Print("CEntityShadow_Dummy(%p)::~CEntityShadow_Dummy()\n",this); }
	unsigned long GetType() const { return ET_DUMMY; }
};

#endif




















