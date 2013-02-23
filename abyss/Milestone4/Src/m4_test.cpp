#include "../Include/m4_test.h"
#include "../../Objects/Include/e_static_mesh.h"

#include <time.h>

#define ADD_BONES
//#define ADD_NORMALS
//#define ADD_DIFFUSE
//#define ADD_TEXCOORDS

#define N_LEN_SEGMENTS 20
#define N_RING_SEGMENTS 8

class CTestController : public CController< CObject > {
public:
	CTestController() { CLog::Print("CTestController::CTestController()\n"); }
	~CTestController() { CLog::Print("CTestController::~CTestController()\n"); }
	void Added( CObject& in_rHost ) {
		CLog::Print("CTestController::Added()\n");
		m_Start = clock();
	}
	void Removed( CObject& in_rHost ) { CLog::Print("CTestController::Removed()\n"); }
	bool Process( CObject& in_rHost ) {
		//CLog::Print("CTestController::Process()\n");

		CEntity* pEnt = in_rHost.GetEntity();
		assert(pEnt);
		assert( pEnt->GetType() == ET_STATIC_MESH );
		CEntity_StaticMesh* pMesh = (CEntity_StaticMesh*)pEnt;
		CSkeleton& rSkeleton = pMesh->GetSkeleton();

		CBone* pBone0 = rSkeleton.GetRootBone();
		assert(pBone0);
		const std::vector<CBone*>& rChildren0 = pBone0->GetChildren();
		assert( rChildren0.size() == 1 );

		CBone* pBone1 = rChildren0[0];
		assert(pBone1);
		const std::vector<CBone*>& rChildren1 = pBone1->GetChildren();
		assert( rChildren1.size() == 1 );

		CBone* pBone2 = rChildren1[0];

		#define CYCLE (10*CLOCKS_PER_SEC)
		clock_t diff = ( clock() - m_Start ) % CYCLE;
		float Fraction = ((float)diff) / ((float)CYCLE);
		//Fraction = 0.5f;
		//CLog::Print("fraction = %f\n",Fraction);

		// bone 0

		// bone1
		{
			CMatrix NewM;
			NewM.ConstructRotationZ( (float)(CONST_PI_2)*Fraction );
			NewM *= CMatrix().ConstructTranslation( CVector(6.0f,0.0f,0.0f) );

			{
				CElementHandle<CBonePosition> H = pBone1->m_Position.Get();
				CBonePosition& rPos = H;
				rPos.SetPosition( NewM );
			}
		}

		// bone 2
		{
			CMatrix NewM;
			NewM.ConstructRotationZ( -(float)(CONST_PI_2)*Fraction );
			NewM *= CMatrix().ConstructTranslation( CVector(8.0f,0.0f,0.0f) );

			{
				CElementHandle<CBonePosition> H = pBone2->m_Position.Get();
				CBonePosition& rPos = H;
				rPos.SetPosition( NewM );
			}
		}

		return true;
	}
	const std::string& GetName() const {
		static std::string Name("TestController");
		return Name;
	}
private:
	clock_t m_Start;
};

struct CVertex
{
	CVector m_XYZ;
#ifdef ADD_BONES
	float m_BoneWeight0;
	float m_BoneWeight1;
	unsigned long m_BoneIndices;
#endif
#ifdef ADD_NORMALS
	CVector m_Normal;
#endif
#ifdef ADD_DIFFUSE
	unsigned long m_Diffuse;
#endif
#ifdef ADD_TEXCOORDS
	float u;
	float v;
#endif
};
#define VERTEX_SIZE sizeof(CVertex)

unsigned long g_Tex[] = {
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
};

