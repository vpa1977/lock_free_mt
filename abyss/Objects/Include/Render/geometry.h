#ifndef OBJECT_GEOMETRY_HPP_INCLUDED
#define OBJECT_GEOMETRY_HPP_INCLUDED

#include "../../../Common/Include/vb_format.h"
#include "boost/shared_ptr.hpp"
#include <vector>


struct CRenderable_AbstractVertexBuffer
{
	CVBFormat m_VertexFormat;
	std::vector<unsigned char> m_VertexData;
};

struct CRenderable_AbstractIndexBuffer
{
	std::vector< unsigned long > m_IndexData;
};

enum OBJECT_PRIMITIVE_TYPE
{
	POINT_LIST		= 1,
	LINE_LIST		= 2,
	LINE_STRIP		= 3,
	TRIANGLE_LIST	= 4,
	TRIANGLE_STRIP	= 5,
	TRIANGLE_FAN	= 6
};

struct CRenderable_AbstractRenderCall
{
	CRenderable_AbstractRenderCall() : m_VB(-1), m_BaseVertex(0), m_IB(-1), m_NPrimitives(-1) {}

	// CRenderable_AbstractVertexBuffer& rVB = m_VertexBuffers[m_VB];
	// is used in this render call.
	long m_VB;

	// Base vertex to start rendering with,
	// in case when indexing is not used.
	long m_BaseVertex;

	// CRenderable_AbstractIndexBuffer& rIB = m_IndexBuffers[m_IB];
	// is used in this render call.
	// If negative, then indexing is not used, and m_BaseVertexIndex is ignored.
	long m_IB;

	// Base index to start rendering with,
	// in case when indexing is used.
	long m_BaseVertexIndex;
	
	long m_NPrimitives;
	OBJECT_PRIMITIVE_TYPE m_PrimitiveType;
};

struct CRenderable_AbstractGeometry
{
	std::vector< CRenderable_AbstractVertexBuffer > m_VertexBuffers;
	std::vector< CRenderable_AbstractIndexBuffer  > m_IndexBuffers;
	std::vector< CRenderable_AbstractRenderCall   > m_RenderCalls;
};

/*

Primeri:

CRenderable_AbstractRenderCall = {22,-1,666,1,LINE_LIST}
Iz vertex bufera m_VertexBuffers[22] renderitsya 1 line segment.
Vertex-i 0,1.

CRenderable_AbstractRenderCall = {17,-1,666,2,TRIANGLE_STRIP}
Iz vertex bufera m_VertexBuffers[17] renderitsya 2 treugolnika strip-om.
Vertex-i 0,1,2, 2,1,3.

CRenderable_AbstractRenderCall = {11,3,4,2,TRIANGLE_STRIP}
Ispolzuem index buffer 3.
Pust` m_IndexBuffers[3] = {0,0,0,0,666,888,777,555,0,0,0}
Iz vertex bufera m_VertexBuffers[11] renderitsya 2 treugolnika strip-om.
Vertex-i 666,888,777, 777,888,555.

*/

#endif














