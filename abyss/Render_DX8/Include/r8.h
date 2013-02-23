#ifndef R8_HPP_INCLUDED
#define R8_HPP_INCLUDED

#include "../../Objects/Include/Render/renderable.h"
#include "renderable_dx8.h"
#include "skeleton_dx8.h"

#include "r8_object_textures.h"

#include "algebra.h"
#include "../../Common/Include/vb_format.h"
#include "r8_vertex_process_flags.h"
#include "../../Common/Include/primitive_types.h"
#include "handle_storage.h"
#include "render_pixel_shader.h"
#include "r8_texture_manager.h"
#include "r8_ps_manager.h"
#include "r8_font_info.h"
//#include "r8_vs_manager.h"
#include "r8_settings.h"

#include <d3d8.h>

#include <map>

struct CSymbolInfo
{
//	CSymbolInfo() : m_X(0.0f), m_Y(0.0f), m_Width(0.0f) {}
	float m_X;
	float m_Y;
	float m_Width;
};

class CRenderDX8
{
public:
	CRenderDX8();
	~CRenderDX8();
	void SetInterface(IDirect3D8* in_pD3D8, IDirect3DDevice8* in_pD3D8Device, D3DFORMAT in_RenderFormat, RENDER_VERTEX_PROCESS_TYPE in_VertexProcessType );
	void ReleaseEverything();
	//
	bool Present(); // true - Ok, false - device lost.

	void PrepareToRestoreDevice();
	void RestoreResources();

	void Clear(unsigned long in_Color);

	// viewport
	void SetViewport(	unsigned long X, unsigned long Y,
						unsigned long Width, unsigned long Height,
						float MinZ, float MaxZ	);
	// camera
	void SetCamera(	const CVector& in_Org, const CVector& in_Dir, const CVector& in_Right,
					float in_FovX, float in_FovY,
					float in_Znear, float in_Zfar );
	void SetViewMatrix( const CMatrix& in_View );
	void SetProjectionMatrix( const CMatrix& in_Proj );

	// lights
	void SetAmbient( unsigned long in_AmbientColor );
	void SetDirectionalLight( const CVector& in_Dir, float in_R, float in_G, float in_B );
	void SetPointLight( const CVector& in_Org, float in_R, float in_G, float in_B );

	// textures
	unsigned long CreateTexture(
		unsigned long in_DX
		,unsigned long in_DY
		// TODO: insert in_InputFormat. ATM input is always 32bit (X8R8G8B8 or A8R8G8B8)
		,bool in_HasAlpha // TODO: get rid of this once in_InputFormat is implemented.
		// TODO: implement bool in_InputHasMipmaps
		// TODO: implement bool in_GenerateMipmaps
		,const void* in_pData, unsigned long in_DataLength
	);
	void ReleaseTexture( unsigned long in_TextureHandle );

	// buffers
	unsigned long CreateVertexBuffer(
		const void* in_pData,
		unsigned long in_DataLength,
		const CVBFormat& in_Format,
		bool in_bDynamic
	);
	bool UpdateVertexBuffer(
		unsigned long in_Handle,
		const void* in_pData,
		unsigned long in_DataLength
	);
	void ReleaseVertexBuffer( unsigned long in_VertexBufferHandle );

	// shaders
	unsigned long CreatePixelShader( const CPixelShader& in_rShaderDescription );
	void ReleasePixelShader( unsigned long in_PixelShaderHandle );

	// fonts
	unsigned long CreateFont( const CFontDescription& in_rFontDescription );

	// rendering
	void DrawTestObject( const CMatrix& in_M );
	void DrawVB(
		const CMatrix& in_M,
		unsigned long in_VBHandle,
		unsigned long in_FirstVertex,
		unsigned long in_PrimitiveType,
		unsigned long in_PrimitiveCount
	);
	void DrawVB(
		const CMatrix& in_M,
		unsigned long in_VBHandle,
		unsigned long in_PSHandle,
		unsigned long in_FirstVertex,
		unsigned long in_PrimitiveType,
		unsigned long in_PrimitiveCount
	);
	void DrawString(
		long in_X,
		long in_Y,
		const char* in_pszString,
		unsigned long in_FontHandle,
		unsigned long in_Color
	);
	void DrawDot(
		long in_X,
		long in_Y,
		unsigned long in_Color
	);
	void DrawRenderables(
		const CMatrix& in_M,

		const std::vector< CRenderable_AbstractMaterial >& in_rAbstractMaterials,
		const std::vector< CRenderable_BoneRemap >& in_rAbstractBoneRemaps,

		std::vector< CRenderable_MaterialDX8 >& in_rRenderMaterials,
		std::vector< CRenderable_BoneRemapDX8 >& in_rRenderBoneRemaps,

		const CSkeleton_DX8& in_rSkeleton
	);
private:
	// device
	IDirect3D8*					m_pD3D8;
	IDirect3DDevice8*			m_pD3D8Device;
	D3DFORMAT					m_RenderFormat;
	RENDER_VERTEX_PROCESS_TYPE	m_VertexProcessType;

	CMatrix m_ViewMatrix;
	CMatrix m_ProjMatrix;

