#include "../../MT_Core/Include/log.h"
#include "../Include/r8.h"
#include <assert.h>

unsigned long
GetTextureFVF(unsigned long in_Index, unsigned long in_TextureCoordinates)
{
	//CLog::Print("CRenderDX8::GetTextureFVF( %lu, %lu )\n",in_Index,in_TextureCoordinates);

	//assert(in_Index>=0 && in_Index<=3);
	assert(in_Index>=0 && in_Index<=1);

	//assert(in_TextureCoordinates>=1 && in_TextureCoordinates<=3);
	assert(in_TextureCoordinates==2); // TODO

	switch(in_TextureCoordinates)
	{
		case 1:
			return ( D3DFVF_TEXTUREFORMAT1 << (in_Index*2 + 16) );
		case 2:
			return ( D3DFVF_TEXTUREFORMAT2 << (in_Index*2 + 16) );
		case 3:
			return ( D3DFVF_TEXTUREFORMAT3 << (in_Index*2 + 16) );
	}
	return 0;
}

unsigned long
CRenderDX8::CreateVertexBuffer( const void* in_pData, unsigned long in_DataLength, const CVBFormat& in_Format, bool in_bDynamic )
{
CLog::Print("CRenderDX8::CreateVertexBuffer()\n");
	// TEST {
	in_bDynamic = true;
	// } TEST

	assert( m_pD3D8Device );
	assert( in_pData );
	assert( !(in_Format.m_XYZ && in_Format.m_XYZRHW) && !(!in_Format.m_XYZ && !in_Format.m_XYZRHW) );
	assert( !(in_Format.m_XYZRHW && in_Format.m_Normal) );
	bool Tex0Present = in_Format.m_Tex0Dimension!=CVBFormat::TEXCOORDSET_NONE;
	bool Tex1Present = in_Format.m_Tex1Dimension!=CVBFormat::TEXCOORDSET_NONE;
	bool Tex2Present = in_Format.m_Tex2Dimension!=CVBFormat::TEXCOORDSET_NONE;
	bool Tex3Present = in_Format.m_Tex3Dimension!=CVBFormat::TEXCOORDSET_NONE;
	assert(
		(!Tex0Present&&!Tex1Present&&!Tex2Present&&!Tex3Present) ||
		( Tex0Present&&!Tex1Present&&!Tex2Present&&!Tex3Present) ||
		( Tex0Present&& Tex1Present&&!Tex2Present&&!Tex3Present) ||
		( Tex0Present&& Tex1Present&& Tex2Present)
	);

	// compute FVF
	// and vertex size
	unsigned long FVF = 0;
	unsigned long VertexSize = 0;
	CalcFVFandVertexSize( in_Format, FVF, VertexSize );
	assert(in_DataLength>=VertexSize);

	// create
	IDirect3DVertexBuffer8* pVertexBuffer = NULL;
	unsigned long UsageWriteOnly = D3DUSAGE_WRITEONLY;
	unsigned long UsageDynamic = in_bDynamic ? D3DUSAGE_DYNAMIC : 0;
	unsigned long UsageVertexProcessing = (m_VertexProcessType==RVP_SOFTWARE) ? D3DUSAGE_SOFTWAREPROCESSING : 0;
	unsigned long Usage = UsageWriteOnly | UsageDynamic | UsageVertexProcessing;
	D3DPOOL Pool = in_bDynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
	HRESULT Res = m_pD3D8Device->CreateVertexBuffer(
		in_DataLength,
		Usage,
		FVF,
		Pool,
		&pVertexBuffer
	);
	assert( Res == D3D_OK );
	assert( pVertexBuffer != NULL );

	// fill in
	BYTE* pData = NULL;
	Res = pVertexBuffer->Lock( 0, in_DataLength, &pData, 0 );
	assert( Res == D3D_OK );
	assert( pData != NULL );
	memcpy( pData, in_pData, in_DataLength );
	Res = pVertexBuffer->Unlock();
	assert( Res == D3D_OK );

	// remember it
	unsigned long VBHandle = m_VertexBuffersHandlesStorage.GetNewHandle();
	assert(VBHandle>0);
	CVBInfoMapInsertResult VBIR = m_VBMap.insert(
		CVBInfoMapValueType(
			VBHandle,
			CVBInfo(
				in_bDynamic,
				FVF,
				VertexSize,
				in_DataLength,
				in_DataLength/VertexSize,
				pVertexBuffer
			)
		)
	);
	assert( VBIR.second == true );

	// create shadow copy
	if(in_bDynamic)
	{
		CVBInfoMapIterator It = VBIR.first;
		std::vector<unsigned long>& rData = It->second.m_Data;
		rData.resize( in_DataLength );
		memcpy( &rData[0], in_pData, in_DataLength );
	}

	return VBHandle;
}

