#ifndef OBJECT_RENDERABLE_HPP_INCLUDED
#define OBJECT_RENDERABLE_HPP_INCLUDED

//#include "material.h"
#include "geometry.h"
#include <vector>

//struct CRenderable_Abstract
//{
//	CRenderable_AbstractMaterial m_Material;
//	CRenderable_AbstractGeometry m_Geometry;
//};

struct CRenderable_BoneRemap {
	std::vector< unsigned char > m_UsedBones;
	std::vector< CRenderable_AbstractGeometry > m_Geometries;
};

#endif





