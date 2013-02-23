#include "../Include/e_static_mesh.h"
#include "../../MT_Core/Include/log.h"

CEntity_StaticMesh::CEntity_StaticMesh() : m_Skeleton(NULL)
{
	//CLog::Print("CEntity_StaticMesh(%p)::CEntity_StaticMesh(), pOrigin=%p\n",this,(CLinkOrigin*)this);

	// TEST {
	//m_pRootBone = new CBone( CMatrix(), const std::vector< CBone* >() );
	//m_pRootBone = new CBone( std::pair< CQuaternion, CVector >(), const std::vector< CBone* >() );
	//m_pRootBone = new CBone( CVector(), const std::vector< CBone* >() );
	// } TEST
}

CEntity_StaticMesh::CEntity_StaticMesh(
	const std::vector< CRenderable_AbstractMaterial > in_Materials
	,const std::vector< CRenderable_BoneRemap >& in_RemappedGeometries
	,CBone* in_pRootBone
) :
	m_Skeleton(in_pRootBone)
	,m_RemappedGeometries(in_RemappedGeometries)
	,m_Materials(in_Materials)
{
	//CLog::Print("CEntity_StaticMesh(%p)::CEntity_StaticMesh(), pOrigin=%p\n",this,(CLinkOrigin*)this);
	CalcBox();
}

//CEntity_StaticMesh::CEntity_StaticMesh( const std::vector< CRenderable_Abstract >& in_Renderables, CBone* in_pRootBone ) :
//	m_Renderables(in_Renderables),
//	m_Skeleton(in_pRootBone)
//{
//	//CLog::Print("CEntity_StaticMesh(%p)::CEntity_StaticMesh(), pOrigin=%p\n",this,(CLinkOrigin*)this);
//	CalcBox();
//}

CEntity_StaticMesh::~CEntity_StaticMesh() {
	//CLog::Print("CEntity_StaticMesh(%p)::~CEntity_StaticMesh()\n",this);
	assert( m_ShadowTracker.NoShadowsAlive() );
}

void
CEntity_StaticMesh::CalcBox() {
	bool bFirst = true;
	for( std::vector< CRenderable_BoneRemap >::const_iterator ItRemaps=m_RemappedGeometries.begin(); ItRemaps!=m_RemappedGeometries.end(); ++ItRemaps ) {
		const CRenderable_BoneRemap& rRemap = *ItRemaps;
		for( std::vector< CRenderable_AbstractGeometry >::const_iterator It=rRemap.m_Geometries.begin(); It!=rRemap.m_Geometries.end(); ++It ) {
			const CRenderable_AbstractGeometry& rGeom = *It;
			const std::vector< CRenderable_AbstractVertexBuffer >& rBuffers = rGeom.m_VertexBuffers;
			for( std::vector< CRenderable_AbstractVertexBuffer >::const_iterator Itt=rBuffers.begin(); Itt!=rBuffers.end(); ++Itt ) {
				const CRenderable_AbstractVertexBuffer& rVB = *Itt;
				if( rVB.m_VertexFormat.m_XYZ && !rVB.m_VertexData.empty() ) {

					// calc vertex stride
					unsigned long Stride = 3 * sizeof(float);
					if( rVB.m_VertexFormat.m_NBonesPerVertex > 0 ) {
						Stride += (rVB.m_VertexFormat.m_NBonesPerVertex - 1) * sizeof(float);
						Stride += sizeof(unsigned long);
					}
					if( rVB.m_VertexFormat.m_Normal )
						Stride += 3 * sizeof(float);
					if( rVB.m_VertexFormat.m_Diffuse )
						Stride += sizeof(unsigned long);
					if( rVB.m_VertexFormat.m_Specular )
						Stride += sizeof(unsigned long);
					Stride += rVB.m_VertexFormat.m_Tex0Dimension * sizeof(float);
					Stride += rVB.m_VertexFormat.m_Tex1Dimension * sizeof(float);
					Stride += rVB.m_VertexFormat.m_Tex2Dimension * sizeof(float);
					Stride += rVB.m_VertexFormat.m_Tex3Dimension * sizeof(float);
					const unsigned char* pData = &(rVB.m_VertexData[0]);
					const unsigned char* pDataEnd = pData + rVB.m_VertexData.size();

					// update box
					for( ; pData+Stride<=pDataEnd; pData += Stride ) {
						const CVector* pV = reinterpret_cast<const CVector*>(pData);
						if(bFirst) {
							m_Box.m_Pos = *pV;
							m_Box.m_Neg = *pV;
							bFirst = false;
						} else {
							m_Box += *pV;
						}
					}
				}
			}
		}
	}
}