bool
CRenderDX8::UpdateVertexBuffer
( unsigned long in_Handle, const void* in_pData, unsigned long in_DataLength )
{
//	CLog::Print("CRenderDX8::UpdateVertexBuffer()\n");
	assert(in_Handle!=0);
	assert(in_pData);
	assert(in_DataLength>0);
	CVBInfoMapIterator It = m_VBMap.find(in_Handle);
	assert( It != m_VBMap.end() );
	CVBInfo& rVBInfo = It->second;
	assert( rVBInfo.m_bDynamic );
	if (rVBInfo.m_BufferSizeBytes < in_DataLength) // recreate buffer.
	{
		rVBInfo.m_pVB->Release();
		rVBInfo.m_pVB = NULL;
		unsigned long UsageWriteOnly = D3DUSAGE_WRITEONLY;
		unsigned long UsageDynamic = rVBInfo.m_bDynamic ? D3DUSAGE_DYNAMIC : 0;
		unsigned long UsageVertexProcessing = (m_VertexProcessType==RVP_SOFTWARE) ? D3DUSAGE_SOFTWAREPROCESSING : 0;
		unsigned long Usage = UsageWriteOnly | UsageDynamic | UsageVertexProcessing;
		
		HRESULT Res = m_pD3D8Device->CreateVertexBuffer(
				in_DataLength,
				Usage,
				rVBInfo.m_FVF,
				rVBInfo.m_bDynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED,
				&rVBInfo.m_pVB
			);
		if (Res == D3DERR_OUTOFVIDEOMEMORY ) 
		{
			assert(0 && "What ? ");
		}
		if (Res == E_OUTOFMEMORY) 
		{
			assert( 0 && "What ?");

		}
		if (Res == D3DERR_INVALIDCALL ) 
		{
			assert(0 && "What really? ");
		}
		assert( Res == D3D_OK);
			
		rVBInfo.m_BufferSizeBytes = in_DataLength;
		rVBInfo.m_NVertices = in_DataLength / rVBInfo.m_VertexSize;

	}
	assert( rVBInfo.m_BufferSizeBytes >= in_DataLength );
	if( rVBInfo.m_pVB )
	{
		// refill
		BYTE* pData = NULL;
		HRESULT Res = rVBInfo.m_pVB->Lock( 0, in_DataLength, &pData, 0 );
		assert( Res == D3D_OK );
		assert( pData != NULL );
		memcpy( pData, in_pData, in_DataLength );
		Res = rVBInfo.m_pVB->Unlock();
		assert( Res == D3D_OK );
	}
	// update shadow copy
	std::vector<unsigned long>& rData = rVBInfo.m_Data;
	rData.resize( in_DataLength );
	memcpy( &rData[0], in_pData, in_DataLength );
	return true;
}

void
CRenderDX8::ReleaseVertexBuffer( unsigned long in_VertexBufferHandle )
{
	CVBInfoMapIterator It = m_VBMap.find( in_VertexBufferHandle );
	assert( It != m_VBMap.end() );
	It->second.m_pVB->Release();
	m_VBMap.erase( It );
	m_VertexBuffersHandlesStorage.ReleaseHandle( in_VertexBufferHandle );
}

void
CRenderDX8::RestoreVertexBuffers()
{
	for( CVBInfoMapIterator It=m_VBMap.begin(); It!=m_VBMap.end(); ++It )
	{
		CVBInfo& rInfo = It->second;
		if(rInfo.m_bDynamic && rInfo.m_pVB==NULL)
		{
			// re-create VB
			unsigned long UsageWriteOnly = D3DUSAGE_WRITEONLY;
			unsigned long UsageDynamic = D3DUSAGE_DYNAMIC;
			unsigned long UsageVertexProcessing = (m_VertexProcessType==RVP_SOFTWARE) ? D3DUSAGE_SOFTWAREPROCESSING : 0;
			unsigned long Usage = UsageWriteOnly | UsageDynamic | UsageVertexProcessing;
			D3DPOOL Pool = D3DPOOL_DEFAULT;
			HRESULT Res = m_pD3D8Device->CreateVertexBuffer(
				rInfo.m_BufferSizeBytes,
				Usage,
				rInfo.m_FVF,
				Pool,
				&rInfo.m_pVB
			);
			assert( Res == D3D_OK );
			assert( rInfo.m_pVB != NULL );
			// re-fill VB
			BYTE* pData = NULL;
			Res = rInfo.m_pVB->Lock( 0, rInfo.m_BufferSizeBytes, &pData, 0 );
			assert( Res == D3D_OK );
			assert( pData != NULL );
			assert( rInfo.m_Data.size() == rInfo.m_BufferSizeBytes );
			memcpy( pData, &(rInfo.m_Data[0]), rInfo.m_BufferSizeBytes );
			Res = rInfo.m_pVB->Unlock();
			assert( Res == D3D_OK );
		}
	}
}

