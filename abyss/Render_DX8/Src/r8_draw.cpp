#include "../Include/r8.h"
#include "../Include/algebra.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

void
CRenderDX8::DrawTestObject( const CMatrix& in_M )
{
	assert(m_pD3D8Device);
	HRESULT Res;
	Res = m_pD3D8Device->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
	assert(Res==D3D_OK);
	Res = m_pD3D8Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	assert(Res==D3D_OK);
	Res = m_pD3D8Device->SetRenderState(D3DRS_LIGHTING,FALSE);
	assert(Res==D3D_OK);

	Res = m_pD3D8Device->BeginScene();
	assert(Res==D3D_OK);

	Res = m_pD3D8Device->SetVertexShader( D3DFVF_XYZ | D3DFVF_DIFFUSE );
	assert(Res==D3D_OK);

	Res = m_pD3D8Device->SetTransform( D3DTS_WORLD, (const D3DMATRIX*)&in_M );
	assert(Res==D3D_OK);

#pragma pack(1)
	struct CTMPVertex
	{
		CVector V;
		unsigned long Color;
		CTMPVertex( float x, float y, float z, unsigned long C ) : V(x,y,z), Color(C) {}
	};
#pragma pack()
	const CTMPVertex vertices[] = {
		CTMPVertex(0.0f,0.0f,0.0f,0xFF0000),
		CTMPVertex(5.0f,0.0f,0.0f,0xFF0000),
		CTMPVertex(0.0f,0.0f,0.0f,0xFF00),
		CTMPVertex(0.0f,5.0f,0.0f,0xFF00),
		CTMPVertex(0.0f,0.0f,0.0f,0xFF),
		CTMPVertex(0.0f,0.0f,5.0f,0xFF)
	};
	Res = m_pD3D8Device->DrawPrimitiveUP( D3DPT_LINELIST, 3, vertices, sizeof(CTMPVertex) );
	assert(Res==D3D_OK);

	Res = m_pD3D8Device->EndScene();
	assert(Res==D3D_OK);
}

unsigned long
GetNVerticesUsed( unsigned long in_PrimitiveType, unsigned long in_PrimitiveCount )
{
	switch(in_PrimitiveType)
	{
		case PRIM_POINT_LIST:
			return in_PrimitiveCount;
		case PRIM_LINE_LIST:
			return 2*in_PrimitiveCount;
		case PRIM_LINE_STRIP:
			return 1+in_PrimitiveCount;
		case PRIM_TRIANGLE_LIST:
			return 3*in_PrimitiveCount;
		case PRIM_TRIANGLE_STRIP:
		case PRIM_TRIANGLE_FAN:
			return 2+in_PrimitiveCount;
		default:
			assert(false);
	}
	return 0;
}

D3DPRIMITIVETYPE
ConvertPrimitiveType(unsigned long in_PrimitiveType)
{
	switch(in_PrimitiveType)
	{
		case PRIM_POINT_LIST:
			return D3DPT_POINTLIST;
		case PRIM_LINE_LIST:
			return D3DPT_LINELIST;
		case PRIM_LINE_STRIP:
			return D3DPT_LINESTRIP;
		case PRIM_TRIANGLE_LIST:
			return D3DPT_TRIANGLELIST;
		case PRIM_TRIANGLE_STRIP:
			return D3DPT_TRIANGLESTRIP;
		case PRIM_TRIANGLE_FAN:
			return D3DPT_TRIANGLEFAN;
	}
	assert(false);
	return D3DPT_POINTLIST;
}

