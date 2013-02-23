#ifndef RENDERABLE_DX8_HPP_INCLUDED
#define RENDERABLE_DX8_HPP_INCLUDED

#include "material_dx8.h"
#include "geometry_dx8.h"
#include <boost/shared_ptr.hpp>

//struct CRenderable_DX8 {
//	CRenderable_MaterialDX8 m_Material;
//	CRenderable_GeometryDX8 m_Geometry;
//};

struct CRenderable_BoneRemapDX8 {
	std::vector< unsigned char > m_UsedBones;
	std::vector< CRenderable_GeometryDX8 > m_Geometries;
};

#endif












