#ifndef GEOMETRY_DX8_HPP_INCLUDED
#define GEOMETRY_DX8_HPP_INCLUDED

#include <boost/shared_ptr.hpp>
#include <d3d8.h>

struct CRenderable_VertexBufferDX8 {
	CRenderable_VertexBufferDX8() : m_pVB(NULL), m_VertexSize(0), m_FVF(0), m_NVertices(0) {}
	~CRenderable_VertexBufferDX8() {
		if(m_pVB) {
			ULONG r = m_pVB->Release();
			assert(0==r);
		}
	}
	IDirect3DVertexBuffer8* m_pVB;
	unsigned long m_VertexSize;
	unsigned long m_FVF;
	unsigned long m_NVertices;
};

struct CRenderable_IndexBufferDX8 {
	CRenderable_IndexBufferDX8() : m_pIB(NULL) {}
	~CRenderable_IndexBufferDX8() {
		if(m_pIB) {
			ULONG r = m_pIB->Release();
			assert(0==r);
		}
	}
	IDirect3DIndexBuffer8* m_pIB;
};

struct CRenderable_RenderCallDX8 {
	long m_VB;
	long m_BaseVertex;
	long m_IB;
	long m_BaseVertexIndex;
	long m_NPrimitives;
	D3DPRIMITIVETYPE m_PrimitiveType;
};

struct CRenderable_GeometryDX8 {
	std::vector< CRenderable_VertexBufferDX8 > m_VertexBuffers;
	std::vector< CRenderable_IndexBufferDX8  > m_IndexBuffers;
	std::vector< CRenderable_RenderCallDX8   > m_RenderCalls;
};

#endif