void
CRenderDX8::DrawVB(
		const CMatrix& in_M,
		unsigned long in_VBHandle,
		unsigned long in_FirstVertex,
		unsigned long in_PrimitiveType,
		unsigned long in_PrimitiveCount
)
{
	assert(m_pD3D8Device);
//CLog::Print("in_PrimitiveCount=%lu\n",in_PrimitiveCount);

	CVBInfoMapIterator It = m_VBMap.find( in_VBHandle );
	assert( It != m_VBMap.end() );

	if(It->second.m_pVB)
	{
		unsigned long NVerticesUsed = GetNVerticesUsed(in_PrimitiveType,in_PrimitiveCount);
		assert( in_FirstVertex+NVerticesUsed <= It->second.m_NVertices );

		HRESULT Res;
		Res = m_pD3D8Device->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE/*D3DZB_FALSE*/);
		assert(Res==D3D_OK);
		Res = m_pD3D8Device->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
		assert(Res==D3D_OK);
		Res = m_pD3D8Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW/*D3DCULL_NONE*/);
		assert(Res==D3D_OK);

		bool bEnabeLighting = 0!=(It->second.m_FVF&D3DFVF_NORMAL);
		Res = m_pD3D8Device->SetRenderState(D3DRS_LIGHTING,bEnabeLighting?TRUE:FALSE);
		assert(Res==D3D_OK);

		Res = m_pD3D8Device->SetRenderState(D3DRS_COLORVERTEX,TRUE);
		assert(Res==D3D_OK);
		Res = m_pD3D8Device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);
		assert(Res==D3D_OK);
		Res = m_pD3D8Device->SetRenderState(D3DRS_SPECULARMATERIALSOURCE,D3DMCS_COLOR2);
		assert(Res==D3D_OK);
		Res = m_pD3D8Device->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE,D3DMCS_COLOR1);
		assert(Res==D3D_OK);

		Res = m_pD3D8Device->SetRenderState(D3DRS_NORMALIZENORMALS,TRUE);
		assert(Res==D3D_OK);

		Res = m_pD3D8Device->SetTransform( D3DTS_WORLD, (const D3DMATRIX*)&in_M );
		assert(Res==D3D_OK);

		Res = m_pD3D8Device->BeginScene();
		assert(Res==D3D_OK);

		Res = m_pD3D8Device->SetStreamSource(0,It->second.m_pVB,It->second.m_VertexSize);
		assert(Res==D3D_OK);
		Res = m_pD3D8Device->SetVertexShader( It->second.m_FVF );
		assert(Res==D3D_OK);
		Res = m_pD3D8Device->DrawPrimitive( ConvertPrimitiveType(in_PrimitiveType), in_FirstVertex, in_PrimitiveCount );
		assert(Res==D3D_OK);

		Res = m_pD3D8Device->EndScene();
		assert(Res==D3D_OK);
	}
}

void
CRenderDX8::DrawVB(
		const CMatrix& in_M,
		unsigned long in_VBHandle,
		unsigned long in_PSHandle,
		unsigned long in_FirstVertex,
		unsigned long in_PrimitiveType,
		unsigned long in_PrimitiveCount
)
{
	assert(m_pD3D8Device);

	CVBInfoMapIterator It = m_VBMap.find( in_VBHandle );
	assert( It != m_VBMap.end() );

	if(It->second.m_pVB)
	{
		unsigned long NVerticesUsed = GetNVerticesUsed(in_PrimitiveType,in_PrimitiveCount);
		long testCount = It->second.m_NVertices;
		assert( in_FirstVertex+NVerticesUsed <= It->second.m_NVertices );

		HRESULT Res;
		{
			// TODO: Move to "once-per-frame" render settings
			Res = m_pD3D8Device->SetRenderState(D3DRS_COLORVERTEX,TRUE);
			assert(Res==D3D_OK);
			Res = m_pD3D8Device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1);
			assert(Res==D3D_OK);
			Res = m_pD3D8Device->SetRenderState(D3DRS_SPECULARMATERIALSOURCE,D3DMCS_COLOR2);
			assert(Res==D3D_OK);
			Res = m_pD3D8Device->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE,D3DMCS_COLOR1);
			assert(Res==D3D_OK);
			Res = m_pD3D8Device->SetRenderState(D3DRS_NORMALIZENORMALS,TRUE);
			assert(Res==D3D_OK);
		}

		Res = m_pD3D8Device->SetTransform( D3DTS_WORLD, (const D3DMATRIX*)&in_M );
		assert(Res==D3D_OK);
		Res = m_pD3D8Device->SetStreamSource(0,It->second.m_pVB,It->second.m_VertexSize);
		assert(Res==D3D_OK);
		Res = m_pD3D8Device->SetVertexShader( It->second.m_FVF );
		assert(Res==D3D_OK);

		Res = m_pD3D8Device->BeginScene();
		assert(Res==D3D_OK);

		unsigned long NPasses = m_PixelShaderManager.UsePixelShader(in_PSHandle);
		assert(NPasses>0);
		for(unsigned long i=0; i<NPasses; ++i)
		{
			m_PixelShaderManager.ApplyPixelShaderPass(i);
			Res = m_pD3D8Device->DrawPrimitive( ConvertPrimitiveType(in_PrimitiveType), in_FirstVertex, in_PrimitiveCount );
			assert(Res==D3D_OK);
		}

		Res = m_pD3D8Device->EndScene();
		assert(Res==D3D_OK);
	}

}

