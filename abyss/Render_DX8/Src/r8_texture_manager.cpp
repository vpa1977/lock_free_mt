#include "../Include/r8_texture_manager.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

CRenderDX8TextureManager::CRenderDX8TextureManager() :
	m_pD3D8(NULL),
	m_pD3D8Device(NULL),
	m_RenderFormat(D3DFMT_UNKNOWN),
	m_PreferredTextureFormat(D3DFMT_UNKNOWN),
	m_PreferredTextureFormatAlpha(D3DFMT_UNKNOWN)
{
}

CRenderDX8TextureManager::~CRenderDX8TextureManager()
{
	assert( !m_pD3D8 && !m_pD3D8Device && m_RenderFormat==D3DFMT_UNKNOWN );
	assert( m_TexMap.empty() );
}

void
CRenderDX8TextureManager::ReleaseEverything()
{
	for( CTextureInfoMapIterator It=m_TexMap.begin(); It!=m_TexMap.end(); ++It )
	{
		It->second.m_pTex->Release();
	}
	m_TexMap.clear();
	m_TexturesHandlesStorage.Clear();
}

void
CRenderDX8TextureManager::SetInterface( IDirect3D8* in_pD3D8, IDirect3DDevice8* in_pD3D8Device, D3DFORMAT in_RenderFormat )
{
	assert(
		(in_pD3D8 && in_pD3D8Device && (in_RenderFormat==D3DFMT_X8R8G8B8||in_RenderFormat==D3DFMT_R5G6B5)) ||
		(!in_pD3D8 && !in_pD3D8Device && in_RenderFormat==D3DFMT_UNKNOWN)
	);
	if(in_pD3D8)
	{
		assert( !m_pD3D8 && !m_pD3D8Device && m_RenderFormat==D3DFMT_UNKNOWN );
		m_pD3D8 = in_pD3D8;
		m_pD3D8Device = in_pD3D8Device;
		m_RenderFormat = in_RenderFormat;
		ChooseTextureFormats();
	}
	else
	{
		assert(m_TexMap.empty());
		m_pD3D8 = NULL;
		m_pD3D8Device = NULL;
		m_RenderFormat = D3DFMT_UNKNOWN;
		m_PreferredTextureFormat = D3DFMT_UNKNOWN;
		m_PreferredTextureFormatAlpha = D3DFMT_UNKNOWN;
	}
}

unsigned long
CRenderDX8TextureManager::CreateTexture(
	unsigned long in_DX
	,unsigned long in_DY
	// TODO: insert in_InputFormat. ATM input is always 32bit (X8R8G8B8 или A8R8G8B8)
	,bool in_HasAlpha // TODO: get rid of this once in_InputFormat is implemented.
	// TODO: implement bool in_InputHasMipmaps
	// TODO: implement bool in_GenerateMipmaps
	,const void* in_pData, unsigned long in_DataLength
)
{
//CLog::Print("CRenderDX8TextureManager::CreateTexture()\n");
//CLog::Print("  dx = %lu\n",in_DX);
//CLog::Print("  dy = %lu\n",in_DY);
//CLog::Print("  sizebytes = %lu\n",in_DataLength);
	assert(m_pD3D8Device);
	assert(in_pData!=NULL);
	assert(in_DX>0);
	assert(in_DY>0);
	assert(in_DataLength >= in_DX*in_DY*sizeof(unsigned long));
	assert(D3DFMT_X8R8G8B8==m_PreferredTextureFormat);
	assert(D3DFMT_A8R8G8B8==m_PreferredTextureFormatAlpha);

//	const unsigned long* pFirstPixel = (const unsigned long*)in_pData;
//CLog::Print("  first pixel = %08x\n",*pFirstPixel);

	// create
	IDirect3DTexture8* pTex = NULL;
	DWORD Usage = 0;
	HRESULT Res = m_pD3D8Device->CreateTexture(
		in_DX, in_DY,
		1,
		Usage,
		in_HasAlpha ? m_PreferredTextureFormatAlpha : m_PreferredTextureFormat,
		D3DPOOL_MANAGED,
		&pTex
	);
	assert( Res == D3D_OK );
	assert( pTex != NULL ); // TODO: report error
CLog::Print("  generated %lu mipmap levels\n",pTex->GetLevelCount());

	// fill in
	D3DLOCKED_RECT LockedRect;
	//Res = pTex->LockRect( 0, &LockedRect, NULL, D3DLOCK_DISCARD );
	Res = pTex->LockRect( 0, &LockedRect, NULL, 0 );
	assert( Res == D3D_OK );
	assert(LockedRect.pBits!=NULL);

	// TODO: tex format can be non-32bit
//CLog::Print("  pitch = %lu bytes\n",LockedRect.Pitch);
	assert(LockedRect.Pitch>=in_DX*sizeof(unsigned long));
	BYTE* pDataDst = (BYTE*)LockedRect.pBits;
	unsigned long* pDataSrc = (unsigned long*)in_pData;
	for(unsigned long Y=0; Y<in_DY; ++Y)
	{
//CLog::Print("  copying %lu bytes to offset %lu\n",in_DX*sizeof(unsigned long),pDataDst-(BYTE*)LockedRect.pBits);
		memcpy(pDataDst,pDataSrc,in_DX*sizeof(unsigned long));
		pDataDst += LockedRect.Pitch;
		pDataSrc += in_DX;
	}

	Res = pTex->UnlockRect( 0 );
	assert( Res == D3D_OK );

	// remember it
	unsigned long TexHandle = m_TexturesHandlesStorage.GetNewHandle();
	assert(TexHandle>0);
	CTextureInfoMapInsertResult TIR = m_TexMap.insert(
		CTextureInfoMapValueType(
			TexHandle,
			CTextureInfo(
				in_DX, in_DY,
				in_HasAlpha ? m_PreferredTextureFormatAlpha : m_PreferredTextureFormat,
				pTex
			)
		)
	);
	assert( TIR.second == true );

	return TexHandle;
}

