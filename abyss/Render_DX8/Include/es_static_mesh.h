#ifndef ENTITY_SHADOW_STATIC_MESH_HPP_INCLUDED
#define ENTITY_SHADOW_STATIC_MESH_HPP_INCLUDED

#include "static_mesh_dx8.h"
#include "../../Common/Include/link.h"
#include "../../Objects/Include/shadow.h"
#include <boost/shared_ptr.hpp>
#include <stdio.h>

#define N_TARGETS 2 // CObject and CEntity_StaticMesh

class CEntityShadow_StaticMesh : public CShadow, public CDeathNotificator<N_TARGETS>
{
public:
	CEntityShadow_StaticMesh( boost::shared_ptr< CStaticMesh_DX8 >& in_pMesh );
	~CEntityShadow_StaticMesh();
	unsigned long GetType() const { return ET_STATIC_MESH; }

	boost::shared_ptr< CStaticMesh_DX8 > m_pStaticMeshDX8;
};

#endif




