void
CRenderDX8::DrawRenderables(
	const CMatrix& in_M,
	const std::vector< CRenderable_AbstractMaterial >& in_rAbstractMaterials,
	const std::vector< CRenderable_BoneRemap >& in_rAbstractBoneRemaps,
	std::vector< CRenderable_MaterialDX8 >& in_rRenderMaterials,
	std::vector< CRenderable_BoneRemapDX8 >& in_rRenderBoneRemaps,
	const CSkeleton_DX8& in_rSkeleton
)
{
	assert(m_pD3D8Device);
	assert( in_rAbstractMaterials.size() == in_rAbstractBoneRemaps.size() );
	if( in_rAbstractMaterials.size() != in_rRenderMaterials.size() )
		FillMaterials( in_rAbstractMaterials, in_rRenderMaterials );
	if( in_rAbstractBoneRemaps.size() != in_rRenderBoneRemaps.size() )
		FillGeometries( in_rAbstractBoneRemaps, in_rRenderBoneRemaps );

	HRESULT Res;
	m_pD3D8Device->BeginScene();
	//m_pD3D8Device->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME  );

	size_t nMaterials = in_rRenderMaterials.size();
	size_t nBoneRemaps = in_rRenderBoneRemaps.size();
	size_t nBones = in_rSkeleton.m_BoneIDs.size();

	std::vector< CRenderable_MaterialDX8 >::iterator ItMat = in_rRenderMaterials.begin();
	std::vector< CRenderable_BoneRemapDX8 >::iterator ItRemap = in_rRenderBoneRemaps.begin();
	for( ; ItMat!=in_rRenderMaterials.end() && ItRemap!=in_rRenderBoneRemaps.end(); ++ItMat,++ItRemap ) {
		CRenderable_BoneRemapDX8& rRemap = *ItRemap;
		size_t RemapSize = rRemap.m_UsedBones.size();
		//const unsigned char* pRemap = &(rRemap.m_UsedBones[0]);

		CRenderable_MaterialDX8& rMaterial = *ItMat;
		unsigned long NPasses = m_PixelShaderManager.UsePixelShader( rMaterial.m_ShaderHandle );
		assert(NPasses>0);
		for(unsigned long i=0; i<NPasses; ++i) {
			m_PixelShaderManager.ApplyPixelShaderPass(i);
			for( std::vector< CRenderable_GeometryDX8 >::iterator ItGeom=rRemap.m_Geometries.begin(); ItGeom!=rRemap.m_Geometries.end(); ++ItGeom ) {
				CRenderable_GeometryDX8& rGeometry = *ItGeom;

				for( std::vector< CRenderable_RenderCallDX8 >::iterator ItRenderCall=rGeometry.m_RenderCalls.begin(); ItRenderCall!=rGeometry.m_RenderCalls.end(); ++ItRenderCall ) {
					CRenderable_RenderCallDX8& rCall = *ItRenderCall;
					assert( rCall.m_VB>=0 && rCall.m_VB<(long)rGeometry.m_VertexBuffers.size() );

					CRenderable_VertexBufferDX8& rVB = rGeometry.m_VertexBuffers[rCall.m_VB];
					Res = m_pD3D8Device->SetStreamSource( 0, rVB.m_pVB, rVB.m_VertexSize );
					assert(Res==D3D_OK);
					if (rCall.m_IB > -1 ) 
					{
						CRenderable_IndexBufferDX8& rIB = rGeometry.m_IndexBuffers[rCall.m_IB];
						Res = m_pD3D8Device->SetIndices(rIB.m_pIB, rCall.m_BaseVertex);
						assert(Res == S_OK);
						ApplyVertexShader( rVB.m_FVF, in_M, in_rSkeleton, rRemap.m_UsedBones );
						Res = m_pD3D8Device->DrawIndexedPrimitive( rCall.m_PrimitiveType, rCall.m_BaseVertex, rVB.m_NVertices,rCall.m_BaseVertexIndex, rCall.m_NPrimitives );
						assert(Res==D3D_OK);
					}
					else
					{
						ApplyVertexShader( rVB.m_FVF, in_M, in_rSkeleton, rRemap.m_UsedBones );
						Res = m_pD3D8Device->DrawPrimitive( rCall.m_PrimitiveType, rCall.m_BaseVertex, rCall.m_NPrimitives );
						assert(Res==D3D_OK);
					}
				}

			}
		}

		// DEBUG {
		// skip all remaps after the first one
		//break;
		// } DEBUG
	}

	m_pD3D8Device->EndScene();
}