	// vertex buffers
	CHandleStorage m_VertexBuffersHandlesStorage;
	struct CVBInfo
	{
		CVBInfo() : m_bDynamic(false), m_FVF(0), m_VertexSize(0), m_BufferSizeBytes(0), m_NVertices(0), m_pVB(NULL) {}
		CVBInfo( bool in_bDynamic, unsigned long in_FVF, unsigned long in_VertexSize, unsigned long in_BufferSizeBytes, unsigned long in_NVertices, IDirect3DVertexBuffer8* in_pVB ) :
			m_bDynamic(in_bDynamic),
			m_FVF(in_FVF),
			m_VertexSize(in_VertexSize),
			m_BufferSizeBytes(in_BufferSizeBytes),
			m_NVertices(in_NVertices),
			m_pVB(in_pVB)
		{}
		bool m_bDynamic;
		std::vector<unsigned long> m_Data;
		unsigned long m_FVF;
		unsigned long m_VertexSize;
		unsigned long m_BufferSizeBytes;
		unsigned long m_NVertices;
		IDirect3DVertexBuffer8* m_pVB;
	};
	typedef std::map< unsigned long, CVBInfo > CVBInfoMap;
	typedef CVBInfoMap::value_type CVBInfoMapValueType;
	typedef CVBInfoMap::iterator CVBInfoMapIterator;
	typedef std::pair<CVBInfoMapIterator,bool> CVBInfoMapInsertResult;
	CVBInfoMap m_VBMap;
	void RestoreVertexBuffers();

	static void CalcFVFandVertexSize( const CVBFormat&, unsigned long&, unsigned long& );

//	// index buffers
//	CHandleStorage m_IndexBuffersHandlesStorage;

	// textures
	CRenderDX8TextureManager m_TextureManager;

	//
	CRenderDX8_ObjectsTexturesContainer m_ObjectTextures;

	// pixel shaders
	CRenderDX8PixelShaderManager m_PixelShaderManager;

	// fonts
	CHandleStorage m_FontsHandlesStorage;
	struct CFontInfo
	{
		CFontInfo() {}
		CFontInfo( bool in_bInternal, long in_Height, const std::vector< CSymbolInfo >& in_Symbols, unsigned long in_TextureHandle, unsigned long in_TextureSize, unsigned long in_ShaderHandle ) :
			m_bInternal(in_bInternal)
			,m_Height(in_Height)
			,m_Symbols(in_Symbols)
			,m_TextureHandle(in_TextureHandle)
			,m_TextureSize(in_TextureSize)
			,m_ShaderHandle(in_ShaderHandle)
		{}
		bool m_bInternal;
		long m_Height;
		std::vector< CSymbolInfo > m_Symbols;
		unsigned long m_TextureHandle;
		unsigned long m_TextureSize;
		unsigned long m_ShaderHandle;
	};
	typedef std::map< unsigned long, CFontInfo > CFontInfoMap;
	typedef CFontInfoMap::value_type CFontInfoMapValueType;
	typedef CFontInfoMap::iterator CFontInfoMapIterator;
	typedef std::pair<CFontInfoMapIterator,bool> CFontInfoMapInsertResult;
	CFontInfoMap m_FontsMap;
	void CreateInternalFont();

	unsigned long m_DotShaderHandle;
	void CreateDotShader();

	CRenderDX8_ObjectsTexturesContainer m_ObjectsTexturesContainer;
	void FillMaterials(
		const std::vector< CRenderable_AbstractMaterial >& in_rAbstractMaterials,
		std::vector< CRenderable_MaterialDX8 >& in_rRenderMaterials
	);
	void FillGeometries(
		const std::vector< CRenderable_BoneRemap >& in_rAbstractBoneRemaps,
		std::vector< CRenderable_BoneRemapDX8 >& in_rRenderBoneRemaps
	);


	// lights
	unsigned long GetLightsCode();

	static unsigned long GetTexturesCode(unsigned long in_FVF);


	//unsigned long m_VertexShader;

	struct CShaderDataKey
	{
		CShaderDataKey( unsigned long in_FVF=0, unsigned long in_LightsCode=0, unsigned long in_TexturesCode=0 ) :
			m_FVF(in_FVF)
			,m_LightsCode(in_LightsCode)
			,m_TexturesCode(in_TexturesCode)
		{}
		unsigned long m_FVF;
		unsigned long m_LightsCode;
		unsigned long m_TexturesCode;
		bool operator < (const CShaderDataKey& rOther) const {
			if( m_FVF!=rOther.m_FVF )
				return m_FVF<rOther.m_FVF;
			if( m_LightsCode!=rOther.m_LightsCode )
				return m_LightsCode<rOther.m_LightsCode;
			return m_TexturesCode<rOther.m_TexturesCode;
		}
	};
	struct CShaderDataValue
	{
		CShaderDataValue( const DWORD* in_pDeclaration=NULL, const DWORD* in_pCode=NULL, const DWORD* in_Constants=NULL ) :
			m_pDeclaration(in_pDeclaration)
			,m_pCode(in_pCode)
			,m_Constants(in_Constants)
			,m_bCreated(false)
			,m_Handle(NULL)
		{
			assert( m_pDeclaration && m_pCode && m_Constants );
		}
		const DWORD* m_pDeclaration;
		const DWORD* m_pCode;
		const DWORD* m_Constants;
		//
		bool m_bCreated;
		DWORD m_Handle;
	};
	typedef std::map< CShaderDataKey, CShaderDataValue > CShaderDataMap;
	CShaderDataMap m_ShaderDataArray;
	void SetupShaderDefinitions();
	void ApplyVertexShader( unsigned long in_FVF, const CMatrix& in_M, const CSkeleton_DX8& in_Skel, const std::vector<unsigned char>& in_UsedBones );
	void ApplyVertexShaderConstants( const DWORD* in_pConstants, const CMatrix& in_M, const CSkeleton_DX8& in_Skel, const std::vector<unsigned char>& in_UsedBones );
	//void SetupBoneRecurse( const CBone_DX8* pBone, const CMatrix& rParentM );
};

#endif