void
CRenderDX8::CalcFVFandVertexSize( const CVBFormat& in_Format, unsigned long& out_rFVF, unsigned long& out_rVertexSize ) {
	out_rFVF = 0;
	out_rVertexSize = 0;

	if(in_Format.m_NBonesPerVertex > 0) {
		assert(in_Format.m_XYZ);
		if( 1 == in_Format.m_NBonesPerVertex ) {
			out_rFVF = D3DFVF_XYZB1;
		} else if( 2 == in_Format.m_NBonesPerVertex ) {
			out_rFVF = D3DFVF_XYZB2;
		} else if( 3 == in_Format.m_NBonesPerVertex ) {
			out_rFVF = D3DFVF_XYZB3;
		} else if( 4 == in_Format.m_NBonesPerVertex ) {
			out_rFVF = D3DFVF_XYZB4;
		}
		out_rVertexSize = 3*sizeof(float);
		out_rVertexSize += (in_Format.m_NBonesPerVertex-1)*sizeof(float);
		out_rFVF |= D3DFVF_LASTBETA_UBYTE4;
		out_rVertexSize += sizeof(unsigned long);

	} else if(in_Format.m_XYZ) {
		out_rFVF = D3DFVF_XYZ;
		out_rVertexSize = 3*sizeof(float);

	} else {
		// in_Format.m_XYZRHW
		out_rFVF = D3DFVF_XYZRHW;
		out_rVertexSize = 4*sizeof(float);
	}

	if(in_Format.m_Normal) {
		out_rFVF |= D3DFVF_NORMAL;
		out_rVertexSize += 3*sizeof(float);
	}
	if(in_Format.m_Diffuse) {
		out_rFVF |= D3DFVF_DIFFUSE;
		out_rVertexSize += sizeof(unsigned long);
	}
	if(in_Format.m_Specular) {
		out_rFVF |= D3DFVF_SPECULAR;
		out_rVertexSize += sizeof(unsigned long);
	}
	unsigned long NTextureCoordSets = 0;
	if(in_Format.m_Tex0Dimension>0) {
		assert(in_Format.m_Tex0Dimension<=3);
		out_rFVF |= GetTextureFVF(0,in_Format.m_Tex0Dimension);
		out_rVertexSize += in_Format.m_Tex0Dimension*sizeof(float);
		NTextureCoordSets = 1;
		if(in_Format.m_Tex1Dimension>0) {
			assert(in_Format.m_Tex1Dimension<=3);
			out_rFVF |= GetTextureFVF(1,in_Format.m_Tex1Dimension);
			out_rVertexSize += in_Format.m_Tex1Dimension*sizeof(float);
			NTextureCoordSets = 2;

			assert(in_Format.m_Tex2Dimension==0);
			assert(in_Format.m_Tex3Dimension==0);
			//if(in_Format.m_Tex2Dimension>0) {
			//	assert(in_Format.m_Tex2Dimension<=3);
			//	out_rFVF |= GetTextureFVF(2,in_Format.m_Tex2Dimension);
			//	out_rVertexSize += in_Format.m_Tex2Dimension*sizeof(float);
			//	NTextureCoordSets = 3;
			//	if(in_Format.m_Tex3Dimension>0) {
			//		assert(in_Format.m_Tex3Dimension<=3);
			//		out_rFVF |= GetTextureFVF(3,in_Format.m_Tex3Dimension);
			//		out_rVertexSize += in_Format.m_Tex3Dimension*sizeof(float);
			//		NTextureCoordSets = 4;
			//	}
			//}
		}
	}
	out_rFVF |= (NTextureCoordSets<<D3DFVF_TEXCOUNT_SHIFT);
}
