void
CRenderDX8::FillMaterials(
	const std::vector< CRenderable_AbstractMaterial >& in_rAbstractMaterials,
	std::vector< CRenderable_MaterialDX8 >& in_rRenderMaterials
)
{
	assert( in_rAbstractMaterials.size() != in_rRenderMaterials.size() );
	assert( in_rRenderMaterials.empty() );

	in_rRenderMaterials.resize( in_rAbstractMaterials.size() );
	std::vector< CRenderable_AbstractMaterial >::const_iterator ItSrc=in_rAbstractMaterials.begin();
	std::vector< CRenderable_MaterialDX8 >::iterator ItDst=in_rRenderMaterials.begin();
	for( ; ItSrc!=in_rAbstractMaterials.end() && ItDst!=in_rRenderMaterials.end(); ++ItSrc,++ItDst ) {
		// FUCK
		//CRenderable_AbstractMaterial Mat;
		//Mat.m_ARGB = 0xffffffff;
		//Mat.m_DiffuseSource = CRenderable_AbstractMaterial::DS_CONSTANT;
		//const CRenderable_AbstractMaterial& rMaterialSrc = Mat;
		const CRenderable_AbstractMaterial& rMaterialSrc = *ItSrc;
		CRenderable_MaterialDX8& rMaterialDst = *ItDst;

		std::vector< unsigned long > TextureHandles;
		rMaterialDst.m_ShaderHandle = m_PixelShaderManager.CreatePixelShader( rMaterialSrc, TextureHandles );
		rMaterialDst.m_Textures.resize( TextureHandles.size() );
		for( size_t i=0; i<TextureHandles.size(); ++i)
			rMaterialDst.m_Textures[i].Set( TextureHandles[i], &m_ObjectTextures );
	}
}

