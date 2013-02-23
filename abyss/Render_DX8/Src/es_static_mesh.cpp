#include "../Include/es_static_mesh.h"
#include "../../MT_Core/Include/log.h"


//==========================================================================================

CEntityShadow_StaticMesh::CEntityShadow_StaticMesh( boost::shared_ptr< CStaticMesh_DX8 >& in_pMesh ) :
	m_pStaticMeshDX8(in_pMesh)
{
	//CLog::Print("CEntityShadow_StaticMesh(%p)::CEntityShadow_StaticMesh() pLinkTarget=%p\n",this,(CLinkTarget*)this);
}

CEntityShadow_StaticMesh::~CEntityShadow_StaticMesh() {
	//CLog::Print("CEntityShadow_StaticMesh(%p)::~CEntityShadow_StaticMesh()\n",this);
}