CObject* CreateTestObject() {

	// renderables
	//std::vector< CRenderable_Abstract > renderables;
	std::vector< CRenderable_BoneRemap > BoneRemaps;
	std::vector< CRenderable_AbstractMaterial > Materials;
	{
		//renderables.resize(1);
		BoneRemaps.resize(1);
		Materials.resize(1);

		CRenderable_AbstractMaterial& rMaterial = Materials[0];
		{
			CRenderable_AbstractTextureMap* pMap = new CRenderable_AbstractTextureMap();
			pMap->m_bHasAlpha = true;
			pMap->m_Height = 4;
			pMap->m_Width = 4;
			pMap->m_Data.resize(4*4);
			memcpy( &(pMap->m_Data[0]), g_Tex, 4*4*sizeof(unsigned long) );
			rMaterial.m_Diffuse.m_pTextureMap.reset( pMap );
			rMaterial.m_DiffuseSource = CRenderable_AbstractMaterial::DS_CONSTANT;
			//rMaterial.m_DiffuseSource = CRenderable_AbstractMaterial::DS_VERTEX_COLOR;
			//rMaterial.m_DiffuseSource = CRenderable_AbstractMaterial::DS_TEXTURE;
			rMaterial.m_ARGB = 0xff0000ff;
#ifdef ADD_NORMALS
			rMaterial.m_bLightingEnabled = true;
#else
			rMaterial.m_bLightingEnabled = false;
#endif
			rMaterial.m_bTwoSided = false;
		}

		//CRenderable_Abstract& rRenderable = renderables[0];
		CRenderable_BoneRemap& rBoneRemap = BoneRemaps[0];

		//CRenderable_AbstractGeometry& rGeometry = rRenderable.m_Geometry;
		rBoneRemap.m_Geometries.resize(1);
		CRenderable_AbstractGeometry& rGeometry = rBoneRemap.m_Geometries[0];

		long nTriangles = 0;
		{
			rGeometry.m_VertexBuffers.resize(1);
			CRenderable_AbstractVertexBuffer& rVB = rGeometry.m_VertexBuffers[0];

			rVB.m_VertexFormat.m_XYZ = true;

#ifdef ADD_BONES
			rVB.m_VertexFormat.m_NBonesPerVertex = 3;
#endif

#ifdef ADD_NORMALS
			rVB.m_VertexFormat.m_Normal = true;
#endif

#ifdef ADD_DIFFUSE
			rVB.m_VertexFormat.m_Diffuse = true;
#endif

#ifdef ADD_TEXCOORDS
			rVB.m_VertexFormat.m_Tex0Dimension = CVBFormat::TEXCOORDSET_FLAT;
#endif

			// 20 segments
			// of octagon(8) shape
			// each face = 2 triangles
			nTriangles = 20 * N_RING_SEGMENTS * 2;
			rVB.m_VertexData.resize( nTriangles * 3 * VERTEX_SIZE );
			unsigned char* pVBData = &(rVB.m_VertexData[0]);
			CVertex* pVert = (CVertex*)pVBData;
			for( long i=0; i<20; ++i ) {
				CLog::Print("%d\n",i);

				float XBegin = float(i);

#ifdef ADD_BONES
				long I = i;

				float Weight0Begin;
				if(I<=4)
					Weight0Begin = 1.0f;
				else if(I<8)
					Weight0Begin = float(4-(I-4)) / 4.0f;
				else
					Weight0Begin = 0.0f;

				float Weight1Begin;
				if(I<=4)
					Weight1Begin = 0.0f;
				else if(I<8)
					Weight1Begin = float(I-4) / 4.0f;
				else if(I<=12)
					Weight1Begin = 1.0f;
				else if(I<16)
					Weight1Begin = float(4-(I-12)) / 4.0f;
				else 
					Weight1Begin = 0.0f;
				CLog::Print("w0b=%f w1b=%f\n",Weight0Begin,Weight1Begin);
#endif

#ifdef ADD_DIFFUSE
				float qb = float(i)/20.0f;
				unsigned long cb = (unsigned long)(qb*255.0f);
				if(cb>255) cb=255;
				cb |= 0xff000000;
#endif

#ifdef ADD_TEXCOORDS
				float UBegin = float(i)/20.0f;
#endif

				float XEnd = float(i+1);

#ifdef ADD_BONES
				I = i+1;

				float Weight0End;
				if(I<=4)
					Weight0End = 1.0f;
				else if(I<8)
					Weight0End = float(4-(I-4)) / 4.0f;
				else
					Weight0End = 0.0f;

				float Weight1End;
				if(I<=4)
					Weight1End = 0.0f;
				else if(I<8)
					Weight1End = float(I-4) / 4.0f;
				else if(I<=12)
					Weight1End = 1.0f;
				else if(I<16)
					Weight1End = float(4-(I-12)) / 4.0f;
				else 
					Weight1End = 0.0f;
				CLog::Print("w0e=%f w1e=%f\n",Weight0End,Weight1End);
#endif

#ifdef ADD_DIFFUSE
				float qe = float(i+1)/20.0f;
				unsigned long ce = (unsigned long)(qe*255.0f);
				if(ce>255) ce=255;
				ce |= 0xff000000;
#endif

#ifdef ADD_TEXCOORDS
				float UEnd = float(i+1)/20.0f;
#endif

				for( long j=0; j<N_RING_SEGMENTS; ++j ) {

					float ABegin = float(j) * float(CONST_2PI) / float(N_RING_SEGMENTS);
					float YBegin = 0.5f*cosf(ABegin);
					float ZBegin = 0.5f*sinf(ABegin);
#ifdef ADD_TEXCOORDS
					float VBegin = float(j)/float(N_RING_SEGMENTS);
#endif

					float AEnd = float(j+1) * float(CONST_2PI) / float(N_RING_SEGMENTS);
					float YEnd = 0.5f*cosf(AEnd);
					float ZEnd = 0.5f*sinf(AEnd);
#ifdef ADD_TEXCOORDS
					float VEnd = float(j+1)/float(N_RING_SEGMENTS);
#endif

					pVert[0].m_XYZ = CVector(XBegin,YBegin,ZBegin);
					pVert[1].m_XYZ = CVector(  XEnd,YBegin,ZBegin);
					pVert[2].m_XYZ = CVector(XBegin,  YEnd,  ZEnd);

					pVert[3].m_XYZ = CVector(  XEnd,YBegin,ZBegin);
					pVert[4].m_XYZ = CVector(  XEnd,  YEnd,  ZEnd);
					pVert[5].m_XYZ = CVector(XBegin,  YEnd,  ZEnd);

#ifdef ADD_BONES
					pVert[0].m_BoneWeight0 = Weight0Begin;
					pVert[0].m_BoneWeight1 = Weight1Begin;
					pVert[0].m_BoneIndices = 0x00060300;

					pVert[1].m_BoneWeight0 = Weight0End;
					pVert[1].m_BoneWeight1 = Weight1End;
					pVert[1].m_BoneIndices = 0x00060300;

					pVert[2].m_BoneWeight0 = Weight0Begin;
					pVert[2].m_BoneWeight1 = Weight1Begin;
					pVert[2].m_BoneIndices = 0x00060300;

					pVert[3].m_BoneWeight0 = Weight0End;
					pVert[3].m_BoneWeight1 = Weight1End;
					pVert[3].m_BoneIndices = 0x00060300;

					pVert[4].m_BoneWeight0 = Weight0End;
					pVert[4].m_BoneWeight1 = Weight1End;
					pVert[4].m_BoneIndices = 0x00060300;

					pVert[5].m_BoneWeight0 = Weight0Begin;
					pVert[5].m_BoneWeight1 = Weight1Begin;
					pVert[5].m_BoneIndices = 0x00060300;
#endif

#ifdef ADD_NORMALS
					pVert[0].m_Normal = CVector( 0.0f, pVert[0].m_XYZ.y, pVert[0].m_XYZ.z ).Normalize();
					pVert[1].m_Normal = CVector( 0.0f, pVert[1].m_XYZ.y, pVert[1].m_XYZ.z ).Normalize();
					pVert[2].m_Normal = CVector( 0.0f, pVert[2].m_XYZ.y, pVert[2].m_XYZ.z ).Normalize();
					pVert[3].m_Normal = CVector( 0.0f, pVert[3].m_XYZ.y, pVert[3].m_XYZ.z ).Normalize();
					pVert[4].m_Normal = CVector( 0.0f, pVert[4].m_XYZ.y, pVert[4].m_XYZ.z ).Normalize();
					pVert[5].m_Normal = CVector( 0.0f, pVert[5].m_XYZ.y, pVert[5].m_XYZ.z ).Normalize();
#endif

#ifdef ADD_DIFFUSE
					pVert[0].m_Diffuse = cb;
					pVert[1].m_Diffuse = ce;
					pVert[2].m_Diffuse = cb;
					pVert[3].m_Diffuse = ce;
					pVert[4].m_Diffuse = ce;
					pVert[5].m_Diffuse = cb;
#endif

#ifdef ADD_TEXCOORDS
					pVert[0].u = UBegin;
					pVert[0].v = VBegin;
					pVert[1].u = UEnd;
					pVert[1].v = VBegin;
					pVert[2].u = UBegin;
					pVert[2].v = VEnd;
					pVert[3].u = UEnd;
					pVert[3].v = VBegin;
					pVert[4].u = UEnd;
					pVert[4].v = VEnd;
					pVert[5].u = UBegin;
					pVert[5].v = VEnd;
#endif

					pVert += 6;
				}
			}
		}

		{
#ifdef ADD_BONES
			rBoneRemap.m_UsedBones.resize(3);
			rBoneRemap.m_UsedBones[0] = 0;
			rBoneRemap.m_UsedBones[1] = 1;
			rBoneRemap.m_UsedBones[2] = 2;
#endif
		}

		{
			rGeometry.m_RenderCalls.resize(1);
			CRenderable_AbstractRenderCall& rRCall = rGeometry.m_RenderCalls[0];
			rRCall.m_VB = 0;
			rRCall.m_BaseVertex = 0;
			rRCall.m_IB = -1;
			rRCall.m_BaseVertexIndex = -1;
			rRCall.m_NPrimitives = nTriangles;
			rRCall.m_PrimitiveType = TRIANGLE_LIST;
		}
	}

	// skeleton
	CBone* pRootBone = NULL;
#ifdef ADD_BONES
	{
		CBone* pBone2 = new CBone(
			2
			//,std::pair<CQuaternion,CVector>(CQuaternion(),CVector(10,0,0))
			,CMatrix().ConstructTranslation( CVector(8,0,0) )
			,CMatrix().ConstructTranslation( CVector(8,0,0) ).Invert()
			,std::vector<CBone*>()
		);

		std::vector<CBone*> Bone1Children;
		Bone1Children.push_back( pBone2 );
		CBone* pBone1 = new CBone(
			1
			//,std::pair<CQuaternion,CVector>(CQuaternion(),CVector(10,0,0))
			,CMatrix().ConstructTranslation( CVector(6,0,0) )
			,CMatrix().ConstructTranslation( CVector(8,0,0) ).Invert()
			,Bone1Children
		);

		std::vector<CBone*> Bone0Children;
		Bone0Children.push_back( pBone1 );
		pRootBone = new CBone(
			0
			//,std::pair<CQuaternion,CVector>(CQuaternion(),CVector(10,0,0))
			,CMatrix()
			,CMatrix().ConstructTranslation( CVector(8,0,0) ).Invert()
			,Bone0Children
		);
	}
#endif
	
	// assembly
	CEntity_StaticMesh* pE = new CEntity_StaticMesh( Materials, BoneRemaps, pRootBone );
	CObject* pO = new CObject( "Test", true, CMatrix(), std::vector<CObject*>(), pE );
	CTestController* pC = new CTestController();
	bool Added = pO->AddController( pC );
	assert( Added );
	return pO;

	return NULL;
}





