void
CRenderDX8::FillGeometries(
	const std::vector< CRenderable_BoneRemap >& in_rAbstractBoneRemaps,
	std::vector< CRenderable_BoneRemapDX8 >& in_rRenderBoneRemaps
)
{
	assert( in_rAbstractBoneRemaps.size() != in_rRenderBoneRemaps.size() );

	assert( in_rRenderBoneRemaps.empty() );
	in_rRenderBoneRemaps.resize( in_rAbstractBoneRemaps.size() );

	std::vector< CRenderable_BoneRemap >::const_iterator ItSrc=in_rAbstractBoneRemaps.begin();
	std::vector< CRenderable_BoneRemapDX8 >::iterator ItDst=in_rRenderBoneRemaps.begin();
	for( ; ItSrc!=in_rAbstractBoneRemaps.end() && ItDst!=in_rRenderBoneRemaps.end(); ++ItSrc,++ItDst ) {
		const CRenderable_BoneRemap& rSrcRemap = *ItSrc;
		CRenderable_BoneRemapDX8& rDstRemap = *ItDst;

		rDstRemap.m_UsedBones = rSrcRemap.m_UsedBones;

		assert( rDstRemap.m_Geometries.empty() );
		rDstRemap.m_Geometries.resize( rSrcRemap.m_Geometries.size() );

		std::vector< CRenderable_AbstractGeometry >::const_iterator ItSrcGeom=rSrcRemap.m_Geometries.begin();
		std::vector< CRenderable_GeometryDX8 >::iterator ItDstGeom=rDstRemap.m_Geometries.begin();
		for( ; ItSrcGeom!=rSrcRemap.m_Geometries.end() && ItDstGeom!=rDstRemap.m_Geometries.end(); ++ItSrcGeom,++ItDstGeom ) {

			const CRenderable_AbstractGeometry& rGeometrySrc = *ItSrcGeom;
			CRenderable_GeometryDX8& rGeometryDst = *ItDstGeom;

			// fill in render calls
			rGeometryDst.m_RenderCalls.resize( rGeometrySrc.m_RenderCalls.size() );
			std::vector< CRenderable_AbstractRenderCall >::const_iterator ItSrcRC = rGeometrySrc.m_RenderCalls.begin();
			std::vector< CRenderable_RenderCallDX8 >::iterator ItDstRC = rGeometryDst.m_RenderCalls.begin();
			for( ; ItSrcRC!=rGeometrySrc.m_RenderCalls.end() && ItDstRC!=rGeometryDst.m_RenderCalls.end(); ++ItSrcRC,++ItDstRC ) {
				const CRenderable_AbstractRenderCall& rSrcRC = *ItSrcRC;
				CRenderable_RenderCallDX8& rDstRC = *ItDstRC;
				rDstRC.m_VB = rSrcRC.m_VB;
				rDstRC.m_BaseVertex = rSrcRC.m_BaseVertex;
				rDstRC.m_IB = rSrcRC.m_IB;
				rDstRC.m_BaseVertexIndex = rSrcRC.m_BaseVertexIndex;
				rDstRC.m_NPrimitives = rSrcRC.m_NPrimitives;
				rDstRC.m_PrimitiveType = ConvertPrimitiveType(rSrcRC.m_PrimitiveType);
			}

			// create index buffers
			rGeometryDst.m_IndexBuffers.resize( rGeometrySrc.m_IndexBuffers.size() );
			std::vector< CRenderable_AbstractIndexBuffer >::const_iterator ItSrcIB = rGeometrySrc.m_IndexBuffers.begin();
			std::vector< CRenderable_IndexBufferDX8 >::iterator ItDstIB = rGeometryDst.m_IndexBuffers.begin();
			for( ; ItSrcIB!=rGeometrySrc.m_IndexBuffers.end() && ItDstIB!=rGeometryDst.m_IndexBuffers.end(); ++ItSrcIB,++ItDstIB ) {
				const CRenderable_AbstractIndexBuffer& rSrcIB = *ItSrcIB;
				CRenderable_IndexBufferDX8& rDstIB = *ItDstIB;
			
				unsigned long TrueDataSize = rSrcIB.m_IndexData.size()*sizeof(long);
				assert(TrueDataSize > 0);
				unsigned long UsageWriteOnly = D3DUSAGE_WRITEONLY;
				unsigned long UsageVertexProcessing = (m_VertexProcessType==RVP_SOFTWARE) ? D3DUSAGE_SOFTWAREPROCESSING : 0;
				unsigned long Usage = UsageWriteOnly | UsageVertexProcessing;
				HRESULT Res = m_pD3D8Device->CreateIndexBuffer( rSrcIB.m_IndexData.size() *sizeof(DWORD),
											Usage, D3DFMT_INDEX32,
											D3DPOOL_DEFAULT, &rDstIB.m_pIB ) ;
				// fill in
				BYTE* pData = NULL;
				Res = rDstIB.m_pIB->Lock( 0, TrueDataSize, &pData, 0 );
				assert( Res == D3D_OK );
				assert( pData != NULL );
				memcpy( pData, &(rSrcIB.m_IndexData[0]), TrueDataSize );
				Res = rDstIB.m_pIB->Unlock();
				assert( Res == D3D_OK );
				//
				
			}


			// create and fill vertex buffers
			rGeometryDst.m_VertexBuffers.resize( rGeometrySrc.m_VertexBuffers.size() );
			std::vector< CRenderable_AbstractVertexBuffer >::const_iterator ItSrcVB = rGeometrySrc.m_VertexBuffers.begin();
			std::vector< CRenderable_VertexBufferDX8 >::iterator ItDstVB = rGeometryDst.m_VertexBuffers.begin();
			for( ; ItSrcVB!=rGeometrySrc.m_VertexBuffers.end() && ItDstVB!=rGeometryDst.m_VertexBuffers.end(); ++ItSrcVB,++ItDstVB ) {
				const CRenderable_AbstractVertexBuffer& rSrcVB = *ItSrcVB;
				CRenderable_VertexBufferDX8& rDstVB = *ItDstVB;
				unsigned long FVF = 0;
				unsigned long VertexSize = 0;
				CalcFVFandVertexSize( rSrcVB.m_VertexFormat, FVF, VertexSize );
				assert( VertexSize >= 3*sizeof(float) );
				unsigned long NVertices = (unsigned long)rSrcVB.m_VertexData.size() / VertexSize;
				unsigned long TrueDataSize = VertexSize*NVertices;
				unsigned long UsageWriteOnly = D3DUSAGE_WRITEONLY;
				unsigned long UsageVertexProcessing = (m_VertexProcessType==RVP_SOFTWARE) ? D3DUSAGE_SOFTWAREPROCESSING : 0;
				unsigned long Usage = UsageWriteOnly | UsageVertexProcessing;
				HRESULT Res = m_pD3D8Device->CreateVertexBuffer(
					TrueDataSize,
					Usage,
					FVF,
					D3DPOOL_MANAGED,
					&(rDstVB.m_pVB)
				);
				assert( Res == D3D_OK );
				assert( rDstVB.m_pVB != NULL );
				// fill in
				BYTE* pData = NULL;
				Res = rDstVB.m_pVB->Lock( 0, TrueDataSize, &pData, 0 );
				assert( Res == D3D_OK );
				assert( pData != NULL );
				memcpy( pData, &(rSrcVB.m_VertexData[0]), TrueDataSize );
				Res = rDstVB.m_pVB->Unlock();
				assert( Res == D3D_OK );
				//
				rDstVB.m_FVF = FVF;
				rDstVB.m_NVertices = NVertices;
				rDstVB.m_VertexSize = VertexSize;
			}
		}
	}
}