void
CRenderDX8TextureManager::ReleaseTexture( unsigned long in_TextureHandle )
{
	CTextureInfoMapIterator It = m_TexMap.find( in_TextureHandle );
	assert( It != m_TexMap.end() );
	It->second.m_pTex->Release();
	m_TexMap.erase( It );
	m_TexturesHandlesStorage.ReleaseHandle( in_TextureHandle );
}

IDirect3DTexture8*
CRenderDX8TextureManager::GetTexture( unsigned long in_TextureHandle )
{
	CTextureInfoMapIterator It = m_TexMap.find( in_TextureHandle );
	if( It != m_TexMap.end() )
	{
		return It->second.m_pTex;
	}
	else
		return NULL;
}

void
CRenderDX8TextureManager::ChooseTextureFormats()
{
	CLog::Print("  ChooseTextureFormats()\n");
	assert(m_pD3D8);
	assert(m_pD3D8Device);
	assert(m_RenderFormat==D3DFMT_X8R8G8B8 || m_RenderFormat==D3DFMT_R5G6B5);
	bool Ok;
	// without alpha
	Ok = CheckTextureFormat(D3DFMT_X8R8G8B8);
	assert(Ok);
	m_PreferredTextureFormat = D3DFMT_X8R8G8B8;
	CLog::Print("    D3DFMT_X8R8G8B8    - %s\n",Ok?"YES":"no");
	Ok = CheckTextureFormat(D3DFMT_R8G8B8);
	CLog::Print("    D3DFMT_R8G8B8      - %s\n",Ok?"YES":"no");
	Ok = CheckTextureFormat(D3DFMT_R5G6B5);
	CLog::Print("    D3DFMT_R5G6B5      - %s\n",Ok?"YES":"no");
	Ok = CheckTextureFormat(D3DFMT_X1R5G5B5);
	CLog::Print("    D3DFMT_X1R5G5B5    - %s\n",Ok?"YES":"no");
	Ok = CheckTextureFormat(D3DFMT_X4R4G4B4);
	CLog::Print("    D3DFMT_X4R4G4B4    - %s\n",Ok?"YES":"no");
	// with alpha
	Ok = CheckTextureFormat(D3DFMT_A8R8G8B8);
	assert(Ok);
	m_PreferredTextureFormatAlpha = D3DFMT_A8R8G8B8;
	CLog::Print("    D3DFMT_A8R8G8B8    - %s\n",Ok?"YES":"no");
	Ok = CheckTextureFormat(D3DFMT_A4R4G4B4);
	CLog::Print("    D3DFMT_A4R4G4B4    - %s\n",Ok?"YES":"no");
	Ok = CheckTextureFormat(D3DFMT_A8R3G3B2);
	CLog::Print("    D3DFMT_A8R3G3B2    - %s\n",Ok?"YES":"no");
	Ok = CheckTextureFormat(D3DFMT_A2B10G10R10);
	CLog::Print("    D3DFMT_A2B10G10R10 - %s\n",Ok?"YES":"no");
}

inline
bool
CRenderDX8TextureManager::CheckTextureFormat(D3DFORMAT in_TextureFormat)
{
	assert(m_pD3D8);
	assert(m_RenderFormat==D3DFMT_X8R8G8B8 || m_RenderFormat==D3DFMT_R5G6B5);
	return D3D_OK==m_pD3D8->CheckDeviceFormat(	D3DADAPTER_DEFAULT,
												D3DDEVTYPE_HAL,
												m_RenderFormat,
												0,
												D3DRTYPE_TEXTURE,
												in_TextureFormat );
}

























